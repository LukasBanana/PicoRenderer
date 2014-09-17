/*
 * texture.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_TEXTURE_H__
#define __PR_TEXTURE_H__


#include "types.h"
#include "enums.h"


#define PR_MIP_SIZE(size, mip)      ((size) >> (mip))
#define PR_TEXTURE_HAS_MIPS(tex)    ((tex)->mips > 1)


//! Textures can have a maximum size of 256x256 texels.
//! Textures store all their mip maps in a single texel array for compact memory access.
typedef struct pr_texture
{
    PRtexsize width;    //!< Width of the first MIP level.
    PRtexsize height;   //!< Height of the first MIP level.
    PRubyte mips;       //!< Number of MIP levels.
    PRubyte* texels;    //!< Texel MIP chain.
}
pr_texture;


pr_texture* _pr_texture_create();
void _pr_texture_delete(pr_texture* texture);

void _pr_texture_singular_init(pr_texture* texture);
void _pr_texture_singular_clear(pr_texture* texture);

//! Sets the single color to the specified texture. No null pointer assertion!
PR_INLINE void _pr_texture_singular_color(pr_texture* texture, PRubyte colorIndex)
{
    texture->texels[0] = colorIndex;
}

//! Sets the 2D image data to the specified texture.
PRboolean _pr_texture_image2d(
    pr_texture* texture,
    PRtexsize width, PRtexsize height,
    PRenum format, const PRvoid* data, PRboolean dither, PRboolean generateMips
);

PRboolean _pr_texture_subimage2d(
    pr_texture* texture,
    PRubyte mip, PRtexsize x, PRtexsize y,
    PRtexsize width, PRtexsize height,
    PRenum format, const PRvoid* data, PRboolean dither
);

//! Returns the number of MIP levels for the specified maximal texture dimension (width or height).
PRubyte _pr_texture_num_mips(PRubyte maxSize);

//! Returns a pointer to the specified texture MIP level.
const PRubyte* _pr_texture_select_miplevel(const pr_texture* texture, PRubyte mip, PRtexsize* width, PRtexsize* height);

//! Returns the MIP level index for the specified texture.
PRubyte _pr_texutre_compute_miplevel(const pr_texture* texture, PRfloat dux, PRfloat duy, PRfloat dvx, PRfloat dvy);

//! Samples the nearest texels.
PRubyte _pr_texture_sample_nearest(const PRubyte* mipTexels, PRtexsize width, PRtexsize height, PRfloat u, PRfloat v);


#endif
