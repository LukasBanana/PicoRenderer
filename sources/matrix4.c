/*
 * matrix4.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "matrix4.h"


void _pr_matrix_mul_float3(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs)
{
    result[0] = (lhs->m[0][0] * rhs[0]) + (lhs->m[1][0] * rhs[1]) + (lhs->m[2][0] * rhs[2]) + lhs->m[3][0];
    result[1] = (lhs->m[0][1] * rhs[0]) + (lhs->m[1][1] * rhs[1]) + (lhs->m[2][1] * rhs[2]) + lhs->m[3][1];
    result[2] = (lhs->m[0][2] * rhs[0]) + (lhs->m[1][2] * rhs[1]) + (lhs->m[2][2] * rhs[2]) + lhs->m[3][2];
}

void _pr_matrix_mul_float4(PRfloat* result, const pr_matrix4* lhs, const PRfloat* rhs)
{
    result[0] = (lhs->m[0][0] * rhs[0]) + (lhs->m[1][0] * rhs[1]) + (lhs->m[2][0] * rhs[2]) + (lhs->m[3][0] * rhs[3]);
    result[1] = (lhs->m[0][1] * rhs[0]) + (lhs->m[1][1] * rhs[1]) + (lhs->m[2][1] * rhs[2]) + (lhs->m[3][1] * rhs[3]);
    result[2] = (lhs->m[0][2] * rhs[0]) + (lhs->m[1][2] * rhs[1]) + (lhs->m[2][2] * rhs[2]) + (lhs->m[3][2] * rhs[3]);
    result[3] = (lhs->m[0][3] * rhs[0]) + (lhs->m[1][3] * rhs[1]) + (lhs->m[2][3] * rhs[2]) + (lhs->m[3][3] * rhs[3]);
}

void _pr_matrix_mul_vector3(pr_vector3* result, const pr_matrix4* lhs, const pr_vector3* rhs)
{
    _pr_matrix_mul_float3((PRfloat*)result, lhs, (const PRfloat*)rhs);
}

void _pr_matrix_mul_vector4(pr_vector4* result, const pr_matrix4* lhs, const pr_vector4* rhs)
{
    _pr_matrix_mul_float4((PRfloat*)result, lhs, (const PRfloat*)rhs);
}

void _pr_matrix_mul_matrix(pr_matrix4* result, const pr_matrix4* lhs, const pr_matrix4* rhs)
{
    float* m3 = result->m;
    const float* m1 = lhs->m;
    const float* m2 = rhs->m;

    m3[ 0] = m1[0]*m2[ 0] + m1[4]*m2[ 1] + m1[ 8]*m2[ 2] + m1[12]*m2[ 3];
    m3[ 1] = m1[1]*m2[ 0] + m1[5]*m2[ 1] + m1[ 9]*m2[ 2] + m1[13]*m2[ 3];
    m3[ 2] = m1[2]*m2[ 0] + m1[6]*m2[ 1] + m1[10]*m2[ 2] + m1[14]*m2[ 3];
    m3[ 3] = m1[3]*m2[ 0] + m1[7]*m2[ 1] + m1[11]*m2[ 2] + m1[15]*m2[ 3];

    m3[ 4] = m1[0]*m2[ 4] + m1[4]*m2[ 5] + m1[ 8]*m2[ 6] + m1[12]*m2[ 7];
    m3[ 5] = m1[1]*m2[ 4] + m1[5]*m2[ 5] + m1[ 9]*m2[ 6] + m1[13]*m2[ 7];
    m3[ 6] = m1[2]*m2[ 4] + m1[6]*m2[ 5] + m1[10]*m2[ 6] + m1[14]*m2[ 7];
    m3[ 7] = m1[3]*m2[ 4] + m1[7]*m2[ 5] + m1[11]*m2[ 6] + m1[15]*m2[ 7];

    m3[ 8] = m1[0]*m2[ 8] + m1[4]*m2[ 9] + m1[ 8]*m2[10] + m1[12]*m2[11];
    m3[ 9] = m1[1]*m2[ 8] + m1[5]*m2[ 9] + m1[ 9]*m2[10] + m1[13]*m2[11];
    m3[10] = m1[2]*m2[ 8] + m1[6]*m2[ 9] + m1[10]*m2[10] + m1[14]*m2[11];
    m3[11] = m1[3]*m2[ 8] + m1[7]*m2[ 9] + m1[11]*m2[10] + m1[15]*m2[11];

    m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[ 8]*m2[14] + m1[12]*m2[15];
    m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[ 9]*m2[14] + m1[13]*m2[15];
    m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
    m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
}
