/*
 * vector3.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_VECTOR3_H
#define PR_VECTOR3_H


#include "types.h"


typedef struct pr_vector3
{
    PRfloat x;
    PRfloat y;
    PRfloat z;
}
pr_vector3;


void _pr_vector_normalize3(pr_vector3* vec);

PR_INLINE PRfloat _pr_vector3_dot(pr_vector3 a, pr_vector3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


#endif
