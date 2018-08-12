/*
 * vector4.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_VECTOR4_H
#define PR_VECTOR4_H


#include "types.h"


typedef struct pr_vector4
{
    PRfloat x;
    PRfloat y;
    PRfloat z;
    PRfloat w;
}
pr_vector4;


PR_INLINE PRfloat _pr_vector4_dot(pr_vector4 a, pr_vector4 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}


#endif
