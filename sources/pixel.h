/*
 * pixel.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PIXEL_H__
#define __PR_PIXEL_H__


#include "types.h"


//! Frame buffer pixel structure.
typedef struct pr_pixel
{
    PRubyte colorIndex; //!< Colores are stored as 8-bit unsigned integer.
    PRushort depth;     //!< Depth values are stored as 16-bit unsigned integer.
}
pr_pixel;


/**
Writes the specified real depth value to a pixel depth.
\param[in] z Specifies the real z value. This must be in the range [0.0 .. 1.0].
*/
PRushort _pr_write_pixel_depth(PRfloat z);

//! Reads the specified pixel depth to a real depth value.
PRfloat _pr_read_pixel_depth(PRushort z);


#endif
