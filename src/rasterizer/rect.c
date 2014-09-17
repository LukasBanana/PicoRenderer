/*
 * rect.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "rect.h"


void _pr_rect_init(pr_rect* rect)
{
    if (rect != NULL)
    {
        rect->left      = 0;
        rect->top       = 0;
        rect->right     = 0;
        rect->bottom    = 0;
    }
}

