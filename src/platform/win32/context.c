/*
 * context.c (WIN32)
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "context.h"
#include "error.h"
#include "helper.h"


pr_context* _pr_context_create(const pr_context_desc* desc, PRuint width, PRuint height)
{
    if (desc == NULL || desc->dc == NULL || width <= 0 || height <= 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT);
        return NULL;
    }

    // Create render context
    pr_context* context = (pr_context*)malloc(sizeof(pr_context));

    // Setup bitmap info structure
    BITMAPINFO bmi;
    PR_ZERO_MEMORY(bmi);

    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = (LONG)width;
    bmi.bmiHeader.biHeight      = (LONG)height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create DIB section
    context->dc         = desc->dc;
    context->dibSection = CreateDIBSection(desc->dc, &bmi, DIB_RGB_COLORS, (void**)(&context->colors), NULL, 0);
    context->width      = width;
    context->height     = height;

    // Create color palette
    context->colorPalette = (pr_color_palette*)malloc(sizeof(pr_color_palette));
    _pr_color_palette_fill_r3g3b2(context->colorPalette);

    return context;
}

void _pr_context_delete(pr_context* context)
{
    if (context != NULL)
    {
        if (context->dibSection != NULL)
            DeleteObject(context->dibSection);
        free(context->colorPalette);
        free(context);
    }
}

void _pr_context_present(pr_context* context, const pr_framebuffer* framebuffer)
{
    if (context == NULL || framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER);
        return;
    }
    if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        _pr_error_set(PR_ERROR_ARGUMENT_MISMATCH);
        return;
    }

    // Get iterators
    pr_color_rgb* dst = context->colors;
    const pr_pixel* pixels = framebuffer->pixels;

    const PRuint num = context->width*context->height;
    pr_color_rgb* dstEnd = dst + num;

    const pr_color_rgb* palette = context->colorPalette->colors;
    const pr_color_rgb* paletteColor;

    // Iterate over all pixels
    while (dst != dstEnd)
    {
        paletteColor = (palette + pixels->colorIndex);

        dst->r = paletteColor->r;
        dst->g = paletteColor->g;
        dst->b = paletteColor->b;

        ++dst;
        ++pixels;
    }
}

