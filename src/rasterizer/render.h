/*
 * render.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_RENDER_H__
#define __PR_RENDER_H__


#include "types.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"


void _pr_render_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer);
void _pr_render_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer);
void _pr_render_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer);

void _pr_render_indexed_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer);
void _pr_render_indexed_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer);
void _pr_render_indexed_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer);


#endif
