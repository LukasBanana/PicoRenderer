/*
 * framebuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "framebuffer.h"
#include "error.h"
#include "helper.h"
#include "state_machine.h"

#include <stdlib.h>
#include <string.h>


pr_framebuffer* _pr_framebuffer_create(PRuint width, PRuint height)
{
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create framebuffer
    pr_framebuffer* frameBuffer = PR_MALLOC(pr_framebuffer);

    frameBuffer->width = width;
    frameBuffer->height = height;
    frameBuffer->pixels = PR_CALLOC(pr_pixel, width*height);
    frameBuffer->scanlinesStart = PR_CALLOC(pr_scaline_side, height);
    frameBuffer->scanlinesEnd = PR_CALLOC(pr_scaline_side, height);

    // Initialize framebuffer
    memset(frameBuffer->pixels, 0, width*height*sizeof(pr_pixel));

    _pr_ref_add(frameBuffer);

    return frameBuffer;
}

void _pr_framebuffer_delete(pr_framebuffer* frameBuffer)
{
    if (frameBuffer != NULL)
    {
        _pr_ref_release(frameBuffer);

        PR_FREE(frameBuffer->pixels);
        PR_FREE(frameBuffer->scanlinesStart);
        PR_FREE(frameBuffer->scanlinesEnd);
        PR_FREE(frameBuffer);
    }
}

void _pr_framebuffer_clear(pr_framebuffer* frameBuffer, PRubyte clearColor, PRfloat depth)
{
    if (frameBuffer != NULL && frameBuffer->pixels != NULL)
    {
        // Convert depth (32-bit) into pixel depth (16-bit or 8-bit)
        PRdepthtype clearDepth = _pr_pixel_write_depth(depth);

        // Iterate over the entire framebuffer
        pr_pixel* dst = frameBuffer->pixels;
        pr_pixel* dstEnd = dst + (frameBuffer->width * frameBuffer->height);

        while (dst != dstEnd)
        {
            dst->colorIndex = clearColor;
            dst->depth      = clearDepth;
            ++dst;
        }
    }
    else
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
}

void _pr_framebuffer_setup_scanlines(
    pr_framebuffer* frameBuffer, pr_scaline_side* sides, pr_raster_vertex start, pr_raster_vertex end)
{
    PRint pitch = (PRint)frameBuffer->width;
    PRint len = end.y - start.y;

    if (len <= 0)
    {
        sides[start.y].offset = start.y * pitch + start.x;
        return;
    }

    // Compute offsets (need doubles for offset for better precision, because the range is larger)
    PRdouble offsetStart = (PRdouble)(start.y * pitch + start.x);
    PRdouble offsetEnd   = (PRdouble)(end.y * pitch + end.x);
    PRdouble offsetStep  = (offsetEnd - offsetStart) / len;

    PRfloat zStep       = (end.z - start.z) / len;
    PRfloat uStep       = (end.u - start.u) / len;
    PRfloat vStep       = (end.v - start.v) / len;

    // Fill scanline sides
    pr_scaline_side* sidesEnd = &(sides[end.y]);

    for (sides += start.y; sides <= sidesEnd; ++sides)
    {
        // Setup scanline side
        sides->offset = (PRint)(offsetStart + 0.5);
        sides->z = start.z;
        sides->u = start.u;
        sides->v = start.v;

        // Next step
        offsetStart += offsetStep;
        start.z += zStep;
        start.u += uStep;
        start.v += vStep;
    }
}

