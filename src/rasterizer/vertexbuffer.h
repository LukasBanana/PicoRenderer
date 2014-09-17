/*
 * vertexbuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VERTEXBUFFER_H__
#define __PR_VERTEXBUFFER_H__


#include "vertex.h"
#include "viewport.h"
#include "structs.h"

#include <stdio.h>


typedef struct pr_vertexbuffer
{
    PRsizei numVertices;
    pr_vertex* vertices;
}
pr_vertexbuffer;


pr_vertexbuffer* _pr_vertexbuffer_create();
void _pr_vertexbuffer_delete(pr_vertexbuffer* vertexBuffer);

void _pr_vertexbuffer_transform(
    PRsizei numVertices,
    PRsizei firstVertex,
    pr_vertexbuffer* vertexBuffer,
    const pr_matrix4* worldViewProjectionMatrix,
    const pr_viewport* viewport
);

void _pr_vertexbuffer_transform_all(
    pr_vertexbuffer* vertexBuffer,
    const pr_matrix4* worldViewProjectionMatrix,
    const pr_viewport* viewport
);

void _pr_vertexbuffer_data(pr_vertexbuffer* vertexBuffer, PRsizei numVertices, const PRvoid* coords, const PRvoid* texCoords, PRsizei vertexStride);
void _pr_vertexbuffer_data_from_file(pr_vertexbuffer* vertexBuffer, PRsizei* numVertices, FILE* file);


#endif
