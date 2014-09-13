/*
 * framebuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "framebuffer.h"
#include "error.h"
#include "helper.h"

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
    pr_framebuffer* framebuffer = PR_MALLOC(pr_framebuffer);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->pixels = PR_CALLOC(pr_pixel, width*height);

    // Initialize framebuffer
    memset(framebuffer->pixels, 0, width*height*sizeof(pr_pixel));

    return framebuffer;
}

void _pr_framebuffer_delete(pr_framebuffer* framebuffer)
{
    if (framebuffer != NULL && framebuffer->pixels != NULL)
    {
        free(framebuffer->pixels);
        free(framebuffer);
    }
}

void _pr_framebuffer_clear(pr_framebuffer* framebuffer, PRubyte clearColor, float depth)
{
    if (framebuffer != NULL && framebuffer->pixels != NULL)
    {
        // Convert depth (32-bit) into pixel depth (16-bit)
        PRushort clearDepth = _pr_pixel_write_depth(depth);

        // Iterate over the entire framebuffer
        pr_pixel* dst = framebuffer->pixels;
        pr_pixel* dstEnd = dst + (framebuffer->width * framebuffer->height);

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
