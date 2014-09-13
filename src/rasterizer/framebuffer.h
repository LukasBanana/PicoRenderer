/*
 * framebuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_FRAMEBUFFER_H__
#define __PR_FRAMEBUFFER_H__


#include "pixel.h"
#include "enums.h"


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

void _pr_framebuffer_clear(pr_framebuffer* framebuffer, PRubyte clearColor, float depth);

// Inline pixel plotter function.
PR_INLINE void _pr_framebuffer_plot(pr_framebuffer* framebuffer, PRuint x, PRuint y, PRubyte colorIndex)
{
    framebuffer->pixels[y * framebuffer->width + x].colorIndex = colorIndex;
}


#endif
