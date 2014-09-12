/*
 * platform.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PLATFORM_H__
#define __PR_PLATFORM_H__


#if defined(WIN32)
#   include <Windows.h>
#   define PR_PLATFORM_WIN32
#elif defined(unix)
#   define PR_PLATFORM_POSIX
#else
#   error OS not supported yet
#endif


#if defined(PR_PLATFORM_WIN32)

//! Win32 render context description structure.
typedef struct pr_context_desc
{
    HDC dc;
}
pr_context_desc;

#elif defined(PR_PLATFORM_POSIX)

//! Posix render context description structure.
typedef struct pr_context_desc
{
    int unused;
}
pr_context_desc;

#endif


#endif
