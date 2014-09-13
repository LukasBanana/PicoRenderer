/*
 * ext_math.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_EXT_MATH_H__
#define __PR_EXT_MATH_H__


#include "types.h"


#define PR_MATH_PI          3.14159265359f

#define PR_MIN(a, b)        ((a) < (b) ? (a) : (b))
#define PR_MAX(a, b)        ((a) > (b) ? (a) : (b))

#define PR_CLAMP(x, a, b)   ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

#define PR_SIGN(x)          (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)

#define PR_SQ(x)            ((x)*(x))

#define PR_SWAP(t, a, b)    \
    {                       \
        t _tmp = a;         \
        a = b;              \
        b = _tmp;           \
    }


//! Computes an approximated and fast reciprocal square root.
PRfloat _aprx_inv_sqrt(PRfloat x);

//! Computes an approximated and fast sine.
PRfloat _aprx_sin(PRfloat x);

//! Computes an approximated and fast cosine.
PRfloat _aprx_cos(PRfloat x);


#endif
