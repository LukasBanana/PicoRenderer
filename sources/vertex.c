/*
 * vertex.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "vertex.h"


void _pr_transform_vertex(pr_vertex* vertex, const pr_matrix4* modelViewMatrix, const pr_matrix4* projectionMatrix)
{
    // Transform vertex coordinate by model-view matrix into view-space (VS)
    pr_vector4 coordVS;
    _pr_matrix_mul_float3(&(coordVS.x), modelViewMatrix, &(vertex->coord.x));
    coordVS.w = 1.0f;

    // Transform view-space coordinate into projection space
    _pr_matrix_mul_float4(&(vertex->ndc.x), projectionMatrix, &(coordVS.x));

    // Transform coordinate into normalized device coordinates
    float rhw = 1.0f / vertex->ndc.w;
    vertex->ndc.x *= rhw;
    vertex->ndc.y *= rhw;
    vertex->ndc.z *= rhw;

    //...
}
