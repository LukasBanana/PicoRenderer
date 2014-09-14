/*
 * indexbuffer.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "indexbuffer.h"
#include "helper.h"
#include "error.h"

#include <stdlib.h>


pr_indexbuffer* _pr_indexbuffer_create()
{
    pr_indexbuffer* indexBuffer = PR_MALLOC(pr_indexbuffer);
    
    indexBuffer->numIndices = 0;
    indexBuffer->indices    = NULL;

    return indexBuffer;
}

void _pr_indexbuffer_delete(pr_indexbuffer* indexBuffer)
{
    if (indexBuffer != NULL)
    {
        PR_FREE(indexBuffer->indices);
        PR_FREE(indexBuffer);
    }
}

static void _indexbuffer_resize(pr_indexbuffer* indexBuffer, PRushort numIndices)
{
    // Check if index buffer must be reallocated
    if (indexBuffer->indices == NULL || indexBuffer->numIndices != numIndices)
    {
        // Create new index buffer data
        PR_FREE(indexBuffer->indices);

        indexBuffer->numIndices = numIndices;
        indexBuffer->indices    = PR_CALLOC(PRushort, numIndices);
    }
}

void _pr_indexbuffer_data(pr_indexbuffer* indexBuffer, const PRushort* indices, PRushort numIndices)
{
    if (indexBuffer == NULL || indices == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }

    _indexbuffer_resize(indexBuffer, numIndices);

    // Fill index buffer
    while (numIndices-- > 0)
        indexBuffer->indices[numIndices] = indices[numIndices];
}

void _pr_indexbuffer_data_from_file(pr_indexbuffer* indexBuffer, PRushort* numIndices, FILE* file)
{
    if (indexBuffer == NULL || numIndices == NULL || file == NULL)
    {
        PR_ERROR(PR_ERROR_NULL_POINTER);
        return;
    }
    
    // Read number of indices
    fread(numIndices, sizeof(PRushort), 1, file);

    _indexbuffer_resize(indexBuffer, *numIndices);

    // Read all indices
    fread(indexBuffer->indices, sizeof(PRushort), *numIndices, file);
}

