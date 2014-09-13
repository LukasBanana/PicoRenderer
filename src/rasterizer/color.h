/*
 * color.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_COLOR_H__
#define __PR_COLOR_H__


#include "static_config.h"


#ifdef PR_BGR_COLOR_OUTPUT

#include "color_bgr.h"
typedef pr_color_bgr pr_color;

#else

#include "color_rgb.h"
typedef pr_color_rgb pr_color;

#endif


#endif
