/*
 * image.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "image.h"
#include "error.h"
#include "helper.h"
#include "static_config.h"
#include "color_palette.h"

#ifdef PR_INCLUDE_PLUGINS
#   define STB_IMAGE_IMPLEMENTATION
#   include "plugins/stb/stb_image.h"
#endif


pr_image* _pr_image_load_from_file(const char* filename)
{
    if (filename == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return NULL;
    }
    if (*filename == '\0')
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create image and load data from file
    pr_image* image = PR_MALLOC(pr_image);

    #ifdef PR_INCLUDE_PLUGINS

    image->width    = 0;
    image->height   = 0;
    image->format   = 0;
    image->colors   = stbi_load(filename, &(image->width), &(image->height), &(image->format), 3);
    image->defFree  = PR_FALSE;

    if (image->colors == NULL)
    {

    #endif

    image->width    = 1;
    image->height   = 1;
    image->format   = 3;
    image->colors   = PR_CALLOC(PRubyte, 3);
    image->defFree  = PR_TRUE;

    image->colors[0] = 0;
    image->colors[1] = 0;
    image->colors[2] = 0;

    #ifdef PR_INCLUDE_PLUGINS

    }

    #endif

    return image;
}

pr_image* _pr_image_create(PRint width, PRint height, PRint format)
{
    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create image
    pr_image* image = PR_MALLOC(pr_image);

    image->width    = width;
    image->height   = height;
    image->format   = format;
    image->defFree  = PR_TRUE;
    image->colors   = PR_CALLOC(PRubyte, width*height*format);

    return image;
}

void _pr_image_delete(pr_image* image)
{
    if (image != NULL)
    {
        if (image->colors != NULL)
        {
            #ifdef PR_INCLUDE_PLUGINS
            if (!image->defFree)
            {
                stbi_image_free(image->colors);
            }
            else
            #endif
            {
                //error???
                //PR_FREE(image->colors);
            }
        }
        PR_FREE(image);
    }
}


#define PIXEL(x, y) imageBuffer[(y)*imageWidth+(x)]
#define DITHER(c, s)

/*
This function implements the "Floyd-Steinberg" dithering algorithm.
The distribution pattern around the pixel 'px' is:
       [ px ] [7/16]
[3/16] [5/16] [1/16]
*/
static void _dither_color(PRint* buffer, PRint x, PRint y, PRint comp, PRint width, PRint height, PRint scale)
{
    #define COLOR(x, y) buffer[((y)*width + (x))*3 + comp]

    // Get old and new color
    int oldPixel = COLOR(x, y);
    int newPixel = (oldPixel/scale)*scale;
    
    // Write final color
    COLOR(x, y) = newPixel;
    
    // Get quantification error
    int quantErr = oldPixel - newPixel;

    // Apply dithering distribution
    if (x + 1 < width)
        COLOR(x + 1, y    ) += quantErr*7/16;
    if (x > 0 && y + 1 < height)
        COLOR(x - 1, y + 1) += quantErr*3/16;
    if (y + 1 < height)
        COLOR(x    , y + 1) += quantErr*5/16;
    if (x + 1 < width && y + 1 < height)
        COLOR(x + 1, y + 1) += quantErr*1/16;

    #undef COLOR
}

void _pr_image_color_to_colorindex_r3g3b2(PRubyte* dstColors, const pr_image* srcImage, PRboolean dither)
{
    // Validate and map input parameters
    if (dstColors == NULL || srcImage == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }
 
    const PRint width = srcImage->width;
    const PRint height = srcImage->height;
    const PRint format = srcImage->format;

    if (width <= 0 || height <= 0 || format < 1 || format > 4)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    const PRubyte* src = srcImage->colors;

    const PRuint numPixels = width*height;

    if (dither != PR_FALSE)
    {
        // Fill temporary integer buffer
        const PRuint numColors = width*height*3;
        PRint* buffer = PR_CALLOC(PRint, numColors);

        if (format < 3)
        {
            // Copy gray scale image into buffer
            for (PRuint i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i    ] = src[j];
                buffer[i + 1] = src[j];
                buffer[i + 2] = src[j];
            }
        }
        else
        {
            // Copy RGB image into buffer
            for (PRuint i = 0, j = 0; i < numColors; i += 3, j += format)
            {
                buffer[i    ] = (PRint)src[j    ];
                buffer[i + 1] = (PRint)src[j + 1];
                buffer[i + 2] = (PRint)src[j + 2];
            }
        }

        // Apply dithering to buffer
        for (PRint y = 0; y < height; ++y)
        {
            for (PRint x = 0; x < width; ++x)
            {
                _dither_color(buffer, x, y, 0, width, height, PR_COLORINDEX_SCALE_RED  );
                _dither_color(buffer, x, y, 1, width, height, PR_COLORINDEX_SCALE_GREEN);
                _dither_color(buffer, x, y, 2, width, height, PR_COLORINDEX_SCALE_BLUE );
            }
        }

        // Finally convert buffer to color index
        for (PRuint i = 0, j = 0; i < numPixels; ++i, j += 3)
            dstColors[i] = _pr_color_to_colorindex_r3g3b2(buffer[j], buffer[j + 1], buffer[j + 2]);

        // Delete temporary buffer
        PR_FREE(buffer);
    }
    else
    {
        // Copy image data
        if (format < 3)
        {
            // Copy gray scale image into color index
            PRubyte gray;

            for (PRuint i = 0, j = 0; i < numPixels; ++i, j += format)
            {
                gray = src[j];
                dstColors[i] = _pr_color_to_colorindex_r3g3b2(gray, gray, gray);
            }
        }
        else
        {
            // Copy RGB image into color index
            for (PRuint i = 0, j = 0; i < numPixels; ++i, j += format)
                dstColors[i] = _pr_color_to_colorindex_r3g3b2(src[j], src[j + 1], src[j + 2]);
        }
    }
}

