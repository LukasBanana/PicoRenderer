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
#define PR_POINTS           0x00000001
#define PR_LINES            0x00000002
#define PR_LINE_STRIP       0x00000003
#define PR_LINE_LOOP        0x00000004
#define PR_TRIANGLES        0x00000005
#define PR_TRIANGLE_STRIP   0x00000006
#define PR_TRIANGLE_FAN     0x00000007

// Cull modes
#define PR_CULL_NONE        0x00000010
#define PR_CULL_FRONT       0x00000011
#define PR_CULL_BACK        0x00000012

// Polygon modes
#define PR_POLYGON_FILL     0x00000013
#define PR_POLYGON_LINE     0x00000014
#define PR_POLYGON_POINT    0x00000015

// States
#define PR_SCISSOR          0


#endif
