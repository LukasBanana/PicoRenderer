/*
 * test.c (Linux)
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <pico.h>
#include <X11/Xlib.h>

// --- global members --- //

PRuint scrWidth = 640;
PRuint scrHeight = 480;


// --- functions --- //

int main()
{
    // Create X11 window
    Display* display = XOpenDisplay(NULL);

    if (!display)
    {
        fprintf(stderr, "Opening X11 display failed\n");
        return 1;
    }

    #if 1//TESTING

    int screen = DefaultScreen(display);
    printf("DefaultScreen = %i\n", screen);

    int displayWidth = DisplayWidth(display, screen);
    int displayHeight = DisplayHeight(display, screen);
    printf("DisplaySize = (%i, %i)\n", displayWidth, displayHeight);

    Window rootWnd = RootWindow(display, screen);
    printf("RootWindow = %lu\n", rootWnd);

    unsigned long whitePixel = WhitePixel(display, screen);
    unsigned long blackPixel = BlackPixel(display, screen);
    printf("WhitePixel = %lu\nBlackPixel = %lu\n", whitePixel, blackPixel);

    #endif

    unsigned int borderWidth = 5;

    int wndWidth = (int)scrWidth;
    int wndHeight = (int)scrHeight;

    Window wnd = XCreateSimpleWindow(
        display,
        rootWnd,
        displayWidth/2 - wndWidth /2,
        displayHeight/2 - wndHeight/2,
        wndWidth ,
        wndHeight,
        borderWidth,
        blackPixel, // border pixel value
        whitePixel // background pixel value
    );

    printf("wnd = %lu\n", wnd);

    XMapRaised(display, wnd);
    XMapWindow(display, wnd);


    // Initialize pico renderer
    prInit();


    PRcontextdesc contextDesc;
    //contextDesc.window = ;
    //prCreateContext(&contextDesc, );

    while (1)
    {



    }

    prRelease();

    return 0;
}

