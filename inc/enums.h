/*
 * enums.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_ENUMS_H__
#define __PR_ENUMS_H__


// Color formats
#define PR_UBYTE_RGB        1
//#define PR_UBYTE_RGBA       2

// prGetString arguments
#define PR_STRING_VERSION   1
#define PR_STRING_RENDERER  2
#define PR_STRING_PLUGINS   3

// Geometry primitives
#define PR_POINTS           1
#define PR_LINES            2
#define PR_LINE_STRIP       3
#define PR_LINE_LOOP        4
#define PR_TRIANGLES        5
#define PR_TRIANGLE_STRIP   6
#define PR_TRIANGLE_FAN     7

// Cull modes
#define PR_CULL_NONE        0
#define PR_CULL_FRONT       1
#define PR_CULL_BACK        2


#endif
