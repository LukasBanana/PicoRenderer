/*
 * platform.h
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PLATFORM_H__
#define __PR_PLATFORM_H__


//! Render context description structure.
typedef struct PRcontextdesc
{
    /**
    Reference to the OS dependent window.
    - For Win32, this must be from type 'HWND*'
    - For MacOS, this must be from type 'NSWindow*'
    - For Linux, this must be from type 'Window*'
    */
    void* window;
}
PRcontextdesc;


#endif
