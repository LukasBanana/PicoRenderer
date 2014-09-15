/*
 * ext_math.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "ext_math.h"

#define _USE_MATH_DEFINES
#include <math.h>


// Fast inverse square root from "Quake III Arena"
// See http://en.wikipedia.org/wiki/Fast_inverse_square_root
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

// Fast and accurate sine approximation
// See http://lab.polygonal.de/?p=205
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

// See http://stackoverflow.com/questions/9411823/fast-log2float-x-implementation-c
PRfloat _aprx_log2(PRfloat x)
{
    long* const expPtr = (long*)(&x);
    long y = *expPtr;
    const long lg2 = ((y >> 23) & 255) - 128;
    
    y &= ~(255 << 23);
    y += 127 << 23;
    
    *expPtr = y;

    x = ((-1.0f/3) * x + 2) * x - 2.0f/3;

    return (x + lg2);
}

PRfloat _aprx_log(PRfloat x)
{
    return _aprx_log2(x) * 0.69314718f;
}

PRint _int_log2(PRfloat x)
{
    #ifdef PR_FAST_MATH
    unsigned long* ix = (unsigned long*)(&x);
    unsigned long exp = ((*ix) >> 23) & 0xff;
    return (PRint)exp - 127;
    #else
    int y;
    frexpf(x, &y);
    return y - 1;
    #endif
}

