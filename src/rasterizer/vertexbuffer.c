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

    // Transform vertex to screen coordiante (+0.5 is for rounding adjustment)
    vertex->ndc.x = viewport->x + (vertex->ndc.x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->ndc.y = viewport->y + (vertex->ndc.y + 1.0f) * viewport->halfHeight + 0.5f;
    vertex->ndc.z = viewport->minDepth + vertex->ndc.z * viewport->depthSize;
}

void _pr_vertexbuffer_transform(
    PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer,
    const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    const PRushort lastVertex = numVertices + firstVertex;

    if (lastVertex >= vertexBuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INDEX_OUT_OF_BOUNDS, __FUNCTION__);
        return;
    }

    for (PRushort i = firstVertex; i < lastVertex; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

void _pr_vertexbuffer_transform_all(
    pr_vertexbuffer* vertexBuffer, const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    for (PRushort i = 0; i < vertexBuffer->numVertices; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

static _vertexbuffer_resize(pr_vertexbuffer* vertexBuffer, PRushort numVertices)
{
    // Check if vertex buffer must be reallocated
    if (vertexBuffer->vertices == NULL || vertexBuffer->numVertices != numVertices)
    {
        // Create new vertex buffer data
        PR_FREE(vertexBuffer->vertices);

        vertexBuffer->numVertices   = numVertices;
        vertexBuffer->vertices      = PR_CALLOC(pr_vertex, numVertices);
    }
}

void _pr_vertexbuffer_data(pr_vertexbuffer* vertexBuffer, const PRvertex* vertices, PRushort numVertices)
{
    if (vertexBuffer == NULL || vertices == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    _vertexbuffer_resize(vertexBuffer, numVertices);

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

void _pr_vertexbuffer_data_from_file(pr_vertexbuffer* vertexBuffer, PRushort* numVertices, FILE* file)
{
    if (vertexBuffer == NULL || numVertices == NULL || file == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }
    
    // Read number of vertices
    fread(numVertices, sizeof(PRushort), 1, file);

    _vertexbuffer_resize(vertexBuffer, *numVertices);

    // Read all vertices
    PRvertex data;
    pr_vertex* vert = vertexBuffer->vertices;

    for (PRushort i = 0; i < *numVertices; ++i)
    {
        if (feof(file))
        {
            PR_ERROR(PR_ERROR_UNEXPECTED_EOF);
            return;
        }

        // Read next vertex data
        fread(&data, sizeof(PRvertex), 1, file);

        vert->coord.x = data.x;
        vert->coord.y = data.y;
        vert->coord.z = data.z;

        vert->invTexCoord.x = 1.0f / data.u;
        vert->invTexCoord.y = 1.0f / data.v;

        ++vert;
    }
}

