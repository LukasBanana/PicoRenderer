/*
 * context.c (OSX)
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#import <Cocoa/Cocoa.h>

#include "context.h"
#include "error.h"
#include "helper.h"


// Custom view to display context bitmap in NSWindow

@interface PicoNSView : NSView
{
    pr_context* context;
}

@end

@implementation PicoNSView

- (id)initWithFrame:(NSRect)frame ctx:(pr_context*)pContext;
{
    self = [super initWithFrame:frame];
    context = pContext;
    return self;
}

- (void)drawRect:(NSRect)pRect
{
    NSBitmapImageRep* bmp = (NSBitmapImageRep*)context->bmp;
    [bmp draw];
}

@end


// MacOS specific graphics context

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

    // Store OSX objects
    context->wnd = (void*)desc->window;
    context->bmp = (void*)[[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:   nil
        pixelsWide:                 width
        pixelsHigh:                 height
        bitsPerSample:              8
        samplesPerPixel:            3
        hasAlpha:                   NO
        isPlanar:                   NO
        colorSpaceName:             NSDeviceRGBColorSpace//NSCalibratedRGBColorSpace
        bytesPerRow:                (3 * width)
        bitsPerPixel:               24
    ];

    // Create graphics context
    NSWindow* wnd = (NSWindow*)desc->window;

    // Create pixel buffer
    context->width  = width;
    context->height = height;
    
    // Create color palette
    context->colorPalette = PR_MALLOC(pr_color_palette);
    _pr_color_palette_fill_r3g3b2(context->colorPalette);

    // Initialize state machine
    _pr_state_machine_init(&(context->stateMachine));
    _pr_context_makecurrent(context);

    // Use custom view to display context bitmap
    wnd.contentView = [[PicoNSView alloc]
        initWithFrame:  [[wnd contentView] bounds]
        ctx:            context
    ];

    return context;
}

void _pr_context_delete(pr_context* context)
{
    if (context != NULL)
    {
        _pr_ref_assert(&(context->stateMachine));
        
        // Delete OSX objects
        [((NSBitmapImageRep*)context->bmp) release];

        free(context->colorPalette);
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
    if (context->width != framebuffer->width || context->height != framebuffer->height)
    {
        _pr_error_set(PR_ERROR_ARGUMENT_MISMATCH, __FUNCTION__);
        return;
    }

    // Get iterators
    const PRuint num = context->width * context->height;

    NSBitmapImageRep* bmp = (NSBitmapImageRep*)context->bmp;

    pr_color* dst = (pr_color*)[bmp bitmapData];
    pr_color* dstEnd = dst + num;

    const pr_pixel* pixels = framebuffer->pixels;

    const pr_color* palette = context->colorPalette->colors;
    const pr_color* paletteColor;

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

    // Trigger content view to be redrawn
    NSWindow* wnd = (NSWindow*)context->wnd;
    [[wnd contentView] setNeedsDisplay:YES];
}

