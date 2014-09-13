/*
 * vertexbuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "vertexbuffer.h"
#include "state_machine.h"
#include "error.h"
#include "helper.h"

#include <stdlib.h>


pr_vertexbuffer* _pr_vertexbuffer_create(PRsizei numVertices)
{
    pr_vertexbuffer* vertexbuffer = PR_MALLOC(pr_vertexbuffer);
    
    vertexbuffer->numVertices = numVertices;
    vertexbuffer->vertices = PR_CALLOC(pr_vertex, numVertices);

    return vertexbuffer;
}

void _pr_vertexbuffer_delete(pr_vertexbuffer* vertexbuffer)
{
    if (vertexbuffer != NULL && vertexbuffer->vertices != NULL)
    {
        free(vertexbuffer->vertices);
        free(vertexbuffer);
    }
}

static void _vertex_transform(
    pr_vertex* vertex, const pr_matrix4* modelViewMatrix, const pr_matrix4* projectionMatrix, const pr_viewport* viewport)
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

    // Transform vertex to screen coordiante
    vertex->ndc.x = viewport->x + vertex->ndc.x * viewport->width;
    vertex->ndc.y = viewport->y + vertex->ndc.y * viewport->height;
    //...
}

void _pr_vertexbuffer_transform(
    PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer,
    const pr_matrix4* modelViewMatrix, const pr_matrix4* projectionMatrix, const pr_viewport* viewport)
{
    const PRuint lastVertex = numVertices + firstVertex;

    if (lastVertex >= vertexbuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INDEX_OUT_OF_BOUNDS, __FUNCTION__);
        return;
    }

    for (PRuint i = firstVertex; i < lastVertex; ++i)
    {
        _vertex_transform(
            (vertexbuffer->vertices + i),
            modelViewMatrix,
            projectionMatrix,
            viewport
        );
    }
}

void _pr_vertexbuffer_transform_all(
    pr_vertexbuffer* vertexbuffer, const pr_matrix4* modelViewMatrix, const pr_matrix4* projectionMatrix, const pr_viewport* viewport)
{
    for (PRuint i = 0; i < vertexbuffer->numVertices; ++i)
    {
        _vertex_transform(
            (vertexbuffer->vertices + i),
            modelViewMatrix,
            projectionMatrix,
            viewport
        );
    }
}
