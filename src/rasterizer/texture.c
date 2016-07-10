/*
 * texture.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "texture.h"
#include "ext_math.h"
#include "error.h"
#include "helper.h"
#include "image.h"
#include "state_machine.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>


// --- internals --- //

static void _texture_subimage2d(
    PRcolorindex* texels, PRubyte mip, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data, PRboolean dither)
{
    if (format != PR_UBYTE_RGB)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    // Setup structure for sub-image
    pr_image subimage;
    subimage.width      = width;
    subimage.height     = height;
    subimage.format     = 3;
    subimage.defFree    = PR_TRUE;
    subimage.colors     = (PRubyte*)data;

    _pr_image_color_to_colorindex(texels, &subimage, dither);
}

static void _texture_subimage2d_rect(
    pr_texture* texture, PRubyte mip, PRtexsize x, PRtexsize y, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data)
{
    //...
}

static PRubyte _color_box4_blur(PRubyte a, PRubyte b, PRubyte c, PRubyte d)
{
    PRint x;
    x  = a;
    x += b;
    x += c;
    x += d;
    return (PRubyte)(x / 4);
}

static PRubyte _color_box2_blur(PRubyte a, PRubyte b)
{
    PRint x;
    x  = a;
    x += b;
    return (PRubyte)(x / 2);
}

static PRubyte* _image_scale_down_ubyte_rgb(PRtexsize width, PRtexsize height, const PRubyte* data)
{
    #define COLOR(x, y, i) data[((y)*width + (x))*3 + (i)]

    const PRtexsize scaledWidth = (width > 1 ? width/2 : 1);
    const PRtexsize scaledHeight = (height > 1 ? height/2 : 1);

    PRubyte* scaled = PR_CALLOC(PRubyte, scaledWidth*scaledHeight*3);

    if (width > 1 && height > 1)
    {
        for (PRtexsize y = 0; y < scaledHeight; ++y)
        {
            for (PRtexsize x = 0; x < scaledWidth; ++x)
            {
                for (PRint i = 0; i < 3; ++i)
                {
                    scaled[(y*scaledWidth + x)*3 + i] = _color_box4_blur(
                        COLOR(x*2    , y*2    , i),
                        COLOR(x*2 + 1, y*2    , i),
                        COLOR(x*2 + 1, y*2 + 1, i),
                        COLOR(x*2    , y*2 + 1, i)
                    );
                }
            }
        }
    }
    else if (width > 1)
    {
        for (PRtexsize x = 0; x < scaledWidth; ++x)
        {
            for (PRint i = 0; i < 3; ++i)
            {
                scaled[x*3 + i] = _color_box2_blur(
                    COLOR(x*2    , 0, i),
                    COLOR(x*2 + 1, 0, i)
                );
            }
        }
    }
    else if (height > 1)
    {
        for (PRtexsize y = 0; y < scaledHeight; ++y)
        {
            for (PRint i = 0; i < 3; ++i)
            {
                scaled[y*scaledWidth*3 + i] = _color_box2_blur(
                    COLOR(0, y*2    , i),
                    COLOR(0, y*2 + 1, i)
                );
            }
        }
    }

    return scaled;

    #undef COLOR
}

static PRubyte* _image_scale_down(PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data)
{
    switch (format)
    {
        case PR_UBYTE_RGB:
            return _image_scale_down_ubyte_rgb(width, height, (const PRubyte*)data);
        default:
            break;
    }
    return NULL;
}

// --- interface --- //

pr_texture* _pr_texture_create()
{
    // Create texture
    pr_texture* texture = PR_MALLOC(pr_texture);

    texture->width  = 0;
    texture->height = 0;
    texture->mips   = 0;
    texture->texels = NULL;

    for (size_t i = 0; i < PR_MAX_NUM_MIPS; ++i)
        texture->mipTexels[i] = NULL;

    _pr_ref_add(texture);

    return texture;
}

void _pr_texture_delete(pr_texture* texture)
{
    if (texture != NULL)
    {
        _pr_ref_release(texture);

        PR_FREE(texture->texels);
        PR_FREE(texture);
    }
}

void _pr_texture_singular_init(pr_texture* texture)
{
    if (texture != NULL)
    {
        texture->width  = 1;
        texture->height = 1;
        texture->mips   = 1;
        texture->texels = PR_CALLOC(PRcolorindex, 1);
    }
}

void _pr_texture_singular_clear(pr_texture* texture)
{
    if (texture != NULL)
        PR_FREE(texture->texels);
}

PRboolean _pr_texture_image2d(
    pr_texture* texture, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data, PRboolean dither, PRboolean generateMips)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return PR_FALSE;
    }
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, "textures must not have a size equal to zero");
        return PR_FALSE;
    }
    if (width > PR_MAX_TEX_SIZE || height > PR_MAX_TEX_SIZE)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, "maximum texture size exceeded");
        return PR_FALSE;
    }

    // Determine number of texels
    PRubyte mips = 0;
    size_t numTexels = 0;

    if (generateMips != PR_FALSE)
    {
        PRtexsize w = width;
        PRtexsize h = height;

        while (1)
        {
            // Count number of texels
            numTexels += w*h;
            ++mips;

            if (w == 1 && h == 1)
                break;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }
    else
    {
        mips = 1;
        numTexels = width*height;
    }

    // Check if texels must be reallocated
    if (texture->width != width || texture->height != height || texture->mips != mips)
    {
        // Setup new texture dimension
        texture->width  = width;
        texture->height = height;
        texture->mips   = mips;

        // Free previous texels
        PR_FREE(texture->texels);

        // Create texels
        texture->texels = PR_CALLOC(PRcolorindex, numTexels);

        // Setup MIP texel offsets
        const PRcolorindex* texels = texture->texels;
        PRtexsize w = width, h = height;

        for (PRubyte mip = 0; mip < texture->mips; ++mip)
        {
            // Store current texel offset
            texture->mipTexels[mip] = texels;

            // Goto next texel MIP level
            texels += w*h;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }

    // Fill image data of first MIP level
    PRcolorindex* texels = texture->texels;

    _texture_subimage2d(texels, 0, width, height, format, data, dither);

    if (generateMips != PR_FALSE)
    {
        PRvoid* prevData = (PRvoid*)data;

        // Fill image data
        for (PRubyte mip = 1; mip < texture->mips; ++mip)
        {
            // Goto next texel MIP level
            texels += width*height;

            // Scale down image data
            data = _image_scale_down(width, height, format, prevData);

            if (mip > 1)
                PR_FREE(prevData);
            prevData = (PRvoid*)data;

            if (data == NULL)
            {
                _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
                return PR_FALSE;
            }

            // Halve MIP size
            if (width > 1)
                width /= 2;
            if (height > 1)
                height /= 2;

            // Fill image data for current MIP level
            _texture_subimage2d(texels, mip, width, height, format, data, dither);
        }

        PR_FREE(prevData);
    }

    return PR_TRUE;
}

PRboolean _pr_texture_subimage2d(
    pr_texture* texture, PRubyte mip, PRtexsize x, PRtexsize y, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data, PRboolean dither)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return PR_FALSE;
    }
    if (texture->texels == NULL || x < 0 || y < 0 || x + width >= texture->width || y + height >= texture->height || width <= 0 || height <= 0 || mip >= texture->mips)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return PR_FALSE;
    }

    // Fill image data for specified MIP level
    _texture_subimage2d_rect(texture, mip, x, y, width, height, format, data);

    return PR_TRUE;
}

PRubyte _pr_texture_num_mips(PRubyte maxSize)
{
    return maxSize > 0 ? (PRubyte)(floorf(log2f(maxSize))) + 1 : 0;
}

const PRcolorindex* _pr_texture_select_miplevel(const pr_texture* texture, PRubyte mip, PRtexsize* width, PRtexsize* height)
{
    // Add MIP level offset
    mip = PR_CLAMP((PRubyte)(((PRint)mip) + _stateMachine->textureLodBias), 0, texture->mips - 1);

    // Store mip size in output parameters
    *width = PR_MIP_SIZE(texture->width, mip);
    *height = PR_MIP_SIZE(texture->height, mip);

    // Return MIP-map texel offset
    return texture->mipTexels[mip];
}

/*PRubyte _pr_texture_compute_miplevel(const pr_texture* texture, PRfloat r1x, PRfloat r1y, PRfloat r2x, PRfloat r2y)
{
    // Compute derivation of u and v vectors
    r1x = fabsf(r1x);// * texture->width;
    r1y = fabsf(r1y);// * texture->height;

    r2x = fabsf(r2x);// * texture->width;
    r2y = fabsf(r2y);// * texture->height;

    // Select LOD by maximal derivation vector length (using dot product)
    PRfloat r1_len = sqrtf(r1x*r1x + r1y*r1y);
    PRfloat r2_len = sqrtf(r2x*r2x + r2y*r2y);
    PRfloat d = PR_MAX(r1_len, r2_len)*10.0f;
    
    // Clamp LOD to [0, texture->texture->mips)
    PRint lod = _int_log2(d);
    //PRint lod = (PRint)log2f(d);
    return (PRubyte)PR_CLAMP(lod, 0, texture->mips - 1);
}*/

PRcolorindex _pr_texture_sample_nearest_from_mipmap(const PRcolorindex* mipTexels, PRtexsize mipWidth, PRtexsize mipHeight, PRfloat u, PRfloat v)
{
    // Clamp texture coordinates
    PRint x = (PRint)((u - (PRint)u)*mipWidth);
    PRint y = (PRint)((v - (PRint)v)*mipHeight);

    if (x < 0)
        x += mipWidth;
    if (y < 0)
        y += mipHeight;

    // Sample from texels
    return mipTexels[y*mipWidth + x];
}

PRcolorindex _pr_texture_sample_nearest(const pr_texture* texture, PRfloat u, PRfloat v, PRfloat ddx, PRfloat ddy)
{
    // Select MIP-level texels by tex-coord derivation
    const PRfloat dx = ddx * texture->width;
    const PRfloat dy = ddy * texture->height;

    const PRfloat deltaMaxSq = PR_MAX(dx*dx, dy*dy);

    const PRint lod = _int_log2(deltaMaxSq);// / 2;
    const PRubyte mip = (PRubyte)PR_CLAMP(lod, 0, texture->mips - 1);

    // Get texels from MIP-level
    PRtexsize w, h;
    const PRcolorindex* texels = _pr_texture_select_miplevel(texture, mip, &w, &h);

    // Sample nearest texel
    return _pr_texture_sample_nearest_from_mipmap(texels, w, h, u, v);
    //return _pr_color_to_colorindex_r3g3b2(mip*20, mip*20, mip*20);
}

