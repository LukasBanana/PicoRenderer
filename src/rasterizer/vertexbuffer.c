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


pr_vertexbuffer* _pr_vertexbuffer_create()
{
    pr_vertexbuffer* vertexBuffer = PR_MALLOC(pr_vertexbuffer);
    
    vertexBuffer->numVertices   = 0;
    vertexBuffer->vertices      = NULL;

    return vertexBuffer;
}

void _pr_vertexbuffer_delete(pr_vertexbuffer* vertexBuffer)
{
    if (vertexBuffer != NULL)
    {
        PR_FREE(vertexBuffer->vertices);
        PR_FREE(vertexBuffer);
    }
}

static void _vertex_transform(
    pr_vertex* vertex, const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    // Transform view-space coordinate into projection space
    pr_vector4 coordVS;
    
    coordVS.x = vertex->coord.x;
    coordVS.y = vertex->coord.y;
    coordVS.z = vertex->coord.z;
    coordVS.w = 1.0f;

    _pr_matrix_mul_float4(&(vertex->ndc.x), worldViewProjectionMatrix, &(coordVS.x));

    // Transform coordinate into normalized device coordinates
    PRfloat rhw = 1.0f / vertex->ndc.w;
    vertex->ndc.x *= rhw;
    vertex->ndc.y *= rhw;
    vertex->ndc.z *= rhw;

    // Transform vertex to screen coordiante
    vertex->ndc.x = viewport->x + vertex->ndc.x * viewport->width;
    vertex->ndc.y = viewport->y + vertex->ndc.y * viewport->height;
    //...
}

void _pr_vertexbuffer_transform(
    PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexBuffer,
    const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    const PRuint lastVertex = numVertices + firstVertex;

    if (lastVertex >= vertexBuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INDEX_OUT_OF_BOUNDS, __FUNCTION__);
        return;
    }

    for (PRuint i = firstVertex; i < lastVertex; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

void _pr_vertexbuffer_transform_all(
    pr_vertexbuffer* vertexBuffer, const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    for (PRuint i = 0; i < vertexBuffer->numVertices; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

void _pr_vertexbuffer_data(pr_vertexbuffer* vertexBuffer, const PRvertex* vertices, PRsizei numVertices)
{
    if (vertexBuffer == NULL || vertices == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    // Check if vertex buffer must be reallocated
    if (vertexBuffer->vertices == NULL || vertexBuffer->numVertices != numVertices)
    {
        // Create new vertex buffer data
        PR_FREE(vertexBuffer->vertices);

        vertexBuffer->numVertices   = numVertices;
        vertexBuffer->vertices      = PR_CALLOC(pr_vertex, numVertices);
    }

    // Fill vertex buffer
    pr_vertex* vert = vertexBuffer->vertices;

    while (numVertices-- > 0)
    {
        vert->coord.x = vertices->x;
        vert->coord.y = vertices->y;
        vert->coord.z = vertices->z;

        vert->invTexCoord.x = 1.0f / vertices->u;
        vert->invTexCoord.y = 1.0f / vertices->v;

        ++vert;
        ++vertices;
    }
}

