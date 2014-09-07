/*
 * pico.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "pico.h"
#include "error.h"
#include "framebuffer.h"
#include "vertexbuffer.h"


PRenum prGetError()
{
    return _pr_get_error();
}

PRuint prGenFramebuffer(PRuint width, PRuint height)
{
    return (PRuint)_pr_gen_framebuffer(width, height);
}

void prDeleteFramebuffer(PRuint framebufferID)
{
    _pr_delete_framebuffer((pr_framebuffer*)framebufferID);
}

PRuint prGenVertexbuffer(PRsizei numVertices)
{
    return (PRuint)_pr_gen_vertexbuffer(numVertices);
}

void prDeleteVertexbuffer(PRuint vertexbufferID)
{
    _pr_delete_vertexbuffer((pr_vertexbuffer*)vertexbufferID);
}

void prDraw(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

void prDrawIndexed(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}
