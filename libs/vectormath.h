#include <stdint.h>

//Vector2
struct Vector2 {
    float x;
    float y;
} typedef v2;

v2 Addv2s(v2 V1, v2 V2)
{
    v2 newv2;
    newv2.x = V1.x + V2.x;
    newv2.y = V1.y + V2.y;
    return newv2;
}

v2 Subv2s(v2 V1, v2 V2)
{
    v2 newv2;
    newv2.x = V1.x - V2.x;
    newv2.y = V1.y - V2.y;
    return newv2;
}

v2 Multiv2s(v2 V1, v2 V2)
{
    v2 newv2;
    newv2.x = V1.x * V2.x;
    newv2.y = V1.y * V2.y;
    return newv2;
}

v2 Divv2s(v2 V1, v2 V2)
{
    v2 newv2;
    newv2.x = V1.x / V2.x;
    newv2.y = V1.y / V2.y;
    return newv2;
}

v2 Scalev2(v2 v, float scalefactor)
{
    v2 newv2;
    newv2.x = v.x * scalefactor;
    newv2.y = v.y * scalefactor;
    return newv2;
}