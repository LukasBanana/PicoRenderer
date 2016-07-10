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
#include "raster_vertex.h"


//! Raster scanline side structure
typedef struct pr_scaline_side
{
    PRint       offset; //!< Pixel offset in framebuffer
    PRinterp    z;
    PRinterp    u;
    PRinterp    v;
}
pr_scaline_side;

//! Framebuffer structure
typedef struct pr_framebuffer
{
    PRuint              width;
    PRuint              height;
    #ifdef PR_MERGE_COLOR_AND_DEPTH_BUFFERS
    pr_pixel*           pixels;
    #else
    PRubyte*            colors;
    PRdepthtype         depths;
    #endif
    pr_scaline_side*    scanlinesStart; //!< Start offsets to scanlines
    pr_scaline_side*    scanlinesEnd;   //!< End offsets to scanlines
}
pr_framebuffer;


pr_framebuffer* _pr_framebuffer_create(PRuint width, PRuint height);
void _pr_framebuffer_delete(pr_framebuffer* frameBuffer);

void _pr_framebuffer_clear(pr_framebuffer* frameBuffer, PRfloat clearDepth, PRbitfield clearFlags);

//! Sets the start and end offsets of the specified scanlines.
void _pr_framebuffer_setup_scanlines(
    pr_framebuffer* frameBuffer, pr_scaline_side* sides, pr_raster_vertex start, pr_raster_vertex end
);

PR_INLINE void _pr_framebuffer_plot(pr_framebuffer* frameBuffer, PRuint x, PRuint y, PRcolorindex colorIndex)
{
    #ifdef PR_MERGE_COLOR_AND_DEPTH_BUFFERS
    frameBuffer->pixels[y * frameBuffer->width + x].colorIndex = colorIndex;
    #else
    frameBuffer->colors[y * frameBuffer->width + x] = colorIndex;
    #endif
}


#endif
