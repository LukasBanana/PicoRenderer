/*
 * vector3.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VECTOR3_H__
#define __PR_VECTOR3_H__


#include "types.h"


typedef struct pr_vector3
{
    PRfloat x;
    PRfloat y;
    PRfloat z;
}
pr_vector3;


void _pr_vector_normalize3(pr_vector3* vec);


#endif
