/*
 * test.c (OSX)
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#import <Cocoa/Cocoa.h>

#include <pico.h>
#include <stdio.h>


// --- global members --- //

PRobject context        = NULL;
PRobject frameBuffer    = NULL;
PRboolean isQuit        = PR_FALSE;

PRint screenWidth       = 640;
PRint screenHeight      = 480;


// --- interfaces --- //

@interface AppDelegate : NSObject
{
    BOOL _quit;
}
- (id) initApp;
- (BOOL) isQuit;
@end

@implementation AppDelegate

- (id) initApp
{
    self = [super init];
    _quit = FALSE;
    return (self);
}

- (void) windowWillClose:(id)sender
{
    _quit = TRUE;
}

- (BOOL) isQuit
{
    return (_quit);
}


@end


// --- functions --- //

void postKeyEvent(void* event)
{
    NSString* s = [(NSEvent*)event characters];
    
    if (s != nil && [s length] > 0)
    {
        unsigned int c = [s characterAtIndex:0];
        
        switch (c)
        {
            case 27: // ESC
                isQuit = PR_TRUE;
                break;
        }
    }
    
    [s release];
}


int main(int argc, char* argv[])
{
    // Create window with cocoa framework
    [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    [NSApp setDelegate:[[[AppDelegate alloc] initApp] autorelease]];
    [NSBundle loadNibNamed:@"MainMenu" owner:[NSApp delegate]];
    [NSApp finishLaunching];
    
    NSWindow* window = NULL;
    
    window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, (CGFloat)screenWidth, (CGFloat)screenHeight)
        styleMask:(NSTitledWindowMask + NSClosableWindowMask)
        backing:NSBackingStoreBuffered
        defer:FALSE
    ];
    
    [window center];
    [window setDelegate:[NSApp delegate]];
    [window setAcceptsMouseMovedEvents:TRUE];
    [window setIsVisible:TRUE];
    [window makeKeyAndOrderFront:nil];
    
    // Initialize renderer
    prInit();
    
    // Create graphics context
    PRcontextdesc contextDesc;
    contextDesc.window = (void*)window;
    PRobject context = prCreateContext(&contextDesc, screenWidth, screenHeight);
    
    // Create frame buffer
    frameBuffer = prCreateFrameBuffer(screenWidth, screenHeight);
    prBindFrameBuffer(frameBuffer);
    
    
    // Main loop
    while (!isQuit)
    {
        // Process events
        NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
        
        if (event != nil)
        {
            switch ([(NSEvent*)event type])
            {
                case NSKeyDown:
                    postKeyEvent(event);
                    break;
                    
                default:
                    [NSApp sendEvent:event];
                    break;
            }
        }
        
        [event release];
        
        if ([[NSApp delegate] isQuit])
            isQuit = PR_TRUE;
        
        // Draw scene
        //todo...
        
        prPresent(context);
    }
    
    // Clean up
    prRelease();
    
    return 0;
}

