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
};

#define MAX_TRANSITIONS 10

struct cat_state
{
    int transitions[MAX_TRANSITIONS];
    int transitionWeights[MAX_TRANSITIONS];
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
#define CATSTATE_LAYING_DOWN 1
#define CATSTATE_WALKING 2
#define CATSTATE_RUNNING 3
#define CATSTATE_STANDING 4
#define CATSTATE_LOOKING_AROUND 5 // probably will remain unused for now

// Defining Transitions
#define CATTRANS_SD_LA 0
#define CATTRANS_SD_LD 1
#define CATTRANS_SD_WA 2
#define CATTRANS_SD_RN 3

#define CATTRANS_LD_SD 4

// Defining Animations
#define CATANIM_SITDOWN 0
#define CATANIM_STANDUP 1
#define CATANIM_LOOKAROUND 2
#define CATANIM_LAYDOWN 3
#define CATANIM_GETUP 4

struct animation animations[10] = {
    // CATANIM_SITDOWN
    {
        CA_SITDOWN,
        0,
        5
    },

    // CATANIM_STANDUP
    {
        CA_SITDOWN,
        5,
        0
    },

    // CATANIM_LOOKAROUND
    {
        CA_LOOKAROUND,
        0,
        4
    },

    // CATANIM_LAYDOWN
    {
        CA_LAYDOWN,
        0,
        7
    },

    // CATANIM_GETUP
    {
        CA_LAYDOWN,
        7,
        0
    }
};

struct transition transitions[10] = {
    // CATTRANS_SD_LA
    {
        CATANIM_LOOKAROUND,
        CATSTATE_SITTING,
    },

    // CATTRANS_SD_LD
    {
        CATANIM_LAYDOWN,
        CATSTATE_LAYING_DOWN,
    },

    // CATTRANS_SD_WA
    {

    },

    // CATTRANS_SD_RN
    {

    },

    // CATTRANS_LD_SD
    {
        CATANIM_GETUP,
        CATSTATE_SITTING
    }
};

struct cat_state states[10] = {
    // CATSTATE_SITTING
    {
        {CATTRANS_SD_LA, CATTRANS_SD_LD},
        {1, 1},
        2,

        CA_SITDOWN,
        5,

        0
    },

    // CATSTATE_LAYING_DOWN
    {
        {CATTRANS_LD_SD},
        {1},
        1,

        CA_LAYDOWN,
        7,

        0
    }
};

// states[0].