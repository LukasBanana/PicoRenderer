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

#include <rasterizer/image.h>


// --- global members --- //

PRobject context        = NULL;
PRobject frameBuffer    = NULL;
PRboolean isQuit        = PR_FALSE;

int mouseX = 0, mouseY = 0;

#define PI 3.141592654f


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

void ErrorCallback(PRenum errorID, const char* info)
{
    printf("PicoRenderer Error (%i): %s", errorID, info);
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

    #if 1
    const PRuint screenWidth  = 800;
    const PRuint screenHeight = 600;
    #else
    const PRuint screenWidth  = 1600;
    const PRuint screenHeight = 900;
    #endif

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

    // Initialize renderer
    prInit();
    prErrorHandler(ErrorCallback);

    printf("%s %s\n", prGetString(PR_STRING_RENDERER), prGetString(PR_STRING_VERSION));
    
    // Create render context
    pr_context_desc contextDesc;

    contextDesc.wnd = wnd;

    context = prGenContext(&contextDesc, screenWidth, screenHeight);

    // Create frame buffer
    frameBuffer = prGenFrameBuffer(screenWidth, screenHeight);
    prBindFrameBuffer(frameBuffer);

    // Create texture
    PRobject texture = prGenTexture();
    prTextureImage2DFromFile(texture, "media/banner.png", PR_TRUE, PR_TRUE);

    // Setup matrices
    float projection[16];
    prBuildPerspectiveProjection(projection, (float)screenWidth/screenHeight, 0.1f, 100.0f, 74.0f);
    prProjectionMatrix(projection);

    float worldMatrix[16];
    float objRotation = 0.0f;

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

        // Setup transformation
        prLoadIdentity(worldMatrix);
        prTranslate(worldMatrix, 0.0f, 0.0f, 3.0f);
        prRotate(worldMatrix, 1.0f, 1.0f, 1.0f, objRotation);
        prWorldMatrix(worldMatrix);

        objRotation += PI*0.1f;

        // Drawing
        prClearFrameBuffer(prGetColorIndex(255, 255, 255), 0.0f);
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

            prColor(prGetColorIndex(255, 0, 0));
            prBindTexture(texture);

            prDrawScreenImage(100, 100, mouseX, mouseY);

            #endif
        }
        prPresent(context);
    }

    // Clean up
    prDeleteTexture(texture);
    prDeleteFrameBuffer(frameBuffer);
    prDeleteContext(context);

    prRelease();

    DestroyWindow(wnd);

    UnregisterClass(wcName, GetModuleHandle(NULL));

    return 0;
}


