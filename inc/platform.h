/*
 * platform.h
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_PLATFORM_H
#define PR_PLATFORM_H


//! Render context description structure.
typedef struct PRcontextdesc
{
    /**
    Reference to the OS dependent window.
    - For Win32, this must be from type 'const HWND*'
    - For MacOS, this must be from type 'const NSWindow*'
    - For Linux, this must be from type 'const Window*'
    */
    const void* window;
}
PRcontextdesc;


#endif
