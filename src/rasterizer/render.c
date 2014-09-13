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
#include "static_config.h"


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
        _pr_error_set(PR_ERROR_INVALID_STATE, __FUNCTION__);
        return;
    }
    
    if ( x < 0 || x >= (PRint)framebuffer->width ||
         y < 0 || y >= (PRint)framebuffer->height )
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    #ifdef PR_ORIGIN_LEFT_TOP
    y = framebuffer->height - y - 1;
    #endif

    // Plot screen space point
    _pr_framebuffer_plot(framebuffer, x, y, colorIndex);
}

void _pr_render_points(PRuint numVertices, PRuint firstVertex, pr_vertexbuffer* vertexbuffer)
{
    // Validate bound framebuffer
    pr_framebuffer* framebuffer = _stateMachine.boundFramebuffer;

    if (framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_INVALID_STATE, __FUNCTION__);
        return;
    }

    // Validate vertex buffer
    if (vertexbuffer == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }

    if (firstVertex + numVertices >= vertexbuffer->numVertices)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }
     
    // Transform vertices
    _vertexbuffer_transform(numVertices, firstVertex, vertexbuffer);
    
    // Render points
    pr_vertex* vert;

    PRuint x, y;
    PRuint width = framebuffer->width, height = framebuffer->height;

    for (; firstVertex < numVertices; ++firstVertex)
    {
        vert = (vertexbuffer->vertices + firstVertex);

        x = (PRuint)(vert->ndc.x);
        #ifdef PR_ORIGIN_LEFT_TOP
        y = framebuffer->height - (PRuint)(vert->ndc.y) - 1;
        #else
        y = (PRuint)(vert->ndc.y);
        #endif

        if (x < width && y < height)
            _pr_framebuffer_plot(framebuffer, x, y, _stateMachine.colorIndex);
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
        _pr_error_set(PR_ERROR_INVALID_STATE, __FUNCTION__);
        return;
    }
    
    if ( x1 < 0 || x1 >= (PRint)framebuffer->width ||
         x2 < 0 || x2 >= (PRint)framebuffer->width ||
         y1 < 0 || y1 >= (PRint)framebuffer->height ||
         y2 < 0 || y2 >= (PRint)framebuffer->height )
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    #ifdef PR_ORIGIN_LEFT_TOP
    y1 = framebuffer->height - y1 - 1;
    y2 = framebuffer->height - y2 - 1;
    #endif

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

// --- images --- //

static void _render_screenspace_image_textured(
    pr_framebuffer* framebuffer, pr_texture* texture, PRint left, PRint top, PRint right, PRint bottom)
{
    // Clamp rectangle
    left    = PR_CLAMP(left, 0, (PRint)framebuffer->width - 1);
    top     = PR_CLAMP(top, 0, (PRint)framebuffer->height - 1);
    right   = PR_CLAMP(right, 0, (PRint)framebuffer->width - 1);
    bottom  = PR_CLAMP(bottom, 0, (PRint)framebuffer->height - 1);

    // Flip vertical
    #ifdef PR_ORIGIN_LEFT_TOP
    top = framebuffer->height - top - 1;
    bottom = framebuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        PR_SWAP(PRint, top, bottom);
    if (left > right)
        PR_SWAP(PRint, left, right);

    // Select MIP level
    PRtexsize width, height;
    PRubyte mipLevel = _pr_texutre_compute_miplevel(texture, (PRfloat)(right - left)*(bottom - top), 1.0f);
    PRubyte* texels = _pr_texture_select_miplevel(texture, mipLevel, &width, &height);

    // Rasterize rectangle
    pr_pixel* pixels = framebuffer->pixels;
    const PRuint pitch = framebuffer->width;
    pr_pixel* scaline;

    PRfloat u = 0.0f;
    #ifdef PR_ORIGIN_LEFT_TOP
    PRfloat v = 1.0f;
    #else
    PRfloat v = 0.0f;
    #endif

    const PRfloat uStep = 1.0f / ((PRfloat)(right - left));
    const PRfloat vStep = 1.0f / ((PRfloat)(bottom - top));

    for (PRint y = top; y <= bottom; ++y)
    {
        scaline = pixels + (y * pitch + left);
        
        u = 0.0f;

        for (PRint x = left; x <= right; ++x)
        {
            scaline->colorIndex = _pr_texture_sample_nearest(texels, width, height, u, v);
            ++scaline;
            u += uStep;
        }

        #ifdef PR_ORIGIN_LEFT_TOP
        v -= vStep;
        #else
        v += vStep;
        #endif
    }
}

static void _render_screenspace_image_colored(
    pr_framebuffer* framebuffer, PRubyte colorIndex, PRint left, PRint top, PRint right, PRint bottom)
{
    // Clamp rectangle
    left    = PR_CLAMP(left, 0, (PRint)framebuffer->width - 1);
    top     = PR_CLAMP(top, 0, (PRint)framebuffer->height - 1);
    right   = PR_CLAMP(right, 0, (PRint)framebuffer->width - 1);
    bottom  = PR_CLAMP(bottom, 0, (PRint)framebuffer->height - 1);

    // Flip vertical
    #ifdef PR_ORIGIN_LEFT_TOP
    top = framebuffer->height - top - 1;
    bottom = framebuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        PR_SWAP(PRint, top, bottom);
    if (left > right)
        PR_SWAP(PRint, left, right);

    // Rasterize rectangle
    pr_pixel* pixels = framebuffer->pixels;
    const PRuint pitch = framebuffer->width;
    pr_pixel* scaline;

    for (PRint y = top; y <= bottom; ++y)
    {
        scaline = pixels + (y * pitch + left);
        for (PRint x = left; x <= right; ++x)
        {
            scaline->colorIndex = colorIndex;
            ++scaline;
        }
    }
}

void _pr_render_screenspace_image(PRint left, PRint top, PRint right, PRint bottom)
{
    if (_stateMachine.boundFramebuffer != NULL)
    {
        if (_stateMachine.boundTexture != NULL)
            _render_screenspace_image_textured(_stateMachine.boundFramebuffer, _stateMachine.boundTexture, left, top, right, bottom);
        else
            _render_screenspace_image_colored(_stateMachine.boundFramebuffer, _stateMachine.colorIndex, left, top, right, bottom);
    }
    else
        _pr_error_set(PR_ERROR_INVALID_STATE, __FUNCTION__);
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

