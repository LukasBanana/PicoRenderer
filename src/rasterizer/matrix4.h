/*
 * matrix4.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_MATRIX4_H
#define PR_MATRIX4_H


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


void _pr_matrix_load_identity(pr_matrix4* matrix);
void _pr_matrix_copy(pr_matrix4* dst, const pr_matrix4* src);

void _pr_matrix_mul_float3(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs);
void _pr_matrix_mul_float4(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs);

void _pr_matrix_mul_vector3(pr_vector3* result, const pr_matrix4* lhs, const pr_vector3* rhs);
void _pr_matrix_mul_vector4(pr_vector4* result, const pr_matrix4* lhs, const pr_vector4* rhs);

void _pr_matrix_mul_matrix(pr_matrix4* result, const pr_matrix4* lhs, const pr_matrix4* rhs);

void _pr_matrix_translate(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z);
void _pr_matrix_rotate(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z, PRfloat angle);
void _pr_matrix_scale(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z);

void _pr_matrix_build_perspective(pr_matrix4* result, PRfloat aspectRatio, PRfloat nearPlane, PRfloat farPlane, PRfloat fov);
void _pr_matrix_build_orthogonal(pr_matrix4* result, PRfloat width, PRfloat height, PRfloat nearPlane, PRfloat farPlane);


#endif
