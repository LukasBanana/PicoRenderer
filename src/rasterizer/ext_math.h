/*
 * ext_math.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_EXT_MATH_H__
#define __PR_EXT_MATH_H__


#include "types.h"
#include "static_config.h"
#include "consts.h"


#define PR_MIN(a, b)            ((a) < (b) ? (a) : (b))
#define PR_MAX(a, b)            ((a) > (b) ? (a) : (b))

#define PR_CLAMP(x, a, b)       ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

#define PR_CLAMP_LARGEST(x, c)  if ((x) < (c)) x = c
#define PR_CLAMP_SMALLEST(x, c) if ((x) > (c)) x = c

#define PR_SIGN(x)              (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)

#define PR_SQ(x)                ((x)*(x))

#define PR_SWAP(t, a, b)    \
    {                       \
        t _tmp = a;         \
        a = b;              \
        b = _tmp;           \
    }

#define PR_SWAP_INT(a, b)   \
    {                       \
        a ^= b;             \
        b ^= a;             \
        a ^= b;             \
    }

#ifdef PR_FAST_MATH
#   define PR_SIN(x)        _aprx_sin(x)
#   define PR_COS(x)        _aprx_cos(x)
#   define PR_TAN(x)        (_aprx_sin(x)/_aprx_cos(x))
#   define PR_INV_SQRT(x)   _aprx_inv_sqrt(x)
#   define PR_LOG2(x)       _aprx_log2(x)
#   define PR_LOG(x)        _aprx_log(x)
#else
#   define PR_SIN(x)        sinf(x)
#   define PR_COS(x)        cosf(x)
#   define PR_TAN(x)        tanf(x)
#   define PR_INV_SQRT(x)   (1.0f / sqrtf(x))
#   define PR_LOG2(x)       log2f(x)
#   define PR_LOG(x)        logf(x)
#endif


//! Computes an approximated and fast reciprocal square root.
PRfloat _aprx_inv_sqrt(PRfloat x);

//! Computes an approximated and fast sine.
PRfloat _aprx_sin(PRfloat x);

//! Computes an approximated and fast cosine.
PRfloat _aprx_cos(PRfloat x);

//! Computes an approximated and fast logarithm of base 2.
PRfloat _aprx_log2(PRfloat x);

//! Computes an approximated and fast logarithm of base e (natural logartihm 'ln').
PRfloat _aprx_log(PRfloat x);

//! Computes the integral value of the logarithm of base 2.
PRint _int_log2(PRfloat x);


#endif
