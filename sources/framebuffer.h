/*
 * framebuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_FRAMEBUFFER_H__
#define __PR_FRAMEBUFFER_H__


#include "pixel.h"


#define PR_DEFAULT_RESOLUTION_WIDTH     640
#define PR_DEFAULT_RESOLUTION_HEIGHT    480


//! Framebuffer structure.
typedef struct pr_framebuffer
{
    PRuint      width;
    PRuint      height;
    pr_pixel*   pixels;
}
pr_framebuffer;


pr_framebuffer* _pr_framebuffer_create(PRuint width, PRuint height);
void _pr_framebuffer_delete(pr_framebuffer* framebuffer);


#endif
