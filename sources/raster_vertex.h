/*
 * raster_vertex.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_RASTER_VERTEX_H__
#define __PR_RASTER_VERTEX_H__


#include "vector2.h"


typedef struct pr_raster_vertex
{
    PRint   x; //!< Screen coordainte X.
    PRint   y; //!< Screen coordainte Y.
    PRfloat z; //!< Normalized device coordinate Z.
    PRfloat u; //!< Inverse texture coordinate U.
    PRfloat v; //!< Inverse texture coordinate V.
}
pr_raster_vertex;


#endif
