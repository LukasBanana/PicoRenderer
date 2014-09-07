/*
 * matrix4.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_MATRIX4_H__
#define __PR_MATRIX4_H__


#include "types.h"
#include "vector3.h"
#include "vector4.h"


typedef struct pr_matrix4
{
    // column (c) and row (r)
    //      c  r
    PRfloat m[4][4];
}
pr_matrix4;


void _pr_matrix_mul_float3(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs);
void _pr_matrix_mul_float4(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs);

void _pr_matrix_mul_vector3(pr_vector3* result, const pr_matrix4* lhs, const pr_vector3* rhs);
void _pr_matrix_mul_vector4(pr_vector4* result, const pr_matrix4* lhs, const pr_vector4* rhs);

void _pr_matrix_mul_matrix(pr_matrix4* result, const pr_matrix4* lhs, const pr_matrix4* rhs);


#endif
