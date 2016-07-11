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
#ifdef _WIN32
#   define PR_BGR_COLOR_OUTPUT
#endif

//! Includes the STB image file handler plugin
#define PR_INCLUDE_PLUGINS

//! Flips the screen space vertical
#define PR_ORIGIN_LEFT_TOP

//! Makes excessive use of the approximated math functions
#define PR_FAST_MATH

//! Enables extended debug information
#define PR_DEBUG

//! Use perspective corrected depth and texture coordinates
#define PR_PERSPECTIVE_CORRECTED

//! Use an 8-bit depth buffer (instead of 16 bit)
//#define PR_DEPTH_BUFFER_8BIT

//! Use a 64-bit interpolation type instead of 32-bit
#define PR_INTERP_64BIT

//! Use a 24-bit color buffer (instead of 8 bit)
//#define PR_COLOR_BUFFER_24BIT

//! Merge color- and depth buffers to a single one inside a frame buffer.
#define PR_MERGE_COLOR_AND_DEPTH_BUFFERS //!CAN NOT BE DISABLED YET!


#ifdef PR_INTERP_64BIT
//! 64-bit interpolation type.
typedef double PRinterp;
#define PR_FLOAT(x) x
#else
//! 32-bit interpolation type.
typedef float PRinterp;
#define PR_FLOAT(x) x##f
#endif


#endif
