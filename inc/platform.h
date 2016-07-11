/*
 * platform.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PLATFORM_H__
#define __PR_PLATFORM_H__


#if defined(_WIN32)
#   include <Windows.h>
#   define PR_PLATFORM_WIN32
#elif defined(__APPLE__)
#   define PR_PLATFORM_OSX
#elif defined(__linux__)
#   define PR_PLATFORM_LINUX
#else
#   error OS not supported yet
#endif


#if defined(PR_PLATFORM_WIN32)

//! Win32 render context description structure.
typedef struct PRcontextdesc
{
    HWND window;
}
PRcontextdesc;

#elif defined(PR_PLATFORM_OSX)

//! OSX render context description structure.
typedef struct PRcontextdesc
{
    void* window;
}
PRcontextdesc;

#elif defined(PR_PLATFORM_LINUX)

//! Linux render context description structure.
typedef struct PRcontextdesc
{
    int unused;
}
PRcontextdesc;

#endif


#endif
