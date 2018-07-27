/*
 * context.c (SDL2)
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "context.h"
#include "error.h"
#include "helper.h"

#include <stdlib.h>


pr_context* _currentContext = NULL;

pr_context* _pr_context_create(const PRcontextdesc* desc, PRuint width, PRuint height)
{
    if (desc == NULL || desc->window == NULL || width <= 0 || height <= 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return NULL;
    }

    // Create render context
    pr_context* context = PR_MALLOC(pr_context);

    // Open SDL2 window

    if (desc->window == NULL)
    {
        _pr_error_set(PR_ERROR_CONTEXT, __FUNCTION__);
        return NULL;
    }

    // Create SDL2 objects
    context->wnd = (SDL_Window*)desc->window;
    context->ren = SDL_CreateRenderer(context->wnd, -1, SDL_RENDERER_SOFTWARE);
    context->tex = SDL_CreateTexture(context->ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
    context->colors = PR_CALLOC(pr_color, width*height);
    context->width      = width;
    context->height     = height;

    if (context->ren == NULL)
    {
        _pr_error_set(PR_ERROR_CONTEXT, __FUNCTION__);
        return NULL;
    }

    if (context->tex == NULL)
    {
        _pr_error_set(PR_ERROR_CONTEXT, __FUNCTION__);
        return NULL;
    }

    // Create color palette
    context->colorPalette = PR_MALLOC(pr_color_palette);
    _pr_color_palette_fill_r3g3b2(context->colorPalette);

    // Initialize state machine
    _pr_state_machine_init(&(context->stateMachine));
    _pr_context_makecurrent(context);

    return context;
}

void _pr_context_delete(pr_context* context)
{
    if (context != NULL)
    {
        _pr_ref_assert(&(context->stateMachine));

        // Free SDL2 objects
        SDL_DestroyTexture(context->tex);
        SDL_DestroyRenderer(context->ren);
        SDL_DestroyWindow(context->wnd);

        free(context->colorPalette);
        free(context->colors);
        free(context);
    }
}

void _pr_context_makecurrent(pr_context* context)
{
    _currentContext = context;
    if (context != NULL)
        _pr_state_machine_makecurrent(&(context->stateMachine));
    else
        _pr_state_machine_makecurrent(NULL);
}

void _pr_context_present(pr_context* context, const pr_framebuffer* framebuffer)
{
    if (context == NULL || framebuffer == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return;
    }
    /*if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        _pr_error_set(PR_ERROR_ARGUMENT_MISMATCH, __FUNCTION__);
        return;
    }*/

    // Get iterators
    const PRuint num = context->width*context->height;

    pr_color* dst = context->colors;
    pr_color* dstEnd = dst + num;

    const pr_pixel* pixels = framebuffer->pixels;
    const pr_color* palette = context->colorPalette->colors;

    #ifndef PR_COLOR_BUFFER_24BIT
    const pr_color* paletteColor;
    #endif

    // Iterate over all pixels

    while (dst != dstEnd)
    {
        #ifdef PR_COLOR_BUFFER_24BIT
        *dst = pixels->colorIndex;
        #else
        paletteColor = (palette + pixels->colorIndex);

        dst->r = paletteColor->r;
        dst->g = paletteColor->g;
        dst->b = paletteColor->b;
        #endif

        ++dst;
        ++pixels;
    }
    SDL_RenderClear(context->ren);
    SDL_UpdateTexture(context->tex, NULL, context->colors, context->width * (sizeof(Uint8) * 3));
    SDL_RenderCopy(context->ren, context->tex, NULL, NULL);
    SDL_RenderPresent(context->ren);
}
