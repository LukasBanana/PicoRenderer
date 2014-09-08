/*
 * render.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "render.h"
#include "state_machine.h"
#include "raster_triangle.h"


static void _vertexbuffer_transform(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _pr_vertexbuffer_transform(
        numVertices,
        firstVertex,
        vertexbuffer,
        &(_stateMachine.modelViewMatrix),
        &(_stateMachine.projectionMatrix),
        &(_stateMachine.viewport)
    );
}

static void _vertexbuffer_transform_all(pr_vertexbuffer* vertexbuffer)
{
    _pr_vertexbuffer_transform_all(
        vertexbuffer,
        &(_stateMachine.modelViewMatrix),
        &(_stateMachine.projectionMatrix),
        &(_stateMachine.viewport)
    );
}

static void _setup_triangle_vertex(pr_raster_vertex* rasterVert, const pr_vertex* vert)
{
    rasterVert->x = (PRint)(vert->ndc.x + 0.5f);
    rasterVert->y = (PRint)(vert->ndc.y + 0.5f);
    rasterVert->z = vert->ndc.z;
    rasterVert->u = vert->invTexCoord.x;
    rasterVert->v = vert->invTexCoord.y;
}

static void _raster_triangle(pr_vertex* a, pr_vertex* b, pr_vertex* c)
{
    // To raster a triangle we copy the vertex data into a 'raster_triangle' structure,
    // which is much smaller and compact to reduce memory overhead.
    // This is necessary because during triangle rasterization we will
    // access this memory a lot of times.
    pr_raster_triangle triangle;

    _setup_triangle_vertex(&(triangle.a), a);
    _setup_triangle_vertex(&(triangle.b), b);
    _setup_triangle_vertex(&(triangle.c), c);




}

void _pr_render_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);


}

void _pr_render_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);


}

void _pr_render_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);


}

void _pr_render_indexed_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);


}

void _pr_render_indexed_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);


}

void _pr_render_indexed_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);


}
