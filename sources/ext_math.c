/*
 * ext_math.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "ext_math.h"

#define _USE_MATH_DEFINES
#include <math.h>


PRfloat _aprx_inv_sqrt(PRfloat x)
{
    float x2 = x * 0.5f;
    float y = x;
    long i = *(long*)(&y);      // Evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);  // Magic number
    y = *(float*)(&i);
    y = y * (1.5f - (x2*y*y));  // 1st iteration
    return y;
}

PRfloat _aprx_sin(PRfloat x)
{
    // Always wrap input angle to [-PI .. PI]
    if (x < -PR_MATH_PI)
        x = fmodf(x - PR_MATH_PI, PR_MATH_PI*2.0f) + PR_MATH_PI;
    else if (x > PR_MATH_PI)
        x = fmodf(x + PR_MATH_PI, PR_MATH_PI*2.0f) - PR_MATH_PI;

    // Compute sine
    float y;

    if (x < 0)
    {
        y = 1.27323954f * x + 0.405284735f * x*x;

        if (y < 0)
            y = 0.225f * (y*(-y) - y) + y;
        else
            y = 0.225f * (y*y - y) + y;
    }
    else
    {
        y = 1.27323954f * x - 0.405284735f * x*x;

        if (y < 0)
            y = 0.225f * (y*(-y) - y) + y;
        else
            y = 0.225f * (y*y - y) + y;
    }

    return y;
}

PRfloat _aprx_cos(PRfloat x)
{
    return _aprx_sin(x + PR_MATH_PI*0.5f);
}
