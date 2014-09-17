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
#include <stdbool.h>

#include <rasterizer/image.h>


// --- global members --- //

PRobject context        = NULL;
PRobject frameBuffer    = NULL;
PRboolean isQuit        = PR_FALSE;

int mouseX = 0, mouseY = 0;
int mouseSpeedX = 0, mouseSpeedY = 0;
int mouseWheel = 0;
bool buttonDown[2] = { 0 };

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
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            mouseSpeedX = x - mouseX;
            mouseSpeedY = y - mouseY;

            mouseX = x;
            mouseY = y;
        }
        break;

        case WM_LBUTTONDOWN:
            buttonDown[0] = true;
            break;
        case WM_LBUTTONUP:
            buttonDown[0] = false;
            break;

        case WM_RBUTTONDOWN:
            buttonDown[1] = true;
            break;
        case WM_RBUTTONUP:
            buttonDown[1] = false;
            break;

        case WM_MOUSEWHEEL:
        {
            mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        }
        return 0;

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
    printf("PicoRenderer Error (%i): %s\n", errorID, info);
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
    PRcontextdesc contextDesc;

    contextDesc.wnd = wnd;

    context = prGenContext(&contextDesc, screenWidth, screenHeight);

    // Create frame buffer
    frameBuffer = prGenFrameBuffer(screenWidth, screenHeight);
    prBindFrameBuffer(frameBuffer);

    // Create textures
    PRobject textureA = prGenTexture();
    prTextureImage2DFromFile(textureA, "media/crate.png", PR_TRUE, PR_TRUE);

    PRobject textureB = prGenTexture();
    prTextureImage2DFromFile(textureB, "media/front.png", PR_TRUE, PR_TRUE);

    // Create vertex buffer
    PRobject vertexBuffer = prGenVertexBuffer();
    PRobject indexBuffer = prGenIndexBuffer();

    #if 1

    #define NUM_VERTICES 24

    PRvertex cubeVertices[NUM_VERTICES] =
    {
        // front
        { -1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f },

        // back
        {  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        { -1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        {  1.0f, -1.0f,  1.0f, 0.0f, 1.0f },

        // left
        { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        { -1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f, 1.0f },

        // right
        {  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        {  1.0f, -1.0f, -1.0f, 0.0f, 1.0f },

        // top
        { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f,  1.0f, -1.0f, 0.0f, 1.0f },

        // bottom
        { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f, 1.0f }
    };
    prVertexBufferData(vertexBuffer, NUM_VERTICES, &(cubeVertices[0].x), &(cubeVertices[0].u), sizeof(PRvertex));

    #define NUM_INDICES 36

    PRushort cubeIndices[NUM_INDICES] =
    {
         0, 1, 2,    0, 2, 3, // front
         4, 5, 6,    4, 6, 7, // back
         8, 9,10,    8,10,11, // left
        12,13,14,   12,14,15, // right
        16,17,18,   16,18,19, // top
        20,21,22,   20,22,23, // bottom
    };
    prIndexBufferData(indexBuffer, cubeIndices, NUM_INDICES);

    float size[3] = { 2.0f, 2.0f, 0.5f };

    #else

    #define NUM_VERTICES numVertices
    #define NUM_INDICES numIndices

    PRsizei numVertices = 0, numIndices = 0;

    FILE* mdlFile = fopen("media/house_tris.pico", "rb");
    if (mdlFile)
    {
        prVertexBufferDataFromFile(vertexBuffer, &numVertices, mdlFile);
        prIndexBufferDataFromFile(indexBuffer, &numIndices, mdlFile);
        fclose(mdlFile);
    }
    else
        puts("could not open model file!");

    float size[3] = { 0.7f, -0.7f, 0.7f };

    #endif

    // Setup matrices
    float projectionA[16], projectionB[16], worldMatrix[16], viewMatrix[16];
    float pitch = 0.0f, yaw = 0.0f;
    float posZ = 3.0f;

    #if 0
    PRuint viewWidth = 200;
    PRuint viewHeight = 200;
    #else
    PRuint viewWidth = screenWidth;
    PRuint viewHeight = screenHeight;
    #endif

    float orthoSize = 0.007f;//0.02f;
    prBuildPerspectiveProjection(projectionA, (float)viewWidth/viewHeight, 0.1f, 100.0f, 90.0f * PR_DEG2RAD);
    prBuildOrthogonalProjection(projectionB, orthoSize*viewWidth, orthoSize*viewHeight, 0.1f, 100.0f);

    prLoadIdentity(viewMatrix);
    prViewMatrix(viewMatrix);

    //prCullMode(PR_CULL_BACK);
    prCullMode(PR_CULL_FRONT);
    prPolygonMode(PR_POLYGON_LINE);

    // Main loop
    while (!isQuit)
    {
        // Reset input states
        mouseSpeedX = 0;
        mouseSpeedY = 0;
        mouseWheel = 0;

        // Window event handling
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update user input
        if (buttonDown[0])
        {
            yaw     -= PI*0.0025f*(float)mouseSpeedX;
            pitch   += PI*0.0025f*(float)mouseSpeedY;
        }

        posZ += 0.1f * mouseWheel;

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

            #elif 0

            prColor(prGetColorIndex(255, 0, 0));
            prBindTexture(textureA);

            prDrawScreenImage(100, 100, mouseX, mouseY);

            #elif 0

            // Bind buffers
            prBindVertexBuffer(vertexBuffer);
            prBindIndexBuffer(indexBuffer);
            
            // Setup view
            prViewport(0, 0, viewWidth, viewHeight);
            prColor(prGetColorIndex(0, 0, 255));

            // Setup transformation
            prProjectionMatrix(projectionB);
            prLoadIdentity(worldMatrix);
            prTranslate(worldMatrix, 0.0f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            prRotate(worldMatrix, 0.0f, 1.0f, 0.0f, -yaw);
            prScale(worldMatrix, size[0], size[1], size[2]);
            prWorldMatrix(worldMatrix);

            // Draw lines
            prDrawIndexed(PR_LINES, NUM_INDICES, 0);

            // Setup view
            prViewport(viewWidth, 0, 600, 600);
            prColor(prGetColorIndex(255, 0, 0));

            // Setup transformation
            prProjectionMatrix(projectionA);
            prLoadIdentity(worldMatrix);
            prTranslate(worldMatrix, 0.0f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            prRotate(worldMatrix, 0.0f, 1.0f, 0.0f, yaw);
            prScale(worldMatrix, size[0], size[1], size[2]);
            prWorldMatrix(worldMatrix);

            // Draw lines
            prDrawIndexed(PR_LINES, NUM_INDICES, 0);

            #elif 1

            prColor(prGetColorIndex(0, 0, 255));

            // Bind buffers
            prBindVertexBuffer(vertexBuffer);
            prBindIndexBuffer(indexBuffer);
            prBindTexture(textureA);

            // Setup view
            prViewport(0, 0, viewWidth, viewHeight);

            prEnable(PR_SCISSOR);
            prScissor(20, 20, viewWidth - 100, viewHeight - 100);

            // Setup transformation
            prProjectionMatrix(projectionA);
            prLoadIdentity(worldMatrix);
            prTranslate(worldMatrix, 0.0f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            prRotate(worldMatrix, 0.0f, 1.0f, 0.0f, yaw);
            prScale(worldMatrix, size[0], size[1], size[2]);
            prWorldMatrix(worldMatrix);

            // Draw triangles
            //prDepthRange(0.0f, 0.5f);
            prDrawIndexed(PR_TRIANGLES, NUM_INDICES, 0);

            #   if 1

            // Draw with immediate mode
            prBindTexture(0);
            prColor(prGetColorIndex(255, 0, 0));

            static float angle;

            //angle += 0.02f;
            if (buttonDown[1])
                angle += 0.02f * mouseSpeedX;

            prLoadIdentity(worldMatrix);
            prProjectionMatrix(worldMatrix);
            prTranslate(worldMatrix, 0, 0, 1.5f);
            prRotate(worldMatrix, 0, 0, 1, angle);
            prScale(worldMatrix, 0.7f, 0.7f, 0.7f);
            prWorldMatrix(worldMatrix);

            prCullMode(PR_CULL_NONE);
            prPolygonMode(PR_POLYGON_FILL);
            prDisable(PR_SCISSOR);

            prViewport(0, 0, 100, 100);

            prBegin(PR_TRIANGLES);
            {
                prVertex2f(0, 1.155f);
                prVertex2f(1, -0.577f);
                prVertex2f(-1, -0.577f);
            }
            prEnd();

            #   endif

            #   if 0
            // Setup transformation
            prLoadIdentity(worldMatrix);
            prTranslate(worldMatrix, 1.5f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 1.0f, 1.0f, pitch);
            prWorldMatrix(worldMatrix);

            prBindTexture(textureB);

            // Draw triangles
            //prDepthRange(0.5f, 1.0f);
            prDrawIndexed(PR_TRIANGLES, NUM_INDICES, 0);
            #   endif

            #endif
        }
        prPresent(context);
    }

    // Clean up
    prDeleteTexture(textureA);
    prDeleteTexture(textureB);
    prDeleteVertexBuffer(vertexBuffer);
    prDeleteIndexBuffer(indexBuffer);
    prDeleteFrameBuffer(frameBuffer);
    prDeleteContext(context);

    prRelease();

    DestroyWindow(wnd);
    
    UnregisterClass(wcName, GetModuleHandle(NULL));

    return 0;
}


