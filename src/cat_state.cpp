// Cat Animations
#define CA_SITDOWN 0
#define CA_LOOKAROUND 1
#define CA_LAYDOWN 2
#define CA_WALK 3
#define CA_RUN1 4
#define CA_RUN2 5

struct animation
{
    int animId;
    int frameStart;
    int frameEnd;
    int frameDir;
};

struct cat_state
{
    int * transitions;
    int transitionsCount;

    int animId;
    int animFrame;

    int animOverride;
};

struct transition
{
    int animation;
    int next_state;
};

// Defining States
#define CATSTATE_SITTING 0
#define CATSTATE_LOOKING_AROUND 1
#define CATSTATE_LAYING_DOWN 2
#define CATSTATE_WALKING 3
#define CATSTATE_RUNNING 4

// Defining Transitions
#define CATTRANS_SD_LA 0
#define CATTRANS_SD_LD 1
#define CATTRANS_SD_WA 2
#define CATTRANS_SD_RN 3

// Defining Animations

struct animation animations[10] = {

};

struct transition transitions[10] = {

};

struct cat_state states[10] = {
    {
        {}
    }
};

// states[0].