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

#include <stdio.h>

#include <math.h>//!!!


// --- internals ---

static void _vertexbuffer_transform(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _pr_vertexbuffer_transform(
        numVertices,
        firstVertex,
        vertexBuffer,
        &(PR_STATE_MACHINE.worldViewProjectionMatrix),
        &(PR_STATE_MACHINE.viewport)
    );
}

static void _vertexbuffer_transform_all(pr_vertexbuffer* vertexBuffer)
{
    _pr_vertexbuffer_transform_all(
        vertexBuffer,
        &(PR_STATE_MACHINE.worldViewProjectionMatrix),
        &(PR_STATE_MACHINE.viewport)
    );
}

static void _setup_triangle_vertex(pr_raster_vertex* rasterVert, const pr_vertex* vert)
{
    rasterVert->x = (PRint)(vert->ndc.x);
    rasterVert->y = (PRint)(vert->ndc.y);
    
    rasterVert->z = vert->ndc.z;

    #ifdef PR_PERSPECTIVE_CORRECTED
    rasterVert->u = vert->invTexCoord.x;
    rasterVert->v = vert->invTexCoord.y;
    #else
    rasterVert->u = vert->texCoord.x;
    rasterVert->v = vert->texCoord.y;
    #endif
}

// Returns PR_TRUE if the specified triangle vertices are culled.
static PRboolean _is_triangle_culled(const pr_vertex* a, const pr_vertex* b, const pr_vertex* c)
{
    if (PR_STATE_MACHINE.cullMode != PR_CULL_NONE)
    {
        const PRfloat vis =
            (b->ndc.x - a->ndc.x)*(c->ndc.y - a->ndc.y) -
            (b->ndc.y - a->ndc.y)*(c->ndc.x - a->ndc.x);

        if (PR_STATE_MACHINE.cullMode == PR_CULL_FRONT)
        {
            if (vis > 0.0f)
                return PR_TRUE;
        }
        else
        {
            if (vis < 0.0f)
                return PR_TRUE;
        }
    }
    return PR_FALSE;
}

// --- points --- //

void _pr_render_screenspace_point(PRint x, PRint y, PRubyte colorIndex)
{
    // Validate bound frame buffer
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    
    if ( x < 0 || x >= (PRint)frameBuffer->width ||
         y < 0 || y >= (PRint)frameBuffer->height )
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    #ifdef PR_ORIGIN_LEFT_TOP
    y = frameBuffer->height - y - 1;
    #endif

    // Plot screen space point
    _pr_framebuffer_plot(frameBuffer, x, y, colorIndex);
}

void _pr_render_points(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    // Validate bound frame buffer
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }

    // Validate vertex buffer
    if (vertexBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    if (firstVertex + numVertices >= vertexBuffer->numVertices)
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }
     
    // Transform vertices
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    
    // Render points
    pr_vertex* vert;

    PRuint x, y;
    PRuint width = frameBuffer->width, height = frameBuffer->height;

    for (; firstVertex < numVertices; ++firstVertex)
    {
        vert = (vertexBuffer->vertices + firstVertex);

        x = (PRuint)(vert->ndc.x);
        #ifdef PR_ORIGIN_LEFT_TOP
        y = frameBuffer->height - (PRuint)(vert->ndc.y) - 1;
        #else
        y = (PRuint)(vert->ndc.y);
        #endif

        if (x < width && y < height)
            _pr_framebuffer_plot(frameBuffer, x, y, PR_STATE_MACHINE.colorIndex);
    }
}

void _pr_render_indexed_points(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

// --- lines --- //

// This function implements the line "bresenham" algorithm.
static void _render_screenspace_line_colored(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex)
{
    // Get bound frame buffer
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    if ( x1 < 0 || x1 >= (PRint)frameBuffer->width ||
         x2 < 0 || x2 >= (PRint)frameBuffer->width ||
         y1 < 0 || y1 >= (PRint)frameBuffer->height ||
         y2 < 0 || y2 >= (PRint)frameBuffer->height )
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
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
        _pr_framebuffer_plot(PR_STATE_MACHINE.boundFrameBuffer, (PRuint)x, (PRuint)y, colorIndex);

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

static void _render_indexed_lines_textured(
    const pr_texture* texture, PRushort numVertices, PRushort firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

static void _render_indexed_lines_colored(
    PRubyte colorIndex, PRushort numVertices, PRushort firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (PRushort i = firstVertex, n = numVertices + firstVertex; i + 1 < n; i += 2)
    {
        // Fetch indices
        PRushort indexA = indexBuffer->indices[i];
        PRushort indexB = indexBuffer->indices[i + 1];

        #ifdef PR_DEBUG
        if (indexA >= vertexBuffer->numVertices || indexB >= vertexBuffer->numVertices)
        {
            PR_SET_ERROR_FATAL("element in index buffer out of bounds");
            return;
        }
        #endif

        // Fetch vertices
        const pr_vertex* vertexA = (vertexBuffer->vertices + indexA);
        const pr_vertex* vertexB = (vertexBuffer->vertices + indexB);

        // Raster line
        PRint x1 = (PRint)vertexA->ndc.x;
        PRint y1 = (PRint)vertexA->ndc.y;

        PRint x2 = (PRint)vertexB->ndc.x;
        PRint y2 = (PRint)vertexB->ndc.y;

        _render_screenspace_line_colored(x1, y1, x2, y2, colorIndex);
    }
}

void _pr_render_screenspace_line(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex)
{
    // Get bound frame buffer
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    if (frameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    
    #ifdef PR_ORIGIN_LEFT_TOP
    y1 = frameBuffer->height - y1 - 1;
    y2 = frameBuffer->height - y2 - 1;
    #endif

    _render_screenspace_line_colored(x1, y1, x2, y2, colorIndex);
}

void _pr_render_lines(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
}

void _pr_render_line_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

void _pr_render_line_loop(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

void _pr_render_indexed_lines(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    if (PR_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL || indexBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > indexBuffer->numIndices)
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    _vertexbuffer_transform_all(vertexBuffer);

    if (PR_STATE_MACHINE.boundTexture != NULL)
        _render_indexed_lines_textured(PR_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer, indexBuffer);
    else
        _render_indexed_lines_colored(PR_STATE_MACHINE.colorIndex, numVertices, firstVertex, vertexBuffer, indexBuffer);
}

void _pr_render_indexed_line_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    _vertexbuffer_transform_all(vertexBuffer);
    //...
}

void _pr_render_indexed_line_loop(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    _vertexbuffer_transform_all(vertexBuffer);
    //...
}

// --- images --- //

static void _render_screenspace_image_textured(const pr_texture* texture, PRint left, PRint top, PRint right, PRint bottom)
{
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Clamp rectangle
    left    = PR_CLAMP(left, 0, (PRint)frameBuffer->width - 1);
    top     = PR_CLAMP(top, 0, (PRint)frameBuffer->height - 1);
    right   = PR_CLAMP(right, 0, (PRint)frameBuffer->width - 1);
    bottom  = PR_CLAMP(bottom, 0, (PRint)frameBuffer->height - 1);

    // Flip vertical
    #ifdef PR_ORIGIN_LEFT_TOP
    top = frameBuffer->height - top - 1;
    bottom = frameBuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        PR_SWAP(PRint, top, bottom);
    if (left > right)
        PR_SWAP(PRint, left, right);

    // Select MIP level
    PRtexsize width, height;
    PRubyte mipLevel = _pr_texutre_compute_miplevel(texture, 1.0f / (PRfloat)(right - left), 0.0f, 0.0f, 1.0f / (PRfloat)(bottom - top));
    const PRubyte* texels = _pr_texture_select_miplevel(texture, mipLevel, &width, &height);

    // Rasterize rectangle
    pr_pixel* pixels = frameBuffer->pixels;
    const PRuint pitch = frameBuffer->width;
    pr_pixel* scanline;

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
        scanline = pixels + (y * pitch + left);
        
        u = 0.0f;

        for (PRint x = left; x <= right; ++x)
        {
            scanline->colorIndex = _pr_texture_sample_nearest(texels, width, height, u, v);
            ++scanline;
            u += uStep;
        }

        #ifdef PR_ORIGIN_LEFT_TOP
        v -= vStep;
        #else
        v += vStep;
        #endif
    }
}

static void _render_screenspace_image_colored(PRubyte colorIndex, PRint left, PRint top, PRint right, PRint bottom)
{
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Clamp rectangle
    left    = PR_CLAMP(left, 0, (PRint)frameBuffer->width - 1);
    top     = PR_CLAMP(top, 0, (PRint)frameBuffer->height - 1);
    right   = PR_CLAMP(right, 0, (PRint)frameBuffer->width - 1);
    bottom  = PR_CLAMP(bottom, 0, (PRint)frameBuffer->height - 1);

    // Flip vertical
    #ifdef PR_ORIGIN_LEFT_TOP
    top = frameBuffer->height - top - 1;
    bottom = frameBuffer->height - bottom - 1;
    #endif

    if (top > bottom)
        PR_SWAP(PRint, top, bottom);
    if (left > right)
        PR_SWAP(PRint, left, right);

    // Rasterize rectangle
    pr_pixel* pixels = frameBuffer->pixels;
    const PRuint pitch = frameBuffer->width;
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
    if (PR_STATE_MACHINE.boundFrameBuffer != NULL)
    {
        if (PR_STATE_MACHINE.boundTexture != NULL)
            _render_screenspace_image_textured(PR_STATE_MACHINE.boundTexture, left, top, right, bottom);
        else
            _render_screenspace_image_colored(PR_STATE_MACHINE.colorIndex, left, top, right, bottom);
    }
    else
        PR_ERROR(PR_ERROR_INVALID_STATE);
}

// --- triangles --- //

static void _index_inc(PRint* x, PRint numVertices)
{
    ++(*x);
    if (*x >= numVertices)
        *x = 0;
}

static void _index_dec(PRint* x, PRint numVertices)
{
    --(*x);
    if (*x < 0)
        *x = numVertices - 1;
}

static void _rasterize_polygon_textured(
    pr_framebuffer* frameBuffer, const pr_texture* texture, const pr_raster_vertex* vertices, PRint numVertices, PRubyte mipLevel)
{
    // Select MIP level
    PRtexsize mipWidth, mipHeight;
    const PRubyte* texels = _pr_texture_select_miplevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Find left- and right sided polygon edges
    PRint x, y, top = 0, bottom = 0;

    for (x = 1; x < numVertices; ++x)
    {
        if (vertices[top].y > vertices[x].y)
            top = x;
        if (vertices[bottom].y < vertices[x].y)
            bottom = x;
    }

    // Setup raster scanline sides
    pr_scaline_side* leftSide = frameBuffer->scanlinesStart;
    pr_scaline_side* rightSide = frameBuffer->scanlinesEnd;

    x = y = top;
    for (_index_dec(&y, numVertices); x != bottom; x = y, _index_dec(&y, numVertices))
        _pr_framebuffer_setup_scanlines(frameBuffer, leftSide, vertices[x], vertices[y]);

    x = y = top;
    for (_index_inc(&y, numVertices); x != bottom; x = y, _index_inc(&y, numVertices))
        _pr_framebuffer_setup_scanlines(frameBuffer, rightSide, vertices[x], vertices[y]);

    // Check if sides must be swaped
    long midIndex = (vertices[bottom].y + vertices[top].y) / 2;
    if (frameBuffer->scanlinesStart[midIndex].offset > frameBuffer->scanlinesEnd[midIndex].offset)
        PR_SWAP(pr_scaline_side*, leftSide, rightSide);

    // Start rasterizing the polygon
    PRint len, offset;
    PRfloat z, zAct, zStep;
    PRfloat u, uAct, uStep;
    PRfloat v, vAct, vStep;

    PRint yStart = vertices[top].y;
    PRint yEnd = vertices[bottom].y;

    pr_pixel* pixel;

    // Rasterize each scanline
    for (y = yStart; y <= yEnd; ++y)
    {
        len = rightSide[y].offset - leftSide[y].offset;
        if (len <= 0)
            continue;

        zStep = (rightSide[y].z - leftSide[y].z) / len;
        uStep = (rightSide[y].u - leftSide[y].u) / len;
        vStep = (rightSide[y].v - leftSide[y].v) / len;

        offset = leftSide[y].offset;
        zAct = leftSide[y].z;
        uAct = leftSide[y].u;
        vAct = leftSide[y].v;

        // Rasterize current scanline
        while (len-- > 0)
        {
            // Fetch pixel from framebuffer
            pixel = &(frameBuffer->pixels[offset]);

            // Make depth test
            PRdepthtype depth = _pr_pixel_write_depth(zAct);

            if (depth > pixel->depth)
            {
                pixel->depth = depth;

                #ifdef PR_PERSPECTIVE_CORRECTED
                // Compute perspective coorected coordinates
                z = 1.0f / zAct;
                u = uAct * z;
                v = vAct * z;
                #else
                z = zAct;
                u = uAct;
                v = vAct;
                #endif

                // Sample texture
                pixel->colorIndex = _pr_texture_sample_nearest(texels, mipWidth, mipHeight, u, v);
                //pixel->colorIndex = (PRubyte)(zAct * (PRfloat)UCHAR_MAX);
            }

            // Next pixel
            ++offset;
            zAct += zStep;
            uAct += uStep;
            vAct += vStep;
        }
    }
}

static void _pr_rasterize_triangle_textured(
    pr_framebuffer* frameBuffer, const pr_texture* texture, const pr_vertex* a, const pr_vertex* b, const pr_vertex* c)
{
    pr_raster_triangle triangle;

    // Make culling test
    if (_is_triangle_culled(a, b, c))
        return;

    // To raster a triangle we copy the vertex data into a 'raster_triangle' structure,
    // which is much smaller and compact to reduce memory overhead.
    // This is necessary because during triangle rasterization we will
    // access this memory a lot of times.
    _setup_triangle_vertex(&(triangle.a), a);
    _setup_triangle_vertex(&(triangle.b), b);
    _setup_triangle_vertex(&(triangle.c), c);

    #if 0//!!!

    // Select MIP level
    float tc0x = b->texCoord.x - a->texCoord.x;
    float tc0y = b->texCoord.y - a->texCoord.y;

    float tc1x = c->texCoord.x - a->texCoord.x;
    float tc1y = c->texCoord.y - a->texCoord.y;

    float c0x = b->ndc.x - a->ndc.x;
    float c0y = b->ndc.y - a->ndc.y;

    float c1x = c->ndc.x - a->ndc.x;
    float c1y = c->ndc.y - a->ndc.y;

    float len0 = sqrtf(c0x*c0x + c0y*c0y);
    float len1 = sqrtf(c1x*c1x + c1y*c1y);

    PRubyte mipLevel = _pr_texutre_compute_miplevel(
        texture,
        /*(b->texCoord.x - a->texCoord.x) * (b->ndc.x - a->ndc.x),
        (b->texCoord.y - a->texCoord.y) * (b->ndc.y - a->ndc.y),
        (c->texCoord.x - a->texCoord.x) * (c->ndc.x - a->ndc.x),
        (c->texCoord.y - a->texCoord.y) * (c->ndc.y - a->ndc.y)*/
        tc0x / len0,
        tc0y / len0,
        tc1x / len1,
        tc1y / len1
    );

    #else

    PRubyte mipLevel = 0;

    #endif

    // Rasterize polygon with 3 vertices
    _rasterize_polygon_textured(frameBuffer, texture, &(triangle.a), 3, mipLevel);
}

static void _render_indexed_triangles_textured(
    const pr_texture* texture, PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    // Iterate over the index buffer
    for (PRushort i = firstVertex, n = numVertices + firstVertex; i + 2 < n; i += 3)
    {
        // Fetch indices
        PRushort indexA = indexBuffer->indices[i];
        PRushort indexB = indexBuffer->indices[i + 1];
        PRushort indexC = indexBuffer->indices[i + 2];

        #ifdef PR_DEBUG
        if (indexA >= vertexBuffer->numVertices || indexB >= vertexBuffer->numVertices || indexC >= vertexBuffer->numVertices)
        {
            PR_SET_ERROR_FATAL("element in index buffer out of bounds");
            return;
        }
        #endif

        // Fetch vertices
        const pr_vertex* vertexA = (vertexBuffer->vertices + indexA);
        const pr_vertex* vertexB = (vertexBuffer->vertices + indexB);
        const pr_vertex* vertexC = (vertexBuffer->vertices + indexC);

        // Rasterize triangle
        _pr_rasterize_triangle_textured(PR_STATE_MACHINE.boundFrameBuffer, texture, vertexA, vertexB, vertexC);
    }
}

static void _render_indexed_triangles_colored(
    PRubyte colorIndex, PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

void _pr_render_triangles(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

void _pr_render_triangle_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

void _pr_render_triangle_fan(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer)
{
    _vertexbuffer_transform(numVertices, firstVertex, vertexBuffer);
    //...
}

void _pr_render_indexed_triangles(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    if (PR_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL || indexBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > indexBuffer->numIndices)
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    _vertexbuffer_transform_all(vertexBuffer);

    if (PR_STATE_MACHINE.boundTexture != NULL)
        _render_indexed_triangles_textured(PR_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer, indexBuffer);
    else
        _render_indexed_triangles_colored(PR_STATE_MACHINE.colorIndex, numVertices, firstVertex, vertexBuffer, indexBuffer);
}

void _pr_render_indexed_triangle_strip(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    _vertexbuffer_transform_all(vertexBuffer);
    //...
}

void _pr_render_indexed_triangle_fan(PRushort numVertices, PRushort firstVertex, pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    _vertexbuffer_transform_all(vertexBuffer);
    //...
}

