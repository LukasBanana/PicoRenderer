/*
 * color_palette.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_COLOR_PALETTE_H__
#define __PR_COLOR_PALETTE_H__


#include "color.h"


//! Color palette for 8-bit color indices.
typedef struct pr_color_palette
{
    pr_color colors[256];
}
pr_color_palette;


//! Fills the specified color palette with the encoding R3G3B2.
void _pr_color_palette_fill_r3g3b2(pr_color_palette* colorPalette);

//! Converts the specified RGB color into a color index with encoding R3G3B2.
PRubyte _pr_color_to_colorindex_r3g3b2(PRubyte r, PRubyte g, PRubyte b);


#endif
