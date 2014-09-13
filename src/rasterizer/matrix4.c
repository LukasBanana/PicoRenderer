/*
 * matrix4.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "matrix4.h"
#include "error.h"

#include <string.h>
#include <math.h>


void _pr_matrix_load_identity(pr_matrix4* matrix)
{
    matrix->m[0][0] = 1.0f; matrix->m[1][0] = 0.0f; matrix->m[2][0] = 0.0f; matrix->m[3][0] = 0.0f;
    matrix->m[0][1] = 0.0f; matrix->m[1][1] = 1.0f; matrix->m[2][1] = 0.0f; matrix->m[3][1] = 0.0f;
    matrix->m[0][2] = 0.0f; matrix->m[1][2] = 0.0f; matrix->m[2][2] = 1.0f; matrix->m[3][2] = 0.0f;
    matrix->m[0][3] = 0.0f; matrix->m[1][3] = 0.0f; matrix->m[2][3] = 0.0f; matrix->m[3][3] = 1.0f;
}

void _pr_matrix_copy(pr_matrix4* dst, const pr_matrix4* src)
{
    memcpy(dst, src, sizeof(pr_matrix4));
}

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
    PRfloat* m3 = &(result->m[0][0]);
    const PRfloat* m1 = &(lhs->m[0][0]);
    const PRfloat* m2 = &(rhs->m[0][0]);

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

void _pr_matrix_translate(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z)
{
    PRfloat* m = &(result->m[0][0]);
    m[12] += ( m[0]*x + m[4]*y + m[ 8]*z );
    m[13] += ( m[1]*x + m[5]*y + m[ 9]*z );
    m[14] += ( m[2]*x + m[6]*y + m[10]*z );
    m[15] += ( m[3]*x + m[7]*y + m[11]*z );
}

void _pr_matrix_rotate(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z, PRfloat angle)
{
    // Normalize rotation vector
    PRfloat len = x*x + y*y + z*z;

    if (len != 0.0f && len != 1.0f)
    {
        PRfloat invLen = 1.0f / sqrtf(len);
        x *= invLen;
        y *= invLen;
        z *= invLen;
    }

    // Pre-compute rotation values
    const PRfloat s = sinf(angle);
    const PRfloat c = cosf(angle);
    const PRfloat cc = 1.0f - c;

    // Setup rotation matrix
    pr_matrix4 rhs;
    PRfloat* m = &(rhs.m[0][0]);

    m[0] = x*x*cc + c;   m[4] = x*y*cc - z*s; m[ 8] = x*z*cc + y*s; m[12] = 0.0f;
    m[1] = y*x*cc + z*s; m[5] = y*y*cc + c;   m[ 9] = y*z*cc - x*s; m[13] = 0.0f;
    m[2] = x*z*cc - y*s; m[6] = y*z*cc + x*s; m[10] = z*z*cc + c;   m[14] = 0.0f;
    m[3] = 0.0f;         m[7] = 0.0f;         m[11] = 0.0f;         m[15] = 1.0f;

    // Multiply input matrix (lhs) with rotation matrix (rhs)
    pr_matrix4 lhs;
    _pr_matrix_copy(&lhs, result);

    _pr_matrix_mul_matrix(result, &lhs, &rhs);
}

void _pr_matrix_scale(pr_matrix4* result, PRfloat x, PRfloat y, PRfloat z)
{
    PRfloat* m = &(result->m[0][0]);
    m[0] *= x; m[4] *= x; m[ 8] *= x; m[12] *= x;
    m[1] *= y; m[5] *= y; m[ 9] *= y; m[13] *= y;
    m[2] *= z; m[6] *= z; m[10] *= z; m[14] *= z;
}

void _pr_matrix_build_perspective(pr_matrix4* result, PRfloat aspectRatio, PRfloat nearPlane, PRfloat farPlane, PRfloat fov)
{
    if (result == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }

    const PRfloat h = 1.0f / tanf(fov * 0.5f);
    const PRfloat w = h / aspectRatio;

    PRfloat* m = (PRfloat*)(result->m);

    m[ 0] = w;
    m[ 1] = 0.0f;
    m[ 2] = 0.0f;
    m[ 3] = 0.0f;

    m[ 4] = 0.0f;
    m[ 5] = h;
    m[ 6] = 0.0f;
    m[ 7] = 0.0f;

    m[ 8] = 0.0f;
    m[ 9] = 0.0f;
    m[10] = farPlane/(farPlane - nearPlane);
    m[11] = 1.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -(farPlane*nearPlane)/(farPlane - nearPlane);
    m[15] = 0.0f;
}

void _pr_matrix_build_orthogonal(pr_matrix4* result, PRfloat width, PRfloat height, PRfloat nearPlane, PRfloat farPlane)
{
    if (result == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }

    PRfloat* m = (PRfloat*)(result->m);

    m[ 0] = 2.0f / width;
    m[ 1] = 0.0f;
    m[ 2] = 0.0f;
    m[ 3] = 0.0f;

    m[ 4] = 0.0f;
    m[ 5] = 2.0f / width;
    m[ 6] = 0.0f;
    m[ 7] = 0.0f;

    m[ 8] = 0.0f;
    m[ 9] = 0.0f;
    m[10] = 1.0f/(farPlane - nearPlane);
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -nearPlane/(farPlane - nearPlane);
    m[15] = 1.0f;
}

