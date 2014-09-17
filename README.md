PicoRenderer
============

This project provides a simple, 8-bit colored, 3D software renderer written in C99.
It has an interface similiar to OpenGL 1.1.


Plugins
=======

This project makes use of the "stb_image" library (see https://github.com/nothings/stb)


Build
=====
- CMake 2.8 (http://www.cmake.org/)
- C99 compliant compiler


Why C and not C++?
==================
C code is slim, highly portable, compiles fast and for a low-level software renderer one doesn't need lots of object-oriented or templated code.


Getting Started
===============
```c
// PicoRenderer (PR) interface example
// (interface still in development)
#include <pico/pico.h>

int main()
{
  if (!prInit())
    return 1;
  
  // Create window
  #ifdef WIN32
  //HWND hWnd = ...
  #endif
  
  // Create render context
  PRuint scrWidth = 800, scrHeight = 600;
  PRcontextdesc contextDesc;
  
  #if defined(WIN32)
  //contextDesc.hWnd = ...
  #endif
  
  PRobject context = prGenContext(&contextDesc, scrWidth, scrHeight);
  prMakeCurrent(context);
  
  // Create frame buffer
  PRobject frameBuffer = prGenFrameBuffer(scrWidth, scrHeight);
  prBindFrameBuffer(frameBuffer);
  
  prViewport(0, 0, scrWidth, scrHeight);
  
  // Create vertex buffer
  PRobject vertexBuffer = prGenVertexBuffer();
  PRfloat coords[3*3] = { 0,1,0,  1,-1,0,  -1,-1,0 };
  prVertexBufferData(
    vertexBuffer,       // output vertex buffer
    3,                  // number of vertices
    coords,             // vertex coordinates pointer
    NULL,               // texture coordinates pointer (none)
    sizeof(PRfloat)*3,  // vertex stride size (in bytes)
  );
  
  // Setup projection matrix
  PRfloat projection[16];
  prBuildPerspectiveProjection(
    projection,                   // output matrix
    (PRfloat)scrWidth/scrHeight,  // aspect ratio
    1.0f,                         // near clipping plane
    100.0f,                       // far clipping plane
    PR_DEG2RAD(74.0f)             // field of view (fov) in radians
  );
  prProjectionMatrix(projection);
  
  // World transform
  PRfloat worldMatrix[16];
  PRfloat rotation = 0.0f;
  
  // Main loop
  PRboolean isQuit = PR_FALSE;
  
  while (!isQuit)
  { 
    // Update user input
    //...
    
    // Setup world matrix
    prLoadIdentity(worldMatrix);
    prTranslate(worldMatrix, 0, 0, 2);
    prRotate(worldMatrix, 0, 1, 0, rotation);
    prWorldMatrix(worldMatrix);
    rotation += 0.01f;
    
    // Draw scene
    prBindVertexBuffer(vertexBuffer);
    prDraw(PR_TRIANGLES, 3, 0);
    
    // Show frame buffer on render context
    prPresent();
  }
  
  // Release all objects
  prDeleteVertexBuffer(vertexBuffer);
  prDeleteFrameBuffer(frameBuffer);
  prDeleteContext(context);
  
  prRelease();
  
  return 0;
}
```
