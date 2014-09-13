/*
 * static_config.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_STATIC_CONFIG_H__
#define __PR_STATIC_CONFIG_H__


// Version number
#define PR_VERSION_MAJOR    0
#define PR_VERSION_MINOR    1
#define PR_VERSION_REVISION 0
#define PR_VERSION_STATUS   "alpha"
#define PR_VERSION_STR      "0.1 alpha"

//! Use BGR color output instead of RGB (used for Win32 GDI)
#define PR_BGR_COLOR_OUTPUT

//! Includes the STB image file handler plugin
#define PR_INCLUDE_PLUGINS

//! Flips the screen space vertical
#define PR_ORIGIN_LEFT_TOP

//! Makes excessive use of the approximated math functions
#define PR_FAST_MATH


#endif
