/*
 * vertexbuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "vertexbuffer.h"

#include <stdlib.h>


pr_vertexbuffer* _pr_gen_vertexbuffer(PRsizei numVertices)
{
    pr_vertexbuffer* vertexbuffer = (pr_vertexbuffer*)malloc(sizeof(pr_vertexbuffer));
    
    vertexbuffer->numVertices = numVertices;
    vertexbuffer->vertices = (pr_vertex*)calloc(numVertices, sizeof(pr_vertex));

    return vertexbuffer;
}

void _pr_delete_vertexbuffer(pr_vertexbuffer* vertexbuffer)
{
    if (vertexbuffer != NULL && vertexbuffer->vertices != NULL)
    {
        free(vertexbuffer->vertices);
        free(vertexbuffer);
    }
}
