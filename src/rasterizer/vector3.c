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

    if (len == 0.0f || len == 1.0f)
        return;

    len = 1.0f / sqrtf(len);

    vec->x *= len;
    vec->y *= len;
    vec->z *= len;
}

void _pr_vector_aprx_normalize3(pr_vector3* vec)
{
    PRfloat invLen = _aprx_inv_sqrt(PR_SQ(vec->x) + PR_SQ(vec->y) + PR_SQ(vec->z));

    vec->x *= invLen;
    vec->y *= invLen;
    vec->z *= invLen;
}


