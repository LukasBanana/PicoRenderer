/*
 * test.c (SDL2)
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <stdio.h>
#include <pico.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>

#include <rasterizer/image.h>
#include <rasterizer/matrix4.h>


// --- global members --- //

SDL_Event event;

PRobject context        = NULL;
PRobject frameBuffer    = NULL;
PRboolean isQuit        = PR_FALSE;

int mouseX = 0, mouseY = 0;
int mouseSpeedX = 0, mouseSpeedY = 0;
int mouseWheel = 0;
int left = 0, right = 0, forward = 0, back = 0, faster = 0;
bool buttonDown[2] = { 0 };

bool keyDown[256] = { 0 };

PRenum polyMode = PR_POLYGON_FILL;

float pitch = 0.0f, yaw = 0.0f;
float posZ = 3.0f;

#define MOVE_CAMERA
#ifdef MOVE_CAMERA

#define MOVE_SPEED 0.02f

float camX = 0.0f, camY = 0.0f, camZ = -20.0f;

static void MoveCam(float x, float z)
{
    float matrix[16];
    prLoadIdentity(matrix);
    prRotate(matrix, 0.0f, 1.0f, 0.0f, -yaw);
    prRotate(matrix, 1.0f, 0.0f, 0.0f, -pitch);

    float out[3];
    float in[3] = { x, 0.0f, z };
    _pr_matrix_mul_float3(out, (pr_matrix4*)matrix, in);

    float speed = MOVE_SPEED;
    if (faster)
        speed *= 5.0f;

    camX += out[0] * speed;
    camY += out[1] * speed;
    camZ += out[2] * speed;
}

#endif

#define PI 3.141592654f


// --- functions --- //


void ErrorCallback(PRenum errorID, const char* info)
{
    printf("PicoRenderer Error (%i): %s\n", errorID, info);
}

int main()
{

    SDL_Init(SDL_INIT_EVERYTHING);

    #define SCREEN_MODE 1
    #if SCREEN_MODE == 1
    const PRuint screenWidth  = 640;//800;
    const PRuint screenHeight = 480;//600;
    const PRboolean isFullscreen = PR_FALSE;
    #elif SCREEN_MODE == 2
    const PRuint screenWidth  = 1280;
    const PRuint screenHeight = 768;
    const PRboolean isFullscreen = PR_TRUE;
    #elif SCREEN_MODE == 3
    const PRuint screenWidth  = 1600;
    const PRuint screenHeight = 900;
    const PRboolean isFullscreen = PR_FALSE;
    #else
    const PRuint screenWidth  = 1920;
    const PRuint screenHeight = 1080;
    const PRboolean isFullscreen = PR_TRUE;
    #endif

    // Initialize renderer
    prInit();
    prErrorHandler(ErrorCallback);

    printf("%s %s\n", prGetString(PR_STRING_RENDERER), prGetString(PR_STRING_VERSION));

    // Create render context
    PRcontextdesc contextDesc;

    contextDesc.window = (void *)SDL_CreateWindow("pico_renderer_test(SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

    context = prCreateContext(&contextDesc, screenWidth, screenHeight);

    // Create frame buffer
    frameBuffer = prCreateFrameBuffer(screenWidth, screenHeight);
    prBindFrameBuffer(frameBuffer);

    // Create textures
    #ifdef PR_COLOR_BUFFER_24BIT
    const PRboolean dither = PR_FALSE;
    #else
    const PRboolean dither = PR_TRUE;
    #endif

    PRobject textureA = prCreateTexture();
    prTexImage2DFromFile(textureA, "media/house.jpg", dither, PR_TRUE); //TODO change it back to "media/house.jpg"

    PRobject textureB = prCreateTexture();
    prTexImage2DFromFile(textureB, "media/tiles.png", dither, PR_TRUE);

    //prTexEnvi(PR_TEXTURE_LOD_BIAS, 1);

    // Create vertex buffer
    PRobject vertexBuffer = prCreateVertexBuffer();
    PRobject indexBuffer = prCreateIndexBuffer();

    #if 0

    #define NUM_VERTICES 24

    PRvertex cubeVertices[NUM_VERTICES] =
    {
        //  x      y      z     u     v
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

    //float size[3] = { 2.0f, 2.0f, 0.5f };
    float size[3] = { 1.0f, 1.0f, 1.0f };

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

    float size[3] = { 5, -5, 5 };//0.7f, -0.7f, 0.7f };

    #endif

    // Setup matrices
    float projectionA[16], projectionB[16], worldMatrix[16], viewMatrix[16];

    #if 0
    PRuint viewWidth = 200;
    PRuint viewHeight = 200;
    #else
    PRuint viewWidth = screenWidth;
    PRuint viewHeight = screenHeight;
    #endif

    float orthoSize = 0.007f;//0.02f;
    prBuildPerspectiveProjection(projectionA, (float)viewWidth/viewHeight, 0.01f, 100.0f, 74.0f * PR_DEG2RAD);
    prBuildOrthogonalProjection(projectionB, orthoSize*viewWidth, orthoSize*viewHeight, 0.1f, 100.0f);

    prLoadIdentity(viewMatrix);
    prViewMatrix(viewMatrix);

    //prCullMode(PR_CULL_BACK);
    prCullMode(PR_CULL_FRONT);

    clock_t startTime = clock();
    int fps = 0;

    prEnable(PR_MIP_MAPPING);

    // Main loop
    while (!isQuit)
    {
        // Reset input states

        SDL_GetMouseState(&mouseX, &mouseY);

        mouseSpeedX = (mouseX - (screenWidth/2));
        mouseSpeedY = (mouseY - (screenHeight/2));
        mouseWheel = 0;



        // Window event handling

        while (SDL_PollEvent(&event) != 0)
        {
          // Update user input

          if(event.type == SDL_QUIT)
            isQuit = 1;

          switch(event.type)
          {
            case SDL_MOUSEBUTTONDOWN:
              SDL_GetMouseState(&mouseX, &mouseY);
              yaw     -= PI*0.0025f*(float)abs((mouseSpeedX - (screenWidth/2)) - (mouseX - (screenWidth/2)));//event.button.y
              pitch   += PI*0.0025f*(float)abs((mouseSpeedY - (screenHeight/2)) - (mouseY - (screenHeight/2)));
            break;
            case SDL_MOUSEBUTTONUP:

            break;
            case SDL_MOUSEWHEEL:
              posZ += 0.1f * event.wheel.y;
            break;
          }




          #ifdef MOVE_CAMERA

          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT)
              yaw     -= PI*0.0025f*(float)6.5;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT)
              yaw     += PI*0.0025f*(float)6.5;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP)
              pitch   += PI*0.0025f*(float)6.5;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
              pitch   -= PI*0.0025f*(float)6.5;




          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w)
              forward = 1;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
              back = 1;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a)
              left = 1;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d)
              right = 1;
          if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_w)
              forward = 0;
          if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_s)
              back = 0;
          if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_a)
              left = 0;
          if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_d)
              right = 0;
          if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LSHIFT)
              faster = 1;
          if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_LSHIFT)
              faster = 0;

          #endif
        }

        if (pitch < -PI*0.5f)
            pitch = -PI*0.5f;
        if (pitch > PI*0.5f)
            pitch = PI*0.5f;

        prLoadIdentity(viewMatrix);
        prRotate(viewMatrix, 1.0f, 0.0f, 0.0f, pitch);
        prRotate(viewMatrix, 0.0f, 1.0f, 0.0f, yaw);
        prRotate(viewMatrix, 0.0f, 0.0f, 1.0f, 180.0f * PR_DEG2RAD);
        prTranslate(viewMatrix, camX, camY, -camZ);
        prViewMatrix(viewMatrix);

        if(forward)
            MoveCam(0, 1);
        if(back)
            MoveCam(0, -1);
        if(left)
            MoveCam(-1, 0);
        if(right)
            MoveCam(1, 0);

        #if 0
        // Show FPS
        ++fps;
        if (clock()/CLOCKS_PER_SEC > startTime/CLOCKS_PER_SEC)
        {
            startTime = clock();
            printf("fps = %i\n", fps);
            fps = 0;
        }
        #endif



        // Drawing
        prClearColor(255, 255, 255);
        prClearFrameBuffer(frameBuffer, 0.0f, PR_COLOR_BUFFER_BIT | PR_DEPTH_BUFFER_BIT);
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
            prColor(0, 0, 255);

            // Setup transformation
            prProjectionMatrix(projectionB);
            prLoadIdentity(worldMatrix);
            #ifndef viewMatrix
            prTranslate(worldMatrix, 0.0f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            prRotate(worldMatrix, 0.0f, 1.0f, 0.0f, -yaw);
            #endif
            prScale(worldMatrix, size[0], size[1], size[2]);
            prWorldMatrix(worldMatrix);

            // Draw lines
            prDrawIndexed(PR_LINES, NUM_INDICES, 0);

            // Setup view
            prViewport(viewWidth, 0, 600, 600);
            prColor(255, 0, 0);

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

            prColor(0, 0, 255);

            // Bind buffers
            prBindVertexBuffer(vertexBuffer);
            prBindIndexBuffer(indexBuffer);
            prBindTexture(textureA);

            // Setup view
            prViewport(0, 0, viewWidth, viewHeight);

            //prEnable(PR_SCISSOR);
            //prScissor(20, 20, viewWidth - 100, viewHeight - 100);

            prPolygonMode(polyMode);

            // Setup transformation
            prProjectionMatrix(projectionA);
            prLoadIdentity(worldMatrix);
            #ifndef MOVE_CAMERA
            prTranslate(worldMatrix, 0.0f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            prRotate(worldMatrix, 0.0f, 1.0f, 0.0f, yaw);
            #endif
            prScale(worldMatrix, size[0], size[1], size[2]);
            prWorldMatrix(worldMatrix);

            // Draw triangles
            //prDepthRange(0.0f, 0.5f);
            prDrawIndexed(PR_TRIANGLES, NUM_INDICES, 0);

            #if 1
            for (int i = 1; i < 9; ++i)
            {
                worldMatrix[12] = ((float)(i % 3))*5*size[0];
                worldMatrix[14] = ((float)(i / 3))*5*size[2];
                prWorldMatrix(worldMatrix);
                prDrawIndexed(PR_TRIANGLES, NUM_INDICES, 0);
            }
            #endif

            // Draw floor
            float floorSize = 100.0f;
            int numQuads = 50;

            prLoadIdentity(worldMatrix);
            prTranslate(worldMatrix, 0, 4, 0);
            prScale(worldMatrix, floorSize, floorSize, floorSize);
            prWorldMatrix(worldMatrix);

            prBindTexture(textureB);

            prBegin(PR_TRIANGLES);
            {
                #if defined(PR_PERSPECTIVE_CORRECTED) && 0

                // Only use a single quad when textures are perspective corrected
                prTexCoord2i(0, 0); prVertex3f(-1, 0, 1);
                prTexCoord2i(numQuads, 0); prVertex3f(1, 0, 1);
                prTexCoord2i(numQuads, numQuads); prVertex3f(1, 0, -1);

                prTexCoord2i(0, 0); prVertex3f(-1, 0, 1);
                prTexCoord2i(numQuads, numQuads); prVertex3f(1, 0, -1);
                prTexCoord2i(0, numQuads); prVertex3f(-1, 0, -1);

                #else

                // Use many quads to emulate perspective texture correction
                float step = 2.0f / (float)numQuads;

                for (float x = -1.0f; x < 1.0f; x += step)
                {
                    for (float z = -1.0f; z < 1.0f; z += step)
                    {
                        prTexCoord2i(0, 0); prVertex3f(x, 0, z + step);
                        prTexCoord2i(1, 0); prVertex3f(x + step, 0, z + step);
                        prTexCoord2i(1, 1); prVertex3f(x + step, 0, z);

                        prTexCoord2i(0, 0); prVertex3f(x, 0, z + step);
                        prTexCoord2i(1, 1); prVertex3f(x + step, 0, z);
                        prTexCoord2i(0, 1); prVertex3f(x, 0, z);
                    }
                }

                #endif
            }
            prEnd();

            #   if 0

            // Draw with immediate mode
            prBindTexture(0);
            prColor(255, 0, 0);

            static float angle;

            //angle += 0.02f;
            if (buttonDown[1])
                angle += 0.02f * mouseSpeedX;

            prLoadIdentity(worldMatrix);
            prProjectionMatrix(worldMatrix);
            #ifndef MOVE_CAMERA
            prTranslate(worldMatrix, 0, 0, 1.5f);
            prRotate(worldMatrix, 0, 0, 1, angle);
            #endif
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
            #ifndef MOVE_CAMERA
            prTranslate(worldMatrix, 1.5f, 0.0f, posZ);
            prRotate(worldMatrix, 1.0f, 1.0f, 1.0f, pitch);
            #endif
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

    SDL_Quit();

    return 0;
}
