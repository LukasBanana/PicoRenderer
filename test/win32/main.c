/*
 * test.c (WIN32)
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <pico.h>

#define STB_IMAGE_IMPLEMENTATION
#include "plugins/stb/stb_image.h"

// --- global members --- //

PRobject context        = NULL;
PRobject framebuffer    = NULL;
PRboolean isQuit        = PR_FALSE;

int mouseX = 0, mouseY = 0;


// --- functions --- //

LRESULT CALLBACK window_callback(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
                isQuit = PR_TRUE;
        }
        break;

        case WM_MOUSEMOVE:
        {
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
        }
        break;

        case WM_CLOSE:
        {
            isQuit = PR_TRUE;
        }
        break;
    }
    return DefWindowProc(wnd, message, wParam, lParam);
}

int main()
{
    // Register window class
    static const char* wcName = "__pico_test_class__";

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));
    {
        wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
        wc.hInstance        = GetModuleHandle(NULL);
        wc.lpfnWndProc      = (WNDPROC)window_callback;
        wc.hIcon            = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor          = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
        wc.lpszMenuName     = NULL;
        wc.lpszClassName    = wcName;
    }
    if (!RegisterClass(&wc))
    {
        puts("registering window class failed");
        return 1;
    }

    // Create window
    int desktopWidth    = GetSystemMetrics(SM_CXSCREEN);
    int desktopHeight   = GetSystemMetrics(SM_CYSCREEN);

    PRuint screenWidth  = 800;
    PRuint screenHeight = 600;

    DWORD winStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;

    RECT winRect;
    {
        winRect.left    = desktopWidth/2 - screenWidth/2;
        winRect.top     = desktopHeight/2 - screenHeight/2;
        winRect.right   = desktopWidth/2 + screenWidth/2;
        winRect.bottom  = desktopHeight/2 + screenHeight/2;
    }
    AdjustWindowRect(&winRect, winStyle, FALSE);

    HWND wnd = CreateWindow(
        wcName,
        "pico_renderer test (WIN32)",
        winStyle,
        winRect.left,
        winRect.top,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        HWND_DESKTOP,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    ShowWindow(wnd, SW_SHOW);

    // Create render context
    pr_context_desc contextDesc;

    contextDesc.wnd = wnd;

    prInit();

    context = prGenContext(&contextDesc, screenWidth, screenHeight);

    // Create frame buffer
    framebuffer = prGenFramebuffer(screenWidth, screenHeight);
    prBindFramebuffer(framebuffer);

    // Load image
    typedef struct rgb
    {
        int r, g, b;
    }
    rgb;

    int imageWidth, imageHeight, imageComp;
    PRubyte* imageRawBuffer = (PRubyte*)stbi_load("media/pic.png", &imageWidth, &imageHeight, &imageComp, 3);

    rgb* imageBuffer = (rgb*)calloc(imageWidth*imageHeight, sizeof(rgb));

    for (int i = 0, n = imageWidth*imageHeight; i < n; ++i)
    {
        imageBuffer[i].r = imageRawBuffer[i*3+0];
        imageBuffer[i].g = imageRawBuffer[i*3+1];
        imageBuffer[i].b = imageRawBuffer[i*3+2];
    }

    stbi_image_free(imageRawBuffer);

    #define PIXEL(x, y) imageBuffer[(y)*imageWidth+(x)]
    #define DITHER(c, s)                                    \
        {                                                   \
            int oldPixel = PIXEL(x, y).c;                   \
            int newPixel = (oldPixel/s)*s;                  \
            PIXEL(x, y).c = newPixel;                       \
            int quantError = oldPixel - newPixel;           \
            if (x+1 < imageWidth)                           \
                PIXEL(x+1, y  ).c += quantError*7/16;       \
            if (x > 0 && y+1 < imageHeight)                 \
                PIXEL(x-1, y+1).c += quantError*3/16;       \
            if (y+1 < imageHeight)                          \
                PIXEL(x  , y+1).c += quantError*5/16;       \
            if (x+1 < imageWidth && y+1 < imageHeight)      \
                PIXEL(x+1, y+1).c += quantError*1/16;       \
        }

    #if 1
    for (int y = 0; y < imageHeight; ++y)
    {
        for (int x = 0; x < imageWidth; ++x)
        {
            DITHER(r, 36);
            DITHER(g, 36);
            DITHER(b, 85);
        }
    }
    #endif

    // Main loop
    while (!isQuit)
    {
        // Window event handling
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Drawing
        prClearFramebuffer(prGetColorIndex(255, 255, 255), 0.0f);
        {
            #if 0
            prDrawScreenLine(300, 200, 500, 200, prGetColorIndex(255, 0, 0));
            prDrawScreenLine(500, 200, 500, 400, prGetColorIndex(0, 255, 0));
            prDrawScreenLine(500, 400, 300, 400, prGetColorIndex(0, 0, 255));
            prDrawScreenLine(300, 400, 300, 200, prGetColorIndex(0, 255, 255));

            prDrawScreenLine(10, 10, mouseX, mouseY, prGetColorIndex(255, 255, 0));
            #elif 0
            for (int y = 0; y < 256; ++y)
                prDrawScreenLine(100, 100 + y, 356, 100 + y, prGetColorIndex(y, y, y));
            #elif 1

            for (int y = 0; y < imageHeight; ++y)
            {
                for (int x = 0; x < imageWidth; ++x)
                {
                    rgb* clr = &(PIXEL(x, y));
                    prDrawScreenPoint(
                        100 + x, 100 + y,
                        prGetColorIndex(clr->r, clr->g, clr->b)
                   );
                }
            }

            #endif
        }
        prContextPresent(context);
    }

    // Clean up
    free(imageBuffer);

    prDeleteFramebuffer(framebuffer);
    prDeleteContext(context);

    prRelease();

    DestroyWindow(wnd);

    UnregisterClass(wcName, GetModuleHandle(NULL));

    return 0;
}


