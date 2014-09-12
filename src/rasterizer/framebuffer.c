/*
 * framebuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "framebuffer.h"
#include "error.h"

#include <stdlib.h>


pr_framebuffer* _pr_framebuffer_create(PRuint width, PRuint height)
{
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    pr_framebuffer* framebuffer = (pr_framebuffer*)malloc(sizeof(pr_framebuffer));

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->pixels = calloc(width*height, sizeof(pr_pixel));

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
