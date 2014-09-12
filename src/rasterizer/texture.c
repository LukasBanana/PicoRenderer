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

#include <math.h>
#include <stdlib.h>


pr_texture* _pr_texture_create()
{
    // Create texture
    pr_texture* texture = (pr_texture*)malloc(sizeof(pr_texture));

    texture->width  = 0;
    texture->height = 0;
    texture->mips   = 0;
    texture->texels = NULL;

    return texture;
}

void _pr_texture_delete(pr_texture* texture)
{
    if (texture != NULL)
    {
        PR_SAFE_FREE(texture->texels);
        free(texture);
    }
}

static void _texture_subimage2d(
    pr_texture* texture, PRubyte mip, PRubyte x, PRubyte y, PRubyte width, PRubyte height, PRenum format, const PRubyte* data)
{
    //...
}

static PRubyte* _image_scale_down_ubyte_rgb(PRubyte width, PRubyte height, const PRubyte* data)
{
    PRubyte* scaled = NULL;

    //...

    return scaled;
}

static PRubyte* _image_scale_down_ubyte_rgba(PRubyte width, PRubyte height, const PRubyte* data)
{
    PRubyte* scaled = NULL;

    //...

    return scaled;
}

static PRubyte* _image_scale_down(PRubyte width, PRubyte height, PRenum format, const PRubyte* data)
{
    switch (format)
    {
        case PR_IMAGE_FORMAT_UBYTE_RGB:
            return _image_scale_down_ubyte_rgb(width, height, data);
        case PR_IMAGE_FORMAT_UBYTE_RGBA:
            return _image_scale_down_ubyte_rgba(width, height, data);
        default:
            break;
    }
    return NULL;
}

PRboolean _pr_texture_image2d(pr_texture* texture, PRubyte width, PRubyte height, PRenum format, const PRubyte* data)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER);
        return PR_FALSE;
    }
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return PR_FALSE;
    }

    // Setup texture dimension
    texture->width = width;
    texture->height = height;
    texture->mips = 0;

    // Determine number of texels
    size_t numTexels = 0;

    while (width > 1 || height > 1)
    {
        // Count number of texels
        numTexels += width*height;

        // Halve MIP size
        if (width > 1)
            width /= 2;
        if (height > 1)
            height /= 2;
        ++(texture->mips);
    }

    // Free previous texels
    PR_SAFE_FREE(texture->texels);

    // Create texels
    texture->texels = (PRubyte*)calloc(numTexels, sizeof(PRubyte));

    // Fill image data
    for (PRubyte mip = 0; mip < texture->mips; ++mip)
    {
        // Fill image data for current MIP level
        _texture_subimage2d(texture, mip, 0, 0, width, height, format, data);

        // Scale down image data
        data = _image_scale_down(width, height, format, data);

        if (data == NULL)
        {
            _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
            return PR_FALSE;
        }

        // Halve MIP size
        if (width > 1)
            width /= 2;
        if (height > 1)
            height /= 2;
    }

    return PR_TRUE;
}

PRboolean _pr_texture_subimage2d(
    pr_texture* texture, PRubyte mip, PRubyte x, PRubyte y, PRubyte width, PRubyte height, PRenum format, const PRubyte* data)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER);
        return PR_FALSE;
    }
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return PR_FALSE;
    }

    // Fill image data for specified MIP level
    _texture_subimage2d(texture, mip, x, y, width, height, format, data);

    return PR_TRUE;
}

PRubyte _pr_texture_num_mips(PRubyte maxSize)
{
    return maxSize > 0 ? (PRubyte)(ceilf(log2f(maxSize))) + 1 : 0;
}
