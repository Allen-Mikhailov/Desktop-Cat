#include <windows.h>
#include <stdint.h>
#include <dwmapi.h>
#include <stdlib.h>

#include <string.h>

#include <stdio.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include "window_setup.cpp"
#include "vector2.cpp"
#include "sprite_functions.cpp"
#include "file_extras.cpp"
#include "cat_state.cpp"

// #define CAT_SHADOW

HDC catSheetHDC;
HBITMAP catSpriteMap;

HDC shadowCatSpriteHDC;
HBITMAP shadowCatSpriteMap;

HDC maskHDC;
HBITMAP maskMap;

// Cat Variables
int catAnimDir = 0;
int catAnim = CA_RUN2;

int catState = CATSTATE_SITTING;
int catTransition = -1;
int catAnimKeyframe = 0;
double catAnimSpeed = 6;
double catAnimKeyframeTimer = 0;

double catWalkTargetX = 0;
double catWalkTargetY = 0;

const double maxCatWalkDistance = 600;
const double minCatWalkDistance = 200;
const double catWalkPadding = 200;
const double catWalkSpeed = 50;
const double walkAnimSpeed = 4;

// Dragging
int catDragOffsetX = 0;
int catDragOffsetY = 0;
int draggingCat = 0;

double state_change_timer = 0;
const double state_change_length = 3;

double catAnimF = 0;

double catX = 500;
double catY = 500;

double catVelocity = 0;
double catDirection = 0;

const int targetResetTime = 1000;
double catTargetOffsetX = 0, catTargetOffsetY = 0;

const double catTurnSpeed = 5;
const double catVelocityCap = 500;
const double catAcceleration = 1500;
const double catAnimationSpeed = 25;
const double FRICTION = .97;

char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

const int SPRITE_SCALE = 3;
const int SPRITE_SIZE = 32;
const int SPRITE_UNIT = SPRITE_SCALE*SPRITE_SIZE;

#define SPRITE_SHEET_WIDTH SPRITE_SCALE * 1024
#define SPRITE_SHEET_HEIGHT SPRITE_SCALE * 544

HBRUSH shadowBrush = CreateSolidBrush(RGB(75, 75, 75));
HPEN hPen = CreatePen(PS_NULL, 1, RGB(255, 0, 0));

using namespace std;

const int BORDER_PADING = 200;
void changeCatTarget(int client_width, int client_height)
{
    catTargetOffsetX = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_width   - BORDER_PADING * 2 ) );
    catTargetOffsetY = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_height  - BORDER_PADING * 2 ) );
}

int CatDirFromAngle(double angle)
{
    angle = fmod(M_PI*2 + angle, M_PI*2);
    int anim = (int)((angle+M_PI/8) / (M_PI/4))%8;
    return (6 - anim + 8)%8;
}

void fillBMInfoHeader(BITMAPINFOHEADER *bi, HBITMAP hbitmap)
{
    BITMAP bitmapInfo;
    GetObject(hbitmap, sizeof(BITMAP), &bitmapInfo);

    bi->biSize = sizeof(BITMAPINFOHEADER);
    bi->biWidth = bitmapInfo.bmWidth;
    bi->biHeight = -bitmapInfo.bmHeight;  // Negative height to ensure top-down DIB
    bi->biPlanes = 1;
    bi->biBitCount = bitmapInfo.bmBitsPixel;
    bi->biCompression = BI_RGB;
    bi->biSizeImage = 0;
    bi->biXPelsPerMeter = 0;
    bi->biYPelsPerMeter = 0;
    bi->biClrUsed = 0;
    bi->biClrImportant = 0;
}

void init(HWND window, HDC hdc)
{
    // SetTimer(window, 2, targetResetTime, NULL); 

    SelectObject(hdc, hPen);

    // Getting the path of the sprite sheet
    char filePath[MAX_PATH] = ""; 
    GetModuleFileNameA(NULL, filePath, MAX_PATH);

    char buildPath[MAX_PATH] = "";
    pop_path(buildPath, filePath);

    char catSpritesPath[MAX_PATH] = "";
    strcpy(catSpritesPath, buildPath);
    strcat(catSpritesPath, "\\Cats.bmp");

    // 1x 1024 544
    // 2x 2048, 1088
    // 3x 3072, 1632
    catSheetHDC = CreateCompatibleDC(hdc);
    catSpriteMap = (HBITMAP) LoadImageA(NULL, catSpritesPath, IMAGE_BITMAP, 
        1024*SPRITE_SCALE, 544*SPRITE_SCALE, LR_LOADFROMFILE);
    if (catSpriteMap == NULL)
        printf("Failed to load spritemap\n");
    SelectObject(catSheetHDC, catSpriteMap);
    SetBkColor(catSheetHDC, RGB(255, 255, 255));

    // Creating Shadow Map
    shadowCatSpriteHDC = CreateCompatibleDC(hdc);
    shadowCatSpriteMap = createCompatibleBitmap(hdc, SPRITE_SHEET_WIDTH, SPRITE_SHEET_HEIGHT);
    SelectObject(shadowCatSpriteHDC, shadowCatSpriteMap);
    // SelectObject(shadowCatSpriteHDC, hPen);

    // Filling with transparent Background
    RECT rect = {0, 0, SPRITE_SHEET_WIDTH, SPRITE_SHEET_HEIGHT};
    FillRect(shadowCatSpriteHDC, &rect, transparentBrush);
    SelectObject(shadowCatSpriteHDC, shadowBrush);

    // Shadow Placement
    for (int x = 0; x < 24; x++)
    {
        for (int y = 0; y < 17; y++)
        {
            #ifdef CAT_SHADOW
            Ellipse(shadowCatSpriteHDC, 
                x*SPRITE_UNIT+SPRITE_UNIT/10, 
                y*SPRITE_UNIT+SPRITE_UNIT/3, 
                (x+1)*SPRITE_UNIT-SPRITE_UNIT/10, 
                (y+1)*SPRITE_UNIT-SPRITE_UNIT/10);
            #endif
        }
    }

    printf("Starting to draw shadow map\n");

    BITMAPINFOHEADER spriteSheetBitMapHeader; 
    fillBMInfoHeader(&spriteSheetBitMapHeader, shadowCatSpriteMap);

    BYTE* spriteMapBits = new BYTE[SPRITE_SHEET_WIDTH * SPRITE_SHEET_HEIGHT * (32 / 8)];
    BYTE* shadowMapBits = new BYTE[SPRITE_SHEET_WIDTH * SPRITE_SHEET_HEIGHT * (32 / 8)];

    if (!GetDIBits(catSheetHDC, catSpriteMap, 0, SPRITE_SHEET_HEIGHT, 
        spriteMapBits, (BITMAPINFO*)&spriteSheetBitMapHeader, DIB_RGB_COLORS))
        error_check("GetDIBts 1");

    if (!GetDIBits(shadowCatSpriteHDC, shadowCatSpriteMap, 0, SPRITE_SHEET_HEIGHT, 
        shadowMapBits, (BITMAPINFO*)&spriteSheetBitMapHeader, DIB_RGB_COLORS))
        error_check("GetDIBts 2");

    for (int y = 0; y < SPRITE_SHEET_HEIGHT; ++y) {
        for (int x = 0; x < SPRITE_SHEET_WIDTH; ++x) {
            int index = (y * SPRITE_SHEET_WIDTH + x) * (32 / 8);

            // Check if the pixel color matches the ignoreColor
            if (!(spriteMapBits[index] == 255 
                && spriteMapBits[index+1] == 255 
                && spriteMapBits[index+2] == 255)) {
                shadowMapBits[index] = spriteMapBits[index];
                shadowMapBits[index+1] = spriteMapBits[index+1];
                shadowMapBits[index+2] = spriteMapBits[index+2];
            }
        }
    }

    if (!SetDIBits(shadowCatSpriteHDC, shadowCatSpriteMap, 0, SPRITE_SHEET_HEIGHT, 
        shadowMapBits, (BITMAPINFO*)&spriteSheetBitMapHeader, DIB_RGB_COLORS))
        printf("Setting DIBits");

    delete[] spriteMapBits;
    delete[] shadowMapBits;

    changeCatTarget(client_width, client_height);
}

void DrawCat(int x, int y, int anim, int dir, int frame, HDC hdc, HDC catSheetHDC)
{
    int mapX = anim*4 + frame%4;
    int mapY = dir*2 + frame/4 + 1;
    BitBlt(hdc, x, y, SPRITE_UNIT, SPRITE_UNIT, catSheetHDC, SPRITE_UNIT*mapX, SPRITE_UNIT*mapY, SRCCOPY);
}

int snap_cat_direction(int dir)
{
    if (dir >= 2 && dir <= 4)
        return 1;
    else if (dir == 5 || dir == 6)
        return 7;
    return dir;
}

int find_walk_point()
{
    int pathAttempts = 0;
    while (pathAttempts < 10)
    {   
        pathAttempts++;

        int dir = (int) (random()*8);
        double dirAngle = (double) dir * M_PI/4;

        double distance = minCatWalkDistance + random()*(maxCatWalkDistance - minCatWalkDistance);
        int newX = catX+SPRITE_UNIT/2 + (int)(cos(dirAngle)*distance);
        int newY = catY+SPRITE_UNIT/2 + (int)(sin(dirAngle)*distance);

        // Checking for outa bounds
        if (newX < catWalkPadding || newX > client_width-catWalkPadding
            || newY < catWalkPadding || newY > client_height-catWalkPadding)
            continue;

        double newAngle = atan2(-sin(dirAngle), cos(dirAngle));

        catWalkTargetX = newX-SPRITE_UNIT/2;
        catWalkTargetY = newY-SPRITE_UNIT/2;
        catAnimDir = CatDirFromAngle(newAngle);

        printf("Walking from %d %d to %d %d\n", (int) catX, (int) catY, (int) catWalkTargetX, (int) catWalkTargetY);
        return 1;
    }
    return 0;
}

void start_transition(int transitionId)
{
    catTransition = transitionId;
    catState = -1;

    struct transition *trans = &transitions[transitionId];
    struct animation *newAnimation = &animations[trans->animation];
    catAnimKeyframe = 0;
    catAnimKeyframeTimer = 0;

    printf("Starting Transition to %d\n", catTransition);
}

void transition_from_state()
{
    catAnimDir = snap_cat_direction(catAnimDir);
    struct cat_state *state = &states[catState];
    state_change_timer = 0;
    int newTransition = random_from_weights(state->transitionWeights, state->transitionsCount);

    if (newTransition == -1)
        printf("No transition selected for some reason idk\n");

    start_transition(state->transitions[newTransition]);
}

void set_cat_state(int newState)
{
    catState = newState;
    catTransition = -1;
    printf("Setting State %d\n", catState);

    switch (newState)
    {
        case CATSTATE_WALKING:
            if (!find_walk_point())
                transition_from_state();
    }
}

int pick_view_direction()
{
    int directions = sizeof(viewDirections) / sizeof(int);
    return viewDirections[(int) ((double) rand() /  RAND_MAX * directions)];
}

int mouse1(int down)
{
    POINT p;
    GetCursorPos(&p);
    if (down)
    {
        if (p.x-catX < SPRITE_UNIT && p.x-catX > 0 
            && p.y-catY < SPRITE_UNIT && p.y-catY > 0)
        {
            // set_cat_state(CATSTATE_DRAGGING);
            catDragOffsetX = catX-p.x;
            catDragOffsetY = catY-p.y;
            draggingCat = 1;

            return 1;
        }
    } else {
        if (draggingCat) {
            // transition_from_state();
            draggingCat = 0;
            catWalkTargetX = catX;
            catWalkTargetY = catY;
            return 1;
        }
    }
    return 0;
}

void update_dragging_cat(double delta_time)
{
    POINT p;
    GetCursorPos(&p);

    int pX = catX;
    int pY = catY;

    catX = p.x+catDragOffsetX;
    catY = p.y+catDragOffsetY;

    double angle = fmod(M_PI*2 + atan2(-(catY-pY), catX-pX), M_PI*2);
    // catAnimDir = CatDirFromAngle(angle);
    // catAnim = CA_SITDOWN;
    // catAnimF = 0;
}   

void update_walking_cat(double delta_time)
{
    catAnim = CA_WALK;
    double xDif = catWalkTargetX-catX;
    double yDif = catWalkTargetY-catY;

    double angle = atan2(yDif, xDif);

    double xDist = min(abs(xDif), catWalkSpeed*delta_time*abs(cos(angle)));
    double yDist = min(abs(yDif), catWalkSpeed*delta_time*abs(sin(angle)));

    catAnimF = fmod( catAnimF + delta_time * walkAnimSpeed, 4 );

    catX += xDist*sign(xDif);
    catY += yDist*sign(yDif);

    if (abs(catX-catWalkTargetX) < 1 && abs(catY-catWalkTargetY) < 1)
    {
        printf("Finished walking\n");
        transition_from_state();
    }
}

void update_running_cat_(double delta_time)
{
    catAnim = CA_RUN2;

    POINT p;
    GetCursorPos(&p);

    int targetX;
    int targetY;

    // Mouse Targeting
    targetX = p.x;
    targetY = p.y;

    // Random Targeting
    // targetX = catTargetOffsetX;
    // targetY = catTargetOffsetY;

    double xDiff = (targetX-catX-SPRITE_UNIT/2);
    double yDiff = (targetY-catY-SPRITE_UNIT/2);
    double c = hypot(xDiff, yDiff);

    // Direction
    double targetDir = fmod(M_PI*2 + atan2(-yDiff, xDiff), M_PI*2);
    double rotDif = targetDir-catDirection;

    // printf("diff: %f %f, c: %f\n", xDiff, yDiff, c);
    // printf("rot: %f, rotT: %f, diff: %f\n", catDirection, targetDir, rotDif);

    if (rotDif != 0)
    {
        double rotSign = sign(rotDif);
        if (abs(rotDif) > M_PI)
            rotSign *= -1;

        if (abs(catTurnSpeed*delta_time) > abs(rotDif))
            catDirection += rotDif;
        else 
            catDirection += rotSign*catTurnSpeed*delta_time;

        catDirection = fmod(M_PI*2 + catDirection, M_PI*2);

        catAnimDir = CatDirFromAngle(catDirection);
    }

    if (c != 0)
    {
        double estimatedTime = realisticQuadratic(
            -catAcceleration/2,
            catVelocity,
            -c
        );

        if (estimatedTime < catVelocity/catAcceleration)
            catVelocity =  max(catVelocity - catAcceleration * delta_time, 0.0);
        else
            catVelocity =  min(catVelocity + catAcceleration * delta_time, catVelocityCap);

        double dif = min(delta_time*catVelocity, c);
        if (xDiff != 0)
            catX += cos(catDirection)*dif;
        if (yDiff != 0)
            catY += -sin(catDirection)*dif;
    }

    catAnimF = fmod(catAnimF+delta_time*catVelocity/catVelocityCap*catAnimationSpeed, 8);

    state_change_timer += delta_time;
    if (state_change_timer > state_change_length)
        transition_from_state();
}

void update_cat_state(double delta_time)
{
    if (catTransition == -1)
    {
        // in a state
        struct cat_state *state = &states[catState];
        catAnimF = state->animFrame;
        catAnim = state->animId;

        // Handing the weird stuff with north and south sitting
        if (catState == CATSTATE_SITTING && (catAnimDir == 0 || catAnimDir == 4))
            catAnimF = 6;

        state_change_timer += delta_time;
        if (state_change_timer > state_change_length)
        {
            transition_from_state();
        }
    } 
    
    if (catTransition != -1) {
        // In a transition
        struct transition *trans = &transitions[catTransition];
        struct animation *anim = &animations[trans->animation];
        struct keyframe *Keyframe = &anim->keyframes[catAnimKeyframe];

        double adjustedSpeed = catAnimSpeed*anim->speedMulti;

        catAnimKeyframeTimer += delta_time * adjustedSpeed;
        if (catAnimKeyframeTimer > Keyframe->time)
        {
            // Dealing with any leftover  delta_time
            catAnimKeyframeTimer -= Keyframe->time;
            catAnimKeyframe++;

            printf("Passed: %d\n",Keyframe->frame);

            if (catAnimKeyframe >= anim->keyframeCount)
            {
                set_cat_state(trans->next_state);
                return;
            }

            Keyframe = &anim->keyframes[catAnimKeyframe];
        }

        catAnim = anim->animId;
        catAnimF = Keyframe->frame;
    }
}

void update(double delta_time)
{
    // To get rid of the black lines by doing an Initial fill of transparentBrush
    if (frames == 0)
    {
        RECT rect = {0, 0, client_width, client_height};
        FillRect(hdc, &rect, transparentBrush);
    }

    int pCatX = catX;
    int pCatY = catY;

    if (draggingCat != 1)
    {
        switch (catState)
        {
            case CATSTATE_RUNNING:
                update_running_cat_(delta_time);
                break;
            case CATSTATE_WALKING:
                update_walking_cat(delta_time);
                break;
            default:
                update_cat_state(delta_time);
        }
    } else {
        update_dragging_cat(delta_time);
    }
    
    DrawCat((int) catX, (int) catY, catAnim, catAnimDir, (int) (catAnimF)%8, hdc, shadowCatSpriteHDC);
    coverSpriteDisplacement(hdc, transparentBrush, pCatX, pCatY, catX, catY, SPRITE_UNIT, SPRITE_UNIT);
    
}

void destroy(HWND window)
{
    ReleaseDC(window, catSheetHDC);
    DeleteObject(catSpriteMap);
    DeleteObject(hPen);
    DeleteObject(shadowBrush);
}   