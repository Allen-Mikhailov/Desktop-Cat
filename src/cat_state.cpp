// Cat Animations
#define CA_SITDOWN 0
#define CA_LOOKAROUND 1
#define CA_LAYDOWN 2
#define CA_WALK 3
#define CA_RUN1 4
#define CA_RUN2 5

struct keyframe
{
    int frame;
    double time;
};

struct animation
{
    int animId;
    struct keyframe keyframes[20];
    int keyframeCount;
    double speedMulti;
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

#define CATTRANS_WA_SD 5
#define CATTRANS_RN_SD 6

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
        {
            {0, 1},
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1}
        },
        6,
        1.0
    },

    // CATANIM_STANDUP
    {
        CA_SITDOWN,
        {
            {5, 1},
            {4, 1},
            {3, 1},
            {2, 1},
            {1, 1},
            {0, 1},
        },
        6,
        1.0
    },

    // CATANIM_LOOKAROUND
    {
        CA_LOOKAROUND,
        {
            {2, 1},
            {1, 1},
            {0, 4},
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 4},
            {3, 1},
            {2, 1}
        },
        9,
        1.0
    },

    // CATANIM_LAYDOWN
    {
        CA_LAYDOWN,
        {
            {0, 1},
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1},
            {6, 1},
            {7, 1}
        },
        8,
        1.0
    },

    // CATANIM_GETUP
    {
        CA_LAYDOWN,
        {
            {7, 1},
            {6, 1},
            {5, 1},
            {4, 1},
            {3, 1},
            {2, 1},
            {1, 1},
            {0, 1},
        },
        8,
        1.0
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
        CATANIM_STANDUP,
        CATSTATE_WALKING,
    },

    // CATTRANS_SD_RN
    {
        CATANIM_STANDUP,
        CATSTATE_RUNNING,
    },

    // CATTRANS_LD_SD
    {
        CATANIM_GETUP,
        CATSTATE_SITTING
    },

    // CATTRANS_WA_SD
    {
        CATANIM_SITDOWN,
        CATSTATE_SITTING,
    },

    // CATTRANS_RN_SD
    {
        CATANIM_SITDOWN,
        CATSTATE_SITTING,
    }
};

struct cat_state states[10] = {
    // CATSTATE_SITTING
    {
        {CATTRANS_SD_LA, CATTRANS_SD_LD, CATTRANS_SD_RN},
        {1, 1, 10},
        3,

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
    },

    // CATSTATE_WALKING
    {
        {CATTRANS_WA_SD},
        {1},
        1,
    },

    // CATSTATE_RUNNING
    {
        {CATTRANS_RN_SD},
        {1},
        1,
    },
};