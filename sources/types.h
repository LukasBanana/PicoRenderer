/*
 * types.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_TYPES_H__
#define __PR_TYPES_H__


#include <stddef.h>


#define PR_TRUE     1
#define PR_FALSE    0


//! Boolean type.
typedef char PRboolean;

//! 8-bit signed integer.
typedef char PRbyte;
//! 8-bit unsigned integer.
typedef unsigned char PRubyte;

//! 16-bit signed integer.
typedef short PRshort;
//! 16-bit unsigned integer.
typedef unsigned short PRushort;

//! 32-bit signed integer.
typedef int PRint;
//! 32-bit unsigned integer.
typedef unsigned int PRuint;

//! 32-bit floating-point.
typedef float PRfloat;
//! 64-bit floating-point.
typedef double PRdouble;

//! Enumeration type.
typedef unsigned int PRenum;
//! Flags type.
typedef unsigned int PRflags;
//! Size type.
typedef size_t PRsizei;


#endif
