/*
 * vertexbuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VERTEXBUFFER_H__
#define __PR_VERTEXBUFFER_H__


#include "vertex.h"


typedef struct pr_vertexbuffer
{
    PRsizei numVertices;
    pr_vertex* vertices;
}
pr_vertexbuffer;


pr_vertexbuffer* _pr_gen_vertexbuffer(PRsizei numVertices);
void _pr_delete_vertexbuffer(pr_vertexbuffer* vertexbuffer);


#endif
