/*
 * pico.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "pico.h"
#include "error.h"
#include "context.h"
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

// --- context --- //

PRobject prGenContext(const pr_context_desc* desc, PRuint width, PRuint height)
{
    return (PRobject)_pr_context_create(desc, width, height);
}

void prDeleteContext(PRobject context)
{
    _pr_context_delete((pr_context*)context);
}

void prContextPresent(PRobject context, PRobject framebuffer)
{
    _pr_context_present((pr_context*)context, (pr_framebuffer*)framebuffer);
}

// --- framebuffer --- //

PRobject prGenFramebuffer(PRuint width, PRuint height)
{
    #if 1//!!!
    pr_framebuffer* fb = _pr_framebuffer_create(width, height);

    PRubyte c = 0;
    for (PRuint i = 0, n = width*height; i < n; ++i)
    {
        fb->pixels[i].colorIndex = c;
        ++c;
    }

    return (PRobject)fb;
    #else
    return (PRobject)_pr_framebuffer_create(width, height);
    #endif
}

void prDeleteFramebuffer(PRobject framebuffer)
{
    _pr_framebuffer_delete((pr_framebuffer*)framebuffer);
}

void prClearFramebuffer(PRobject framebuffer, PRubyte clearColor, float depth)
{
    //...
}

// --- vertexbuffer --- //

PRobject prGenVertexbuffer(PRsizei numVertices)
{
    return (PRobject)_pr_vertexbuffer_create(numVertices);
}

void prDeleteVertexbuffer(PRobject vertexbuffer)
{
    _pr_vertexbuffer_delete((pr_vertexbuffer*)vertexbuffer);
}

// --- indexbuffer --- //

PRobject prGenIndexbuffer(PRsizei numIndices)
{
    return (PRobject)_pr_indexbuffer_create(numIndices);
}

void prDeleteIndexbuffer(PRobject indexbuffer)
{
    _pr_indexbuffer_delete((pr_indexbuffer*)indexbuffer);
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
