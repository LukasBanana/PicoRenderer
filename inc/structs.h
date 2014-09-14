/*
 * structs.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_STRUCTS_H__
#define __PR_STRUCTS_H__


#include "types.h"


//! Vertex structure. Coordinates: X, Y, Z; Texture-coordinates: U, V.
typedef struct PRvertex
{
    PRfloat x, y, z, u, v;
}
PRvertex;


#endif
