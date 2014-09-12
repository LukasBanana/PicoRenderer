/*
 * vertex.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "vertex.h"

#include <string.h>


void _pr_vertex_init(pr_vertex* vertex)
{
    if (vertex != NULL)
        memset(vertex, 0, sizeof(pr_vertex));
}
