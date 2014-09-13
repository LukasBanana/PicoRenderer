/*
 * vector3.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "vector3.h"
#include "ext_math.h"

#include <math.h>


void _pr_vector_normalize3(pr_vector3* vec)
{
    PRfloat len = PR_SQ(vec->x) + PR_SQ(vec->y) + PR_SQ(vec->z);
    if (len != 0.0f && len != 1.0f)
    {
        len = PR_INV_SQRT(len);
        vec->x *= len;
        vec->y *= len;
        vec->z *= len;
    }
}

