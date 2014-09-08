/*
 * raster_triangle.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_RASTER_TRIANGLE_H__
#define __PR_RASTER_TRIANGLE_H__


#include "raster_vertex.h"


typedef struct pr_raster_triangle
{
    pr_raster_vertex a;
    pr_raster_vertex b;
    pr_raster_vertex c;
}
pr_raster_triangle;


#endif
