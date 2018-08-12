/*
 * indexbuffer.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_INDEXBUFFER_H
#define PR_INDEXBUFFER_H


#include "types.h"

#include <stdio.h>


typedef struct pr_indexbuffer
{
    PRushort    numIndices;
    PRushort*   indices;
}
pr_indexbuffer;


pr_indexbuffer* _pr_indexbuffer_create();
void _pr_indexbuffer_delete(pr_indexbuffer* indexBuffer);

void _pr_indexbuffer_data(pr_indexbuffer* indexBuffer, const PRushort* indices, PRushort numIndices);
void _pr_indexbuffer_data_from_file(pr_indexbuffer* indexBuffer, PRsizei* numIndices, FILE* file);


#endif
