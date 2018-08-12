/*
 * enums.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_ENUMS_H
#define PR_ENUMS_H


// Color formats
#define PR_UBYTE_RGB        0x00000001
//#define PR_UBYTE_RGBA       0x00000002

// prGetString arguments
#define PR_STRING_VERSION   0x00000011
#define PR_STRING_RENDERER  0x00000012
#define PR_STRING_PLUGINS   0x00000013

// prGetIntegerv arguments
#define PR_MAX_TEXTURE_SIZE 0x00000021

// Geometry primitives
#define PR_POINTS           0x00000031
#define PR_LINES            0x00000032
#define PR_LINE_STRIP       0x00000033
#define PR_LINE_LOOP        0x00000034
#define PR_TRIANGLES        0x00000035
#define PR_TRIANGLE_STRIP   0x00000036
#define PR_TRIANGLE_FAN     0x00000037

// Cull modes
#define PR_CULL_NONE        0x00000040
#define PR_CULL_FRONT       0x00000041
#define PR_CULL_BACK        0x00000042

// Polygon modes
#define PR_POLYGON_FILL     0x00000053
#define PR_POLYGON_LINE     0x00000054
#define PR_POLYGON_POINT    0x00000055

// prGetTexLevelParameteri arguments
#define PR_TEXTURE_WIDTH    0x00000060
#define PR_TEXTURE_HEIGHT   0x00000061

// States
#define PR_SCISSOR          0
#define PR_MIP_MAPPING      1

// Texture environment parameters
#define PR_TEXTURE_LOD_BIAS 0

// Frame buffer clear flags
#define PR_COLOR_BUFFER_BIT 0x00000001
#define PR_DEPTH_BUFFER_BIT 0x00000002


#endif
