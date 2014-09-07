/*
 * math.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_MATH_H__
#define __PR_MATH_H__


#include "types.h"


#define PR_MATH_PI 3.14159265359f


//! Computes an approximated and fast reciprocal square root.
PRfloat _aprx_inv_sqrt(PRfloat x);

//! Computes an approximated and fast sine.
PRfloat _aprx_sin(PRfloat x);

//! Computes an approximated and fast cosine.
PRfloat _aprx_cos(PRfloat x);


#endif
