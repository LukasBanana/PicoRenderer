/*
 * vector2.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VECTOR2_H__
#define __PR_VECTOR2_H__


#include "types.h"


typedef struct pr_vector2
{
    PRfloat x;
    PRfloat y;
}
pr_vector2;


PR_INLINE PRfloat _pr_vector2_dot(pr_vector2 a, pr_vector2 b)
{
    return a.x*b.x + a.y*b.y;
}


#endif
