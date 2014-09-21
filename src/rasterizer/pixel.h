/*
 * pixel.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PIXEL_H__
#define __PR_PIXEL_H__


#include "types.h"
#include "static_config.h"

#include <limits.h>


#ifdef PR_DEPTH_BUFFER_8BIT
#define PR_DEPTH_MAX UCHAR_MAX
typedef PRubyte PRdepthtype;
#else
#define PR_DEPTH_MAX USHRT_MAX
typedef PRushort PRdepthtype;
#endif

//! Frame buffer pixel structure.
typedef struct pr_pixel
{
    PRubyte     colorIndex; //!< Colors are stored as 8-bit unsigned integer.
    PRdepthtype depth;      //!< Depth values are stored as 16-bit unsigned integer.
}
pr_pixel;


/**
Writes the specified real depth value to a pixel depth.
\param[in] z Specifies the real z value. This must be in the range [0.0 .. 1.0].
*/
PR_INLINE PRdepthtype _pr_pixel_write_depth(PRinterp z)
{
    return (PRdepthtype)(z * (PRinterp)PR_DEPTH_MAX);
}

//! Reads the specified pixel depth to a real depth value.
PR_INLINE PRinterp _pr_pixel_read_depth(PRdepthtype z)
{
    return ((PRinterp)z) / ((PRdepthtype)PR_DEPTH_MAX);
}


#endif
