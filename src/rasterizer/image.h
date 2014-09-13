/*
 * image.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_IMAGE_H__
#define __PR_IMAGE_H__


#include "types.h"
#include "color.h"


//! Simple 2D image.
typedef struct pr_image
{
    PRint width;        //!< Image width.
    PRint height;       //!< Image height.
    PRint format;       //!< Color format (1, 2, 3 or 4).
    PRboolean defFree;  //!< True if colors will be deleted with default method.
    PRubyte* colors;    //!< Colors array.
}
pr_image;


//! Loads an RGB image from file.
pr_image* _pr_image_load_from_file(const char* filename);
/**
Creates a new image.
\param[in] width Specifies the image width. This must be greather than 0.
\param[in] height Specifies the image width. This must be greather than 0.
\param[in] format Specifies the image format. This must be 1, 2, 3 or 4.
*/
pr_image* _pr_image_create(PRint width, PRint height, PRint format);
//! Deletes the specifies image.
void _pr_image_delete(pr_image* image);

/**
Converts the specified 24-bit RGB colors 'src' into 8-bit color indices 'dst'.
\param[out] dst Pointer to the destination color indices.
This must already be allocated with the specified size: width*height*sizeof(PRubyte)!
\param[in] src Pointer to the source colors.
\param[in] width Specifies the image width.
\param[in] height Specifies the image height.
\param[in] format Specifies the source color format. Must be 1, 2, 3 or 4.
\param[in] dither Specifies whether the image is to be converted with or without dithering.
*/
void _pr_image_color_to_colorindex_r3g3b2(PRubyte* dstColors, const pr_image* srcImage, PRboolean dither);


#endif
