/*
 * render.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_RENDER_H
#define PR_RENDER_H


#include "types.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"


// --- points --- //

void _pr_render_screenspace_point(PRint x, PRint y);

void _pr_render_points(PRsizei numVertices, PRsizei firstVertex, /*const */pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_points(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);

// --- lines --- //

void _pr_render_screenspace_line(PRint x1, PRint y1, PRint x2, PRint y2);

void _pr_render_lines(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);
void _pr_render_line_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);
void _pr_render_line_loop(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_lines(PRsizei numVertices, PRsizei firstVertex, /*const */pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_line_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_line_loop(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);

// --- images --- //

void _pr_render_screenspace_image(PRint left, PRint top, PRint right, PRint bottom);

// --- triangles --- //

void _pr_render_triangles(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);
void _pr_render_triangle_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);
void _pr_render_triangle_fan(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer);

void _pr_render_indexed_triangles(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_triangle_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);
void _pr_render_indexed_triangle_fan(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer);


#endif
