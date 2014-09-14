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


// --- points --- //

void _pr_render_screenspace_point(PRint x, PRint y, PRubyte colorIndex);

void _pr_render_points(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_points(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);

// --- lines --- //

void _pr_render_screenspace_line(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex);

void _pr_render_lines(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);
void _pr_render_line_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);
void _pr_render_line_loop(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_lines(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_line_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_line_loop(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);

// --- images --- //

void _pr_render_screenspace_image(PRint left, PRint top, PRint right, PRint bottom);

// --- triangles --- //

void _pr_render_triangles(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);
void _pr_render_triangle_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);
void _pr_render_triangle_fan(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_triangles(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_triangle_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_triangle_fan(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);


#endif
