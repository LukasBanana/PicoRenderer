/*
 * enums.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_ENUMS_H__
#define __PR_ENUMS_H__


#define PR_IMAGE_FORMAT_UBYTE_RGB       1
//#define PR_IMAGE_FORMAT_UBYTE_RGBA      2

#define PR_DEFAULT_RESOLUTION_WIDTH     640
#define PR_DEFAULT_RESOLUTION_HEIGHT    480

#define PR_STRING_VERSION               1
#define PR_STRING_RENDERER              2
#define PR_STRING_PLUGINS               3

#define PR_PRIMITIVE_POINTS             0x00000001
#define PR_PRIMITIVE_LINES              0x00010001
#define PR_PRIMITIVE_LINE_STRIP         0x00010002
#define PR_PRIMITIVE_LINE_LOOP          0x00010003
#define PR_PRIMITIVE_TRIANGLES          0x00020001
#define PR_PRIMITIVE_TRIANGLE_STRIP     0x00020002
#define PR_PRIMITIVE_TRIANGLE_FAN       0x00020003


#endif
