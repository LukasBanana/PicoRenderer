/*
 * framebuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_FRAMEBUFFER_H__
#define __PR_FRAMEBUFFER_H__


#include "pixel.h"


//! Framebuffer structure.
typedef struct pr_framebuffer
{
    PRuint      width;
    PRuint      height;
    pr_pixel*   pixels;
}
pr_framebuffer;


pr_framebuffer* _pr_gen_framebuffer(PRuint width, PRuint height);
void _pr_delete_framebuffer(pr_framebuffer* framebuffer);


#endif
