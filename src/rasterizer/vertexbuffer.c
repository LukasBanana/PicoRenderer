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
#include "static_config.h"

#include <stdlib.h>


pr_vertexbuffer* _pr_vertexbuffer_create()
{
    pr_vertexbuffer* vertexBuffer = PR_MALLOC(pr_vertexbuffer);

    vertexBuffer->numVertices   = 0;
    vertexBuffer->vertices      = NULL;

    _pr_ref_add(vertexBuffer);

    return vertexBuffer;
}

void _pr_vertexbuffer_delete(pr_vertexbuffer* vertexBuffer)
{
    if (vertexBuffer != NULL)
    {
        _pr_ref_release(vertexBuffer);

        PR_FREE(vertexBuffer->vertices);
        PR_FREE(vertexBuffer);
    }
}

void _pr_vertexbuffer_singular_init(pr_vertexbuffer* vertexBuffer, PRsizei numVertices)
{
    if (vertexBuffer != NULL)
    {
        vertexBuffer->numVertices   = numVertices;
        vertexBuffer->vertices      = PR_CALLOC(pr_vertex, numVertices);
    }
}

void _pr_vertexbuffer_singular_clear(pr_vertexbuffer* vertexBuffer)
{
    if (vertexBuffer != NULL)
        PR_FREE(vertexBuffer->vertices);
}

//!REMOVE THIS!
static void _vertex_transform(
    pr_vertex* vertex, const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    // Transform view-space coordinate into projection space
    _pr_matrix_mul_float4(&(vertex->ndc.x), worldViewProjectionMatrix, &(vertex->coord.x));

    // Transform coordinate into normalized device coordinates
    vertex->ndc.z = 1.0f / vertex->ndc.w;
    vertex->ndc.x *= vertex->ndc.z;
    vertex->ndc.y *= vertex->ndc.z;

    // Transform vertex to screen coordiante (+0.5 is for rounding adjustment)
    vertex->ndc.x = viewport->x + (vertex->ndc.x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->ndc.y = viewport->y + (vertex->ndc.y + 1.0f) * viewport->halfHeight + 0.5f;
    //vertex->ndc.z = viewport->minDepth + vertex->ndc.z * viewport->depthSize;

    #ifdef PR_PERSPECTIVE_CORRECTED
    // Setup inverse-texture coordinates
    vertex->invTexCoord.x = vertex->texCoord.x * vertex->ndc.z;
    vertex->invTexCoord.y = vertex->texCoord.y * vertex->ndc.z;
    #endif
}

void _pr_vertexbuffer_transform(
    PRsizei numVertices, PRsizei firstVertex, pr_vertexbuffer* vertexBuffer,
    const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    const PRsizei lastVertex = numVertices + firstVertex;

    if (lastVertex >= vertexBuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INDEX_OUT_OF_BOUNDS, __FUNCTION__);
        return;
    }

    for (PRsizei i = firstVertex; i < lastVertex; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

void _pr_vertexbuffer_transform_all(
    pr_vertexbuffer* vertexBuffer, const pr_matrix4* worldViewProjectionMatrix, const pr_viewport* viewport)
{
    for (PRsizei i = 0; i < vertexBuffer->numVertices; ++i)
        _vertex_transform((vertexBuffer->vertices + i), worldViewProjectionMatrix, viewport);
}

static void _vertexbuffer_resize(pr_vertexbuffer* vertexBuffer, PRsizei numVertices)
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

void _pr_vertexbuffer_data(pr_vertexbuffer* vertexBuffer, PRsizei numVertices, const PRvoid* coords, const PRvoid* texCoords, PRsizei vertexStride)
{
    if (vertexBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    _vertexbuffer_resize(vertexBuffer, numVertices);

    // Get offset pointers
    const PRbyte* coordsByteAlign = (const PRbyte*)coords;
    const PRbyte* texCoordsByteAlign = (const PRbyte*)texCoords;

    // Fill vertex buffer
    pr_vertex* vert = vertexBuffer->vertices;

    while (numVertices-- > 0)
    {
        // Copy coordinates
        if (coordsByteAlign != NULL)
        {
            const PRfloat* coord = (const PRfloat*)coordsByteAlign;

            vert->coord.x = coord[0];
            vert->coord.y = coord[1];
            vert->coord.z = coord[2];
            vert->coord.w = 1.0f;//coord[3];

            coordsByteAlign += vertexStride;
        }
        else
        {
            vert->coord.x = 0.0f;
            vert->coord.y = 0.0f;
            vert->coord.z = 0.0f;
            vert->coord.w = 1.0f;
        }

        // Copy texture coordinates
        if (texCoordsByteAlign != NULL)
        {
            const PRfloat* texCoord = (const PRfloat*)texCoordsByteAlign;

            vert->texCoord.x = texCoord[0];
            vert->texCoord.y = texCoord[1];

            texCoordsByteAlign += vertexStride;
        }
        else
        {
            vert->texCoord.x = 0.0f;
            vert->texCoord.y = 0.0f;
        }

        // Next vertex
        ++vert;
    }
}

void _pr_vertexbuffer_data_from_file(pr_vertexbuffer* vertexBuffer, PRsizei* numVertices, FILE* file)
{
    if (vertexBuffer == NULL || numVertices == NULL || file == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    // Read number of vertices
    PRushort vertCount = 0;
    fread(&vertCount, sizeof(PRushort), 1, file);
    *numVertices = (PRsizei)vertCount;

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
        vert->coord.w = 1.0f;

        vert->texCoord.x = data.u;
        vert->texCoord.y = data.v;

        ++vert;
    }
}

