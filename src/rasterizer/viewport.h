/*
 * viewport.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VIEWPORT_H__
#define __PR_VIEWPORT_H__


#include "types.h"


typedef struct pr_viewport
{
    PRint x;
    PRint y;
    PRint width;
    PRint height;
    PRfloat minDepth;
    PRfloat maxDepth;
}
pr_viewport;


void _pr_viewport_init(pr_viewport* viewport);


#endif
