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
#include "indexbuffer.h"
#include "state_machine.h"


// --- common --- //

PRenum prGetError()
{
    return _pr_error_get();
}

PRboolean prInit()
{
    _pr_state_machine_init(&_stateMachine);
    return PR_TRUE;
}

PRboolean prRelease()
{
    //...
    return PR_TRUE;
}

// --- framebuffer --- //

PRuint prGenFramebuffer(PRuint width, PRuint height)
{
    return (PRuint)_pr_framebuffer_create(width, height);
}

void prDeleteFramebuffer(PRuint framebufferID)
{
    _pr_framebuffer_delete((pr_framebuffer*)framebufferID);
}

// --- vertexbuffer --- //

PRuint prGenVertexbuffer(PRsizei numVertices)
{
    return (PRuint)_pr_vertexbuffer_create(numVertices);
}

void prDeleteVertexbuffer(PRuint vertexbufferID)
{
    _pr_vertexbuffer_delete((pr_vertexbuffer*)vertexbufferID);
}

// --- indexbuffer --- //

PRuint prGenIndexbuffer(PRsizei numIndices)
{
    return (PRuint)_pr_indexbuffer_create(numIndices);
}

void prDeleteIndexbuffer(PRuint indexbufferID)
{
    _pr_indexbuffer_delete((pr_indexbuffer*)indexbufferID);
}

// --- drawing --- //

void prDraw(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

void prDrawIndexed(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}
