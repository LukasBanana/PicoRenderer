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

#define PR_INLINE   _inline


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
//! Bit field type used for flags.
typedef unsigned int PRbitfield;
//! Size type.
typedef int PRsizei;

//! Texture size type: 8-bit unsigned byte.
typedef PRshort PRtexsize;

//! No type or used for void pointer.
typedef void PRvoid;

//! Object type.
typedef void* PRobject;


#endif
