/*
 * rect.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_RECT_H__
#define __PR_RECT_H__


#include "types.h"


typedef struct pr_rect
{
    PRint left;
    PRint top;
    PRint right;
    PRint bottom;
}
pr_rect;


#endif
