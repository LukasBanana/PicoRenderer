/*
 * color.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_COLOR_H
#define PR_COLOR_H


#include "static_config.h"


#ifdef PR_BGR_COLOR_OUTPUT

#include "color_bgr.h"
typedef pr_color_bgr pr_color;

#else

#include "color_rgb.h"
typedef pr_color_rgb pr_color;

#endif

#ifdef PR_COLOR_BUFFER_24BIT

// excpetion in naming conventions for this project:
// with default static configuration, the color index is an 'unsigned char',
// but with 24-bit color buffer, this is from type 'pr_color' structure.
typedef pr_color PRcolorindex;

#else

typedef PRubyte PRcolorindex;

#endif


#endif
