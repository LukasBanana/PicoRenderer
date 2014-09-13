/*
 * indexbuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "indexbuffer.h"
#include "helper.h"

#include <stdlib.h>


pr_indexbuffer* _pr_indexbuffer_create(PRsizei numIndices)
{
    pr_indexbuffer* indexbuffer = PR_MALLOC(pr_indexbuffer);
    
    indexbuffer->numIndices = numIndices;
    indexbuffer->indices = PR_CALLOC(PRushort, numIndices);

    return indexbuffer;
}

void _pr_indexbuffer_delete(pr_indexbuffer* indexbuffer)
{
    if (indexbuffer != NULL)
    {
        PR_FREE(indexbuffer->indices);
        PR_FREE(indexbuffer);
    }
}
