/*
 * viewport.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VIEWPORT_H__
#define __PR_VIEWPORT_H__


#include "types.h"


//! The viewport has only floats, becuase they will be
//! used to transform the raster_vertex coordinates.
typedef struct pr_viewport
{
    PRfloat x;
    PRfloat y;
    PRfloat width;
    PRfloat height;
    PRfloat minDepth;
    PRfloat maxDepth;
}
pr_viewport;


void _pr_viewport_init(pr_viewport* viewport);


#endif
