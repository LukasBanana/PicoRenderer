/*
 * indexbuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "indexbuffer.h"

#include <stdlib.h>


pr_indexbuffer* _pr_indexbuffer_create(PRsizei numIndices)
{
    pr_indexbuffer* indexbuffer = (pr_indexbuffer*)malloc(sizeof(pr_indexbuffer));
    
    indexbuffer->numIndices = numIndices;
    indexbuffer->indices = (PRushort*)calloc(numIndices, sizeof(PRushort));

    return indexbuffer;
}

void _pr_indexbuffer_delete(pr_indexbuffer* indexbuffer)
{
    if (indexbuffer != NULL && indexbuffer->indices != NULL)
    {
        free(indexbuffer->indices);
        free(indexbuffer);
    }
}
