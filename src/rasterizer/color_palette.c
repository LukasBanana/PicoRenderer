/*
 * color_palette.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "color_palette.h"
#include "error.h"


/*
8-bit color encoding:
Bit     7 6 5 4 3 2 1 0
Color   R R R G G G B B
*/
void _pr_color_palette_fill_r3g3b2(pr_color_palette* colorPalette)
{
    if (colorPalette == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER);
        return;
    }

    pr_color_bgr* clr = colorPalette->colors;

    // Color palettes for 3- and 2 bit color components
    const PRubyte palette3Bit[8] = { 0, 36, 73, 109, 146, 182, 219, 255 };
    const PRubyte palette2Bit[4] = { 0, 85, 170, 255 };

    for (PRubyte r = 0; r < 8; ++r)
    {
        for (PRubyte g = 0; g < 8; ++g)
        {
            for (PRubyte b = 0; b < 4; ++b)
            {
                clr->r = palette3Bit[r];
                clr->g = palette3Bit[g];
                clr->b = palette2Bit[b];
                ++clr;
            }
        }
    }
}

PRubyte _pr_color_to_colorindex_r3g3b2(PRubyte r, PRubyte g, PRubyte b)
{
    return
        (((r / 36) & 0x07) << 5) |
        (((g / 36) & 0x07) << 2) |
        ( (b / 85) & 0x03      );
}

