/*
 * test.c (Linux)
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <pico.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>

// --- global members --- //

PRuint scrWidth = 640;
PRuint scrHeight = 480;


// --- functions --- //

int main()
{
    // Open connection to X11 server
    Display* display = XOpenDisplay(NULL);

    if (!display)
    {
        fprintf(stderr, "Opening X11 display failed\n");
        return 1;
    }

    // Get default resources
    int             screen          = DefaultScreen(display);
    Visual*         visual          = DefaultVisual(display, screen);
    int             depth           = DefaultDepth(display, screen);
    Window          rootWnd         = RootWindow(display, screen);

    int             displayWidth    = DisplayWidth(display, screen);
    int             displayHeight   = DisplayHeight(display, screen);

    unsigned long   whitePixel      = WhitePixel(display, screen);
    unsigned long   blackPixel      = BlackPixel(display, screen);

    XSetWindowAttributes attribs;
    attribs.background_pixel = whitePixel;

    #if 1//DEBUG
    printf("DefaultScreen = %i\n", screen);
    printf("DisplaySize = (%i, %i)\n", displayWidth, displayHeight);
    printf("RootWindow = %lu\n", rootWnd);
    printf("WhitePixel = %lu\nBlackPixel = %lu\n", whitePixel, blackPixel);
    #endif

    // Create window
    unsigned int borderWidth = 5;

    int wndWidth = (int)scrWidth;
    int wndHeight = (int)scrHeight;

    Window wnd = XCreateWindow(
        display,
        rootWnd,
        displayWidth/2 - wndWidth/2,
        displayHeight/2 - wndHeight/2,
        wndWidth,
        wndHeight,
        borderWidth,
        depth,
        InputOutput,
        visual,
        CWBackPixel,
        &attribs
    );

    printf("wnd = %lu\n", wnd);

    //XMapRaised(display, wnd);
    XStoreName(display, wnd, "pico_renderer test (Linux)");
    XSelectInput(display, wnd, ExposureMask | KeyPressMask);
    XMapWindow(display, wnd);

    // Create graphics context
    //GC gfx = XCreateGC(display, wnd, GCFont + GCForeground);


    // Initialize pico renderer
    prInit();


    PRcontextdesc contextDesc;
    //contextDesc.window = ;
    //prCreateContext(&contextDesc, );

    // Main loop
    XEvent event;

    PRboolean isQuit = PR_FALSE;

    while (!isQuit)
    {
        // Update window event
        XNextEvent(display, &event);

        switch (event.type)
        {
            case KeyPress:
            {
                switch (event.xkey.keycode)
                {
                    case 9: // ESC
                        isQuit = PR_TRUE;
                        break;
                }
                printf("Key Pressed: %i\n", event.xkey.keycode);
            }
            break;
        }

        // Draw scene
        //todo...
    }

    // Clean up
    XCloseDisplay(display);

    prRelease();

    return 0;
}

