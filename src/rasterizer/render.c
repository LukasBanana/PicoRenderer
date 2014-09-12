/*
 * render.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "render.h"
#include "state_machine.h"
#include "raster_triangle.h"
#include "ext_math.h"
#include "error.h"


// --- internals ---

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

    //...
}

// --- points --- //

void _pr_render_screenspace_point(PRint x, PRint y, PRubyte colorIndex)
{
    // Validate bound framebuffer
    pr_framebuffer* framebuffer = _stateMachine.boundFramebuffer;

    if (framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_INVALID_STATE);
        return;
    }
    
    if ( x < 0 || x >= (PRint)framebuffer->width ||
         y < 0 || y >= (PRint)framebuffer->height )
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    if (_stateMachine.originLeftTop)
        y = framebuffer->height - y - 1;

    // Plot screen space point
    _pr_framebuffer_plot(framebuffer, x, y, colorIndex);
}

void _pr_render_points(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    // Validate bound framebuffer
    pr_framebuffer* framebuffer = _stateMachine.boundFramebuffer;

    if (framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_INVALID_STATE);
        return;
    }

    // Validate vertex buffer
    if (vertexbuffer == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER);
        return;
    }

    if (firstVertex + numVertices >= vertexbuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return;
    }
    
    // Transform vertices
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    
    // Render points
    pr_vertex* vert;

    PRuint x, y;
    PRuint width = framebuffer->width, height = framebuffer->height;

    if (_stateMachine.originLeftTop)
    {
        for (; firstVertex < numVertices; ++firstVertex)
        {
            vert = (vertexbuffer->vertices + firstVertex);

            x = (PRuint)(vert->ndc.x);
            y = framebuffer->height - (PRuint)(vert->ndc.y) - 1;

            if (x < width && y < height)
                _pr_framebuffer_plot(framebuffer, x, y, _stateMachine.colorIndex);
        }
    }
    else
    {
        for (; firstVertex < numVertices; ++firstVertex)
        {
            vert = (vertexbuffer->vertices + firstVertex);

            x = (PRuint)(vert->ndc.x);
            y = (PRuint)(vert->ndc.y);

            if (x < width && y < height)
                _pr_framebuffer_plot(framebuffer, x, y, _stateMachine.colorIndex);
        }
    }
}

void _pr_render_indexed_points(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

// --- lines --- //

// This function implements the line "bresenham" algorithm.
void _pr_render_screenspace_line(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex)
{
    // Get bound framebuffer
    pr_framebuffer* framebuffer = _stateMachine.boundFramebuffer;

    if (framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_INVALID_STATE);
        return;
    }
    
    if ( x1 < 0 || x1 >= (PRint)framebuffer->width ||
         x2 < 0 || x2 >= (PRint)framebuffer->width ||
         y1 < 0 || y1 >= (PRint)framebuffer->height ||
         y2 < 0 || y2 >= (PRint)framebuffer->height )
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    if (_stateMachine.originLeftTop)
    {
        y1 = framebuffer->height - y1 - 1;
        y2 = framebuffer->height - y2 - 1;
    }

    // Pre-compuations
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    int incx = PR_SIGN(dx);
    int incy = PR_SIGN(dy);
    
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    
    int pdx, pdy, ddx, ddy, es, el;
    
    if (dx > dy)
    {
        pdx = incx;
        pdy = 0;
        ddx = incx;
        ddy = incy;
        es  = dy;
        el  = dx;
    }
    else
    {
        pdx = 0;
        pdy = incy;
        ddx = incx;
        ddy = incy;
        es  = dx;
        el  = dy;
    }
    
    if (el == 0)
        return;
    
    int x   = x1;
    int y   = y1;
    int err = el/2;
    
    // Render each pixel of the line
    for (int t = 0; t < el; ++t)
    {
        // Render pixel
        _pr_framebuffer_plot(_stateMachine.boundFramebuffer, (PRuint)x, (PRuint)y, colorIndex);

        // Move to next pixel
        err -= es;
        if (err < 0)
        {
            err += el;
            x += ddx;
            y += ddy;
        }
        else
        {
            x += pdx;
            y += pdy;
        }
    }
}

void _pr_render_lines(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_line_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_line_loop(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_indexed_lines(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

void _pr_render_indexed_line_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

void _pr_render_indexed_line_loop(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

// --- triangles --- //

void _pr_render_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    //...
}

void _pr_render_indexed_triangles(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

void _pr_render_indexed_triangle_strip(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

void _pr_render_indexed_triangle_fan(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer, const pr_indexbuffer* indexbuffer)
{
    _vertexbuffer_transform_all(vertexbuffer);
    //...
}

