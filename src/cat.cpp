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

HDC catSheetHDC;
HBITMAP catSpriteMap;

HDC shadowCatSpriteHDC;
HBITMAP shadowCatSpriteMap;

// Cat Variables
int catAnimDir = 4;
int catAnim = CA_RUN2;

int catState = CATSTATE_SITTING;
int catTransition = -1;
int catAnimKeyframe = 0;
double catAnimSpeed = 6;
double catAnimKeyframeTimer = 0;

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

HBRUSH shadowBrush = CreateSolidBrush(RGB(100, 100, 100));

using namespace std;

const int BORDER_PADING = 200;
void changeCatTarget(int client_width, int client_height)
{
    catTargetOffsetX = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_width   - BORDER_PADING * 2 ) );
    catTargetOffsetY = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_height  - BORDER_PADING * 2 ) );
}

HPEN hPen = CreatePen(PS_NULL, 1, RGB(255, 0, 0));

void init(HWND window, HDC hdc)
{
    // SetTimer(window, 2, targetResetTime, NULL); 

    SelectObject(hdc, hPen);

    // Getting the path of the sprite sheet
    char filePath[MAX_PATH]; 
    GetModuleFileNameA(NULL, filePath, MAX_PATH);

    char buildPath[MAX_PATH];
    pop_path(buildPath, filePath);

    char catSpritesPath[MAX_PATH];
    strcpy(catSpritesPath, buildPath);
    strcat(catSpritesPath, "\\Cats.bmp");

    // 1x 1024 544
    // 2x 2048, 1088
    // 3x 3072, 1632
    catSheetHDC = CreateCompatibleDC(hdc);
    error_check("create stuff4");
    catSpriteMap = (HBITMAP) LoadImageA(NULL, catSpritesPath, IMAGE_BITMAP, 
        1024*SPRITE_SCALE, 544*SPRITE_SCALE, LR_LOADFROMFILE);
    if (catSpriteMap == NULL)
        printf("Failed to load spritemap\n");

    error_check("create stuff3");
    shadowCatSpriteHDC = CreateCompatibleDC(hdc);
    error_check("create stuff2");
    shadowCatSpriteMap = createCompatibleBitmap(hdc, SPRITE_SHEET_WIDTH, SPRITE_SHEET_HEIGHT);
    SelectObject(shadowCatSpriteHDC, shadowCatSpriteMap);
    SelectObject(shadowCatSpriteHDC, hPen);

    // Filling with transparent Background
    RECT rect = {0, 0, SPRITE_SHEET_WIDTH, SPRITE_SHEET_HEIGHT};
    FillRect(shadowCatSpriteHDC, &rect, transparentBrush);

    SelectObject(shadowCatSpriteHDC, shadowBrush);

    // Shadow Placement
    for (int x = 0; x < 24; x++)
    {
        for (int y = 0; y < 17; y++)
        {
            Ellipse(shadowCatSpriteHDC, 
                x*SPRITE_UNIT+SPRITE_UNIT/10, 
                y*SPRITE_UNIT+SPRITE_UNIT/2, 
                (x+1)*SPRITE_UNIT-SPRITE_UNIT/10, 
                (y+1)*SPRITE_UNIT)-SPRITE_UNIT/2;
        }
    }

    BitBlt(shadowCatSpriteHDC, 0, 0, SPRITE_SHEET_WIDTH, SPRITE_SHEET_HEIGHT, catSheetHDC, 0, 0, SRCCOPY);

    PBITMAPINFO pi = CreateBitmapInfoStruct(window, shadowCatSpriteMap);
    CreateBMPFile(window, (LPTSTR) "./shadows.bmp", pi, shadowCatSpriteMap, shadowCatSpriteHDC);


    SelectObject(catSheetHDC, catSpriteMap);
    // GetObject(catSpriteMap, sizeof(bitmap), &bitmap);

    changeCatTarget(client_width, client_height);
}

void DrawCat(int x, int y, int anim, int dir, int frame, HDC hdc, HDC catSheetHDC)
{
    int mapX = anim*4 + frame%4;
    int mapY = dir*2 + frame/4 + 1;
    BitBlt(hdc, x, y, SPRITE_UNIT, SPRITE_UNIT, catSheetHDC, SPRITE_UNIT*mapX, SPRITE_UNIT*mapY, SRCCOPY);
}

int CatDirFromAngle(double angle)
{
    int anim = (int)((angle+M_PI/8) / (M_PI/4))%8;
    return (6 - anim + 8)%8;
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
}

void start_transition(int transitionId)
{
    catTransition = transitionId;
    catState = -1;

    struct transition *trans = &transitions[transitionId];
    struct animation *newAnimation = &animations[trans->animation];
    // catAnim = newAnimation->animId;
    catAnimKeyframe = 0;
    catAnimKeyframeTimer = 0;

    printf("Starting Transition to %d\n", catTransition);
}

int pick_view_direction()
{
    int directions = sizeof(viewDirections) / sizeof(int);

    return viewDirections[(int) ((double) rand() /  RAND_MAX * directions)];
}

int snap_cat_direction(int dir)
{
    if (dir >= 2 && dir <= 4)
        return 1;
    else if (dir == 5 || dir == 6)
        return 7;
    return dir;
}

void set_cat_state(int newState)
{
    catState = newState;
    catTransition = -1;
    printf("Setting State %d\n", catState);
}

void transition_from_state()
{
    struct cat_state *state = &states[catState];
    state_change_timer = 0;
    int newTransition = random_from_weights(state->transitionWeights, state->transitionsCount);

    if (newTransition == -1)
        printf("No transition selected for some reason idk\n");

    start_transition(state->transitions[newTransition]);
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

    switch (catState)
    {
        case CATSTATE_RUNNING:
            update_running_cat_(delta_time);
            state_change_timer += delta_time;
            if (state_change_timer > state_change_length)
            {
                catAnimDir = snap_cat_direction(catAnimDir);

                transition_from_state();
            }
            break;
        case CATSTATE_WALKING:
            break;
        default:
            update_cat_state(delta_time);
    }
    // update_running_cat_(delta_time);
    DrawCat((int) catX, (int) catY, catAnim, catAnimDir, (int) (catAnimF)%8, hdc, catSheetHDC);
    coverSpriteDisplacement(hdc, transparentBrush, pCatX, pCatY, catX, catY, SPRITE_UNIT, SPRITE_UNIT);
}

void destroy(HWND window)
{
    ReleaseDC(window, catSheetHDC);
    DeleteObject(catSpriteMap);
    DeleteObject(hPen);
}   