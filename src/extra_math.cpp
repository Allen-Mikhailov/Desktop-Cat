#include <cmath>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

using namespace std;

double sign(double v)
{
    if (v == 0)
        return v;
    return v / abs(v);
}

double realisticQuadratic(double a, double b, double c)
{
    double r1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
    double r2 = (-b - sqrt(b*b - 4*a*c))/(2*a);

    if (r1 <= 0)
        return r2;
    return r1;
}

int intfromHex(const char* str)
{
    return (int)strtol(str, NULL, 16);
}

int getRectWidth(RECT rect)
{
    return rect.right-rect.left;
}

int getRectHeight(RECT rect)
{
    return rect.bottom-rect.top;
}

