/*
 * viewport.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "viewport.h"
#include "framebuffer.h"
#include "error.h"


void _pr_viewport_init(pr_viewport* viewport)
{
    if (viewport != NULL)
    {
        viewport->x         = 0;
        viewport->y         = 0;
        viewport->width     = PR_DEFAULT_RESOLUTION_WIDTH;
        viewport->height    = PR_DEFAULT_RESOLUTION_HEIGHT;
        viewport->minDepth  = 0.0f;
        viewport->maxDepth  = 1.0f;
    }
    else
        _pr_set_error(PR_ERROR_NULL_POINTER);
}
