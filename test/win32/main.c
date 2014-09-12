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
        prClearFramebuffer(prGetColorIndex(0, 0, 120), 0.0f);

        prDrawScreenLine(10, 10, mouseX, mouseY, prGetColorIndex(255, 255, 0));
        //...

        prContextPresent(context);
    }

    // Clean up
    prDeleteFramebuffer(framebuffer);
    prDeleteContext(context);

    prRelease();

    DestroyWindow(wnd);

    UnregisterClass(wcName, GetModuleHandle(NULL));

    return 0;
}


