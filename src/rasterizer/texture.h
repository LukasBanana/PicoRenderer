/*
 * texture.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_TEXTURE_H__
#define __PR_TEXTURE_H__


#include "types.h"


#define PR_MIP_SIZE(size, mip) ((size) > (mip))

#define PR_IMAGE_FORMAT_UBYTE_RGB   1
#define PR_IMAGE_FORMAT_UBYTE_RGBA  2


//! Textures can have a maximum size of 256x256 texels.
//! Textures store all their mip maps in a single texel array for compact memory access.
typedef struct pr_texture
{
    PRubyte width;      //!< Width of the first MIP level.
    PRubyte height;     //!< Height of the first MIP level.
    PRubyte mips;       //!< Number of MIP levels.
    PRubyte* texels;    //!< Texel MIP chain.
}
pr_texture;


pr_texture* _pr_texture_create();
void _pr_texture_delete(pr_texture* texture);

PRboolean _pr_texture_image2d(
    pr_texture* texture,
    PRubyte width, PRubyte height,
    PRenum format, const PRubyte* data
);

PRboolean _pr_texture_subimage2d(
    pr_texture* texture,
    PRubyte mip, PRubyte x, PRubyte y,
    PRubyte width, PRubyte height,
    PRenum format, const PRubyte* data
);

//! Returns the number of MIP levels for the specified maximal texture dimension (width or height).
PRubyte _pr_texture_num_mips(PRubyte maxSize);


#endif
