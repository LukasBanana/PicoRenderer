/*
 * raster_vertex.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_RASTER_VERTEX_H__
#define __PR_RASTER_VERTEX_H__


#include "types.h"


//! Raster vertex structure before projection (for clipping)
typedef struct pr_clip_vertex
{
    PRfloat x; //!< X coordinate (in view space).
    PRfloat y; //!< Y coordinate (in view space).
    PRfloat z; //!< Z coordinate (in view space).
    PRfloat w; //!< W coordinate (in view space).
    PRfloat u; //!< Texture coordinate U.
    PRfloat v; //!< Texture coordinate V.
}
pr_clip_vertex;

//! Raster vertex structure after projection
typedef struct pr_raster_vertex
{
    PRint   x; //!< Screen coordinate X.
    PRint   y; //!< Screen coordinate Y.
    PRfloat z; //!< Normalized device coordinate Z.
    PRfloat u; //!< Inverse texture coordinate U.
    PRfloat v; //!< Inverse texture coordinate V.
}
pr_raster_vertex;


#endif
