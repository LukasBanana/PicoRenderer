/*
 * render.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "render.h"
#include "state_machine.h"
#include "global_state.h"
#include "raster_triangle.h"
#include "ext_math.h"
#include "matrix4.h"
#include "error.h"
#include "static_config.h"

#include <stdio.h>

#include <math.h>//!!!


// --- internals ---

#define MAX_NUM_POLYGON_VERTS 32

static pr_clip_vertex _clipVertices[MAX_NUM_POLYGON_VERTS], _clipVerticesTmp[MAX_NUM_POLYGON_VERTS];
static pr_raster_vertex _rasterVertices[MAX_NUM_POLYGON_VERTS], _rasterVerticesTmp[MAX_NUM_POLYGON_VERTS];
static PRint _numPolyVerts = 0;

#define _CVERT_VEC2(v) (*(pr_vector2*)(&((_clipVertices[v]).x)))

static void _vertexbuffer_transform(PRsizei numVertices, PRsizei firstVertex, pr_vertexbuffer* vertexBuffer)
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

static void _setup_clip_vertex(pr_clip_vertex* clipVert, const pr_vertex* vert, const pr_matrix4* worldViewMatrix)
{
    _pr_matrix_mul_float3(&(clipVert->x), worldViewMatrix, &(vert->coord.x));
    clipVert->w = 1.0f;
    clipVert->u = vert->texCoord.x;
    clipVert->v = vert->texCoord.y;
}

static void _project_vertex(pr_clip_vertex* vertex, const pr_matrix4* projectionMatrix, const pr_viewport* viewport)
{
    // Transform view-space coordinate into projection space
    pr_vector4 ndc;
    _pr_matrix_mul_float4(&(ndc.x), projectionMatrix, &(vertex->x));

    // Transform coordinate into normalized device coordinates
    //vertex->z = 1.0f / ndc.w;
    PRfloat rhw = 1.0f / ndc.w;

    vertex->x = ndc.x * rhw;
    vertex->y = ndc.y * rhw;
    vertex->z = /*ndc.z * */rhw;

    // Transform vertex to screen coordiante (+0.5 is for rounding adjustment)
    vertex->x = viewport->x + (vertex->x + 1.0f) * viewport->halfWidth + 0.5f;
    vertex->y = viewport->y + (vertex->y + 1.0f) * viewport->halfHeight + 0.5f;
    //vertex->z = viewport->minDepth + vertex->z * viewport->depthSize;

    #ifdef PR_PERSPECTIVE_CORRECTED
    // Setup inverse texture coordinates
    vertex->u *= rhw;
    vertex->v *= rhw;
    #endif
}

static void _setup_raster_vertex(pr_raster_vertex* rasterVert, const pr_clip_vertex* clipVert)
{
    rasterVert->x = (PRint)(clipVert->x);
    rasterVert->y = (PRint)(clipVert->y);
    rasterVert->z = clipVert->z;
    rasterVert->u = clipVert->u;
    rasterVert->v = clipVert->v;
}

// Returns PR_TRUE if the specified triangle vertices are culled.
static PRboolean _is_triangle_culled(const pr_vector2 a, const pr_vector2 b, const pr_vector2 c)
{
    if (PR_STATE_MACHINE.cullMode != PR_CULL_NONE)
    {
        const PRfloat vis = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);

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

void _pr_render_points(PRsizei numVertices, PRsizei firstVertex, /*const */pr_vertexbuffer* vertexBuffer)
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

void _pr_render_indexed_points(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
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

// Rasterizes a textured line using the "Bresenham" algorithm
static void _rasterize_line(pr_framebuffer* frameBuffer, const pr_texture* texture, PRubyte mipLevel, PRuint indexA, PRuint indexB)
{
    const pr_raster_vertex* vertexA = &(_rasterVertices[indexA]);
    const pr_raster_vertex* vertexB = &(_rasterVertices[indexB]);

    // Select MIP level
    PRtexsize mipWidth, mipHeight;
    const PRubyte* texels = _pr_texture_select_miplevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Pre-compuations
    int dx = vertexB->x - vertexA->x;
    int dy = vertexB->y - vertexA->y;
    
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
    
    int x = vertexA->x;
    int y = vertexA->y;
    float u = vertexA->u;
    float v = vertexA->v;

    float uStep = 0.0f, vStep = 0.0f;

    if (el > 1)
    {
        uStep = (vertexB->u - vertexA->u) / (el - 1);
        vStep = (vertexB->v - vertexA->v) / (el - 1);
    }

    int err = el/2;

    PRubyte colorIndex;
    
    // Render each pixel of the line
    for (PRint t = 0; t < el; ++t)
    {
        // Render pixel
        colorIndex = _pr_texture_sample_nearest(texels, mipWidth, mipHeight, u, v);

        _pr_framebuffer_plot(frameBuffer, (PRuint)x, (PRuint)y, colorIndex);
        
        // Increase tex-coords
        u += uStep;
        v += vStep;

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
    const pr_texture* texture, PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

static void _render_indexed_lines_colored(
    PRubyte colorIndex, PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (PRsizei i = firstVertex, n = numVertices + firstVertex; i + 1 < n; i += 2)
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

void _pr_render_lines(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    //...
}

void _pr_render_line_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    //...
}

void _pr_render_line_loop(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    //...
}

void _pr_render_indexed_lines(PRsizei numVertices, PRsizei firstVertex, /*const */pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
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

void _pr_render_indexed_line_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

void _pr_render_indexed_line_loop(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
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
    pr_pixel* scanline;

    for (PRint y = top; y <= bottom; ++y)
    {
        scanline = pixels + (y * pitch + left);
        for (PRint x = left; x <= right; ++x)
        {
            scanline->colorIndex = colorIndex;
            ++scanline;
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

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'z'
static pr_clip_vertex _get_zplane_vertex(pr_clip_vertex a, pr_clip_vertex b, PRfloat z)
{
    PRfloat m = (z - b.z) / (a.z - b.z);
    pr_clip_vertex c;

    c.x = m * (a.x - b.x) + b.x;
    c.y = m * (a.y - b.y) + b.y;
    c.z = z;

    c.u = m * (a.u - b.u) + b.u;
    c.v = m * (a.v - b.v) + b.v;

    return c;
}

// Clips the polygon at the z planes
static void _polygon_z_clipping(PRfloat zMin, PRfloat zMax)
{
    PRint x, y;

    // Clip at near clipping plane (zMin)
    PRint localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_clipVertices[x].z >= zMin && _clipVertices[y].z >= zMin)
            _clipVerticesTmp[localNumVerts++] = _clipVertices[y];

        // Leaving
        if (_clipVertices[x].z >= zMin && _clipVertices[y].z < zMin)
            _clipVerticesTmp[localNumVerts++] = _get_zplane_vertex(_clipVertices[x], _clipVertices[y], zMin);

        // Entering
        if (_clipVertices[x].z < zMin && _clipVertices[y].z >= zMin)
        {
            _clipVerticesTmp[localNumVerts++] = _get_zplane_vertex(_clipVertices[x], _clipVertices[y], zMin);
            _clipVerticesTmp[localNumVerts++] = _clipVertices[y];
        }
    }

    // Clip at far clipping plane (zMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_clipVerticesTmp[x].z <= zMax && _clipVerticesTmp[y].z <= zMax)
            _clipVertices[_numPolyVerts++] = _clipVerticesTmp[y];

        // Leaving
        if (_clipVerticesTmp[x].z <= zMax && _clipVerticesTmp[y].z > zMax)
            _clipVertices[_numPolyVerts++] = _get_zplane_vertex(_clipVerticesTmp[x], _clipVerticesTmp[y], zMax);

        // Entering
        if (_clipVerticesTmp[x].z > zMax && _clipVerticesTmp[y].z <= zMax)
        {
            _clipVertices[_numPolyVerts++] = _get_zplane_vertex(_clipVerticesTmp[x], _clipVerticesTmp[y], zMax);
            _clipVertices[_numPolyVerts++] = _clipVerticesTmp[y];
        }
    }
}

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'x'
static pr_raster_vertex _get_xplane_vertex(pr_raster_vertex a, pr_raster_vertex b, PRint x)
{
    PRfloat m = ((PRfloat)(x - b.x)) / (a.x - b.x);
    pr_raster_vertex c;

    c.x = x;
    c.y = (PRint)(m * (a.y - b.y) + b.y);
    c.z = m * (a.z - b.z) + b.z;

    c.u = m * (a.u - b.u) + b.u;
    c.v = m * (a.v - b.v) + b.v;

    return c;
}

// Computes the vertex 'c' which is cliped between the vertices 'a' and 'b' and the plane 'y'
static pr_raster_vertex _get_yplane_vertex(pr_raster_vertex a, pr_raster_vertex b, PRint y)
{
    PRfloat m = ((PRfloat)(y - b.y)) / (a.y - b.y);
    pr_raster_vertex c;

    c.x = (PRint)(m * (a.x - b.x) + b.x);
    c.y = y;
    c.z = m * (a.z - b.z) + b.z;

    c.u = m * (a.u - b.u) + b.u;
    c.v = m * (a.v - b.v) + b.v;

    return c;
}

// Clips the polygon at the x and y planes
static void _polygon_xy_clipping(PRint xMin, PRint xMax, PRint yMin, PRint yMax)
{
    PRint x, y;

    // Clip at left clipping plane (xMin)
    PRint localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVertices[x].x >= xMin && _rasterVertices[y].x >= xMin)
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];

        // Leaving
        if (_rasterVertices[x].x >= xMin && _rasterVertices[y].x < xMin)
            _rasterVerticesTmp[localNumVerts++] = _get_xplane_vertex(_rasterVertices[x], _rasterVertices[y], xMin);

        // Entering
        if (_rasterVertices[x].x < xMin && _rasterVertices[y].x >= xMin)
        {
            _rasterVerticesTmp[localNumVerts++] = _get_xplane_vertex(_rasterVertices[x], _rasterVertices[y], xMin);
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];
        }
    }

    // Clip at right clipping plane (xMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVerticesTmp[x].x <= xMax && _rasterVerticesTmp[y].x <= xMax)
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];

        // Leaving
        if (_rasterVerticesTmp[x].x <= xMax && _rasterVerticesTmp[y].x > xMax)
            _rasterVertices[_numPolyVerts++] = _get_xplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], xMax);

        // Entering
        if (_rasterVerticesTmp[x].x > xMax && _rasterVerticesTmp[y].x <= xMax)
        {
            _rasterVertices[_numPolyVerts++] = _get_xplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], xMax);
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];
        }
    }

    // Clip at top clipping plane (yMin)
    localNumVerts = 0;

    for (x = _numPolyVerts - 1, y = 0; y < _numPolyVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVertices[x].y >= yMin && _rasterVertices[y].y >= yMin)
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];

        // Leaving
        if (_rasterVertices[x].y >= yMin && _rasterVertices[y].y < yMin)
            _rasterVerticesTmp[localNumVerts++] = _get_yplane_vertex(_rasterVertices[x], _rasterVertices[y], yMin);

        // Entering
        if (_rasterVertices[x].y < yMin && _rasterVertices[y].y >= yMin)
        {
            _rasterVerticesTmp[localNumVerts++] = _get_yplane_vertex(_rasterVertices[x], _rasterVertices[y], yMin);
            _rasterVerticesTmp[localNumVerts++] = _rasterVertices[y];
        }
    }

    // Clip at bottom clipping plane (yMax)
    _numPolyVerts = 0;

    for (x = localNumVerts - 1, y = 0; y < localNumVerts; x = y, ++y)
    {
        // Inside
        if (_rasterVerticesTmp[x].y <= yMax && _rasterVerticesTmp[y].y <= yMax)
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];

        // Leaving
        if (_rasterVerticesTmp[x].y <= yMax && _rasterVerticesTmp[y].y > yMax)
            _rasterVertices[_numPolyVerts++] = _get_yplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], yMax);

        // Entering
        if (_rasterVerticesTmp[x].y > yMax && _rasterVerticesTmp[y].y <= yMax)
        {
            _rasterVertices[_numPolyVerts++] = _get_yplane_vertex(_rasterVerticesTmp[x], _rasterVerticesTmp[y], yMax);
            _rasterVertices[_numPolyVerts++] = _rasterVerticesTmp[y];
        }
    }
}

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

// Rasterizes convex polygon filled
static void _rasterize_polygon_fill(pr_framebuffer* frameBuffer, const pr_texture* texture, PRubyte mipLevel)
{
    // Select MIP level
    PRtexsize mipWidth, mipHeight;
    const PRubyte* texels = _pr_texture_select_miplevel(texture, mipLevel, &mipWidth, &mipHeight);

    // Find left- and right sided polygon edges
    PRint x, y, top = 0, bottom = 0;

    for (x = 1; x < _numPolyVerts; ++x)
    {
        if (_rasterVertices[top].y > _rasterVertices[x].y)
            top = x;
        if (_rasterVertices[bottom].y < _rasterVertices[x].y)
            bottom = x;
    }

    // Setup raster scanline sides
    pr_scaline_side* leftSide = frameBuffer->scanlinesStart;
    pr_scaline_side* rightSide = frameBuffer->scanlinesEnd;

    x = y = top;
    for (_index_dec(&y, _numPolyVerts); x != bottom; x = y, _index_dec(&y, _numPolyVerts))
        _pr_framebuffer_setup_scanlines(frameBuffer, leftSide, _rasterVertices[x], _rasterVertices[y]);

    x = y = top;
    for (_index_inc(&y, _numPolyVerts); x != bottom; x = y, _index_inc(&y, _numPolyVerts))
        _pr_framebuffer_setup_scanlines(frameBuffer, rightSide, _rasterVertices[x], _rasterVertices[y]);

    // Check if sides must be swaped
    long midIndex = (_rasterVertices[bottom].y + _rasterVertices[top].y) / 2;
    if (frameBuffer->scanlinesStart[midIndex].offset > frameBuffer->scanlinesEnd[midIndex].offset)
        PR_SWAP(pr_scaline_side*, leftSide, rightSide);

    // Start rasterizing the polygon
    PRint len, offset;
    PRfloat z, zAct, zStep;
    PRfloat u, uAct, uStep;
    PRfloat v, vAct, vStep;

    PRint yStart = _rasterVertices[top].y;
    PRint yEnd = _rasterVertices[bottom].y;

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
                // Compute perspective corrected texture coordinates
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

// Rasterizes convex polygon outlines
static void _rasterize_polygon_line(pr_framebuffer* frameBuffer, const pr_texture* texture, PRubyte mipLevel)
{
    for (PRint i = 0; i + 1 < _numPolyVerts; ++i)
        _rasterize_line(frameBuffer, texture, mipLevel, i, i + 1);
    _rasterize_line(frameBuffer, texture, mipLevel, _numPolyVerts - 1, 0);
}

// Rasterizes convex polygon points
static void _rasterize_polygon_point(
    pr_framebuffer* frameBuffer, const pr_texture* texture, PRubyte mipLevel)
{
    for (PRint i = 0; i < _numPolyVerts; ++i)
    {
        _pr_framebuffer_plot(
            frameBuffer,
            _rasterVertices[i].x,
            _rasterVertices[i].y,
            PR_STATE_MACHINE.colorIndex
        );
    }
}

static void _rasterize_polygon(pr_framebuffer* frameBuffer, const pr_texture* texture)
{
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

    // Rasterize polygon with selected MIP level
    switch (PR_STATE_MACHINE.polygonMode)
    {
        case PR_POLYGON_FILL:
            _rasterize_polygon_fill(frameBuffer, texture, mipLevel);
            break;
        case PR_POLYGON_LINE:
            _rasterize_polygon_line(frameBuffer, texture, mipLevel);
            break;
        case PR_POLYGON_POINT:
            _rasterize_polygon_point(frameBuffer, texture, mipLevel);
            break;
    }
}

static PRboolean _clip_and_transform_polygon()
{
    // Get clipping rectangle
    const PRint xMin = PR_STATE_MACHINE.clipRect.left;
    const PRint xMax = PR_STATE_MACHINE.clipRect.right;
    const PRint yMin = PR_STATE_MACHINE.clipRect.top;
    const PRint yMax = PR_STATE_MACHINE.clipRect.bottom;

    // Z clipping
    _numPolyVerts = 3;
    _polygon_z_clipping(1.0f, 100.0f);//!!!

    // Projection
    for (PRint j = 0; j < _numPolyVerts; ++j)
        _project_vertex(&(_clipVertices[j]), &(PR_STATE_MACHINE.projectionMatrix), &(PR_STATE_MACHINE.viewport));

    // Make culling test
    if (_numPolyVerts < 3 || _is_triangle_culled(_CVERT_VEC2(0), _CVERT_VEC2(1), _CVERT_VEC2(2)))
        return PR_FALSE;

    // Setup raster vertices
    for (PRint j = 0; j < _numPolyVerts; ++j)
        _setup_raster_vertex(&(_rasterVertices[j]), &(_clipVertices[j]));

    // Edge clipping
    _polygon_xy_clipping(xMin, xMax, yMin, yMax);

    if (_numPolyVerts < 3)
        return PR_FALSE;

    return PR_TRUE;
}

static void _render_triangles(
    const pr_texture* texture, PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    // Get clipping dimensions
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (PRsizei i = firstVertex, n = numVertices + firstVertex; i + 2 < n; i += 3)
    {
        // Fetch vertices
        const pr_vertex* vertexA = (vertexBuffer->vertices + i);
        const pr_vertex* vertexB = (vertexBuffer->vertices + (i + 1));
        const pr_vertex* vertexC = (vertexBuffer->vertices + (i + 2));

        // Setup polygon
        _setup_clip_vertex(&(_clipVertices[0]), vertexA, &(PR_STATE_MACHINE.worldViewMatrix));
        _setup_clip_vertex(&(_clipVertices[1]), vertexB, &(PR_STATE_MACHINE.worldViewMatrix));
        _setup_clip_vertex(&(_clipVertices[2]), vertexC, &(PR_STATE_MACHINE.worldViewMatrix));

        if (_clip_and_transform_polygon() != PR_FALSE)
        {
            // Rasterize active polygon
            _rasterize_polygon(frameBuffer, texture);
        }
    }
}

void _pr_render_triangles(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    if (PR_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    if (vertexBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }
    if (firstVertex + numVertices > vertexBuffer->numVertices)
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    if (PR_STATE_MACHINE.boundTexture == NULL)
    {
        _pr_texture_singular_color(&PR_SINGULAR_TEXTURE, PR_STATE_MACHINE.colorIndex);
        _render_triangles(&PR_SINGULAR_TEXTURE, numVertices, firstVertex, vertexBuffer);
    }
    else
        _render_triangles(PR_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer);
}

void _pr_render_triangle_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    //...
}

void _pr_render_triangle_fan(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer)
{
    //...
}

static void _render_indexed_triangles(
    const pr_texture* texture, PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    // Get clipping dimensions
    pr_framebuffer* frameBuffer = PR_STATE_MACHINE.boundFrameBuffer;

    // Iterate over the index buffer
    for (PRsizei i = firstVertex, n = numVertices + firstVertex; i + 2 < n; i += 3)
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

        // Setup polygon
        _setup_clip_vertex(&(_clipVertices[0]), vertexA, &(PR_STATE_MACHINE.worldViewMatrix));
        _setup_clip_vertex(&(_clipVertices[1]), vertexB, &(PR_STATE_MACHINE.worldViewMatrix));
        _setup_clip_vertex(&(_clipVertices[2]), vertexC, &(PR_STATE_MACHINE.worldViewMatrix));

        if (_clip_and_transform_polygon() != PR_FALSE)
        {
            // Rasterize active polygon
            _rasterize_polygon(frameBuffer, texture);
        }
    }
}

void _pr_render_indexed_triangles(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
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

    if (PR_STATE_MACHINE.boundTexture == NULL)
    {
        _pr_texture_singular_color(&PR_SINGULAR_TEXTURE, PR_STATE_MACHINE.colorIndex);
        _render_indexed_triangles(&PR_SINGULAR_TEXTURE, numVertices, firstVertex, vertexBuffer, indexBuffer);
    }
    else
        _render_indexed_triangles(PR_STATE_MACHINE.boundTexture, numVertices, firstVertex, vertexBuffer, indexBuffer);
}

void _pr_render_indexed_triangle_strip(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

void _pr_render_indexed_triangle_fan(PRsizei numVertices, PRsizei firstVertex, const pr_vertexbuffer* vertexBuffer, const pr_indexbuffer* indexBuffer)
{
    //...
}

