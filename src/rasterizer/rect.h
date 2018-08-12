/*
 * rect.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_RECT_H
#define PR_RECT_H


#include "types.h"


typedef struct pr_rect
{
    PRint left;
    PRint top;
    PRint right;
    PRint bottom;
}
pr_rect;


void _pr_rect_init(pr_rect* rect);


#endif
