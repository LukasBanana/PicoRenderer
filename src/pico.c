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
#include "render.h"


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

void prContextPresent(PRobject context)
{
    _pr_context_present((pr_context*)context, _stateMachine.boundFramebuffer);
}

// --- framebuffer --- //

PRobject prGenFramebuffer(PRuint width, PRuint height)
{
    return (PRobject)_pr_framebuffer_create(width, height);
}

void prDeleteFramebuffer(PRobject framebuffer)
{
    _pr_framebuffer_delete((pr_framebuffer*)framebuffer);
}

void prBindFramebuffer(PRobject framebuffer)
{
    _pr_state_machine_bind_framebuffer((pr_framebuffer*)framebuffer);
}

void prClearFramebuffer(PRubyte clearColor, float depth)
{
    _pr_framebuffer_clear(_stateMachine.boundFramebuffer, clearColor, depth);
}

PRubyte prGetColorIndex(PRubyte red, PRubyte green, PRubyte blue)
{
    return _pr_color_to_colorindex_r3g3b2(red, green, blue);
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

// --- matrices --- //

void prProjectionMatrix(const PRfloat* matrix4x4)
{
    memcpy(&(_stateMachine.projectionMatrix), matrix4x4, sizeof(pr_matrix4));
}

void prModelViewMatrix(const PRfloat* matrix4x4)
{
    memcpy(&(_stateMachine.modelViewMatrix), matrix4x4, sizeof(pr_matrix4));
}

void prBuildPerspectiveProjection(PRfloat* matrix4x4, PRfloat aspectRatio, PRfloat nearPlane, PRfloat farPlane, PRfloat fov)
{
}

void prBuildOrthogonalProjection(PRfloat* matrix4x4, PRfloat width, PRfloat height, PRfloat nearPlane, PRfloat farPlane)
{
}

void prTranslate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z)
{
}

void prRotate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z, PRfloat angle)
{
}

void prScale(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z)
{
}

void prLoadIdentity(PRfloat* matrix4x4)
{
    _pr_matrix_load_identity((pr_matrix4*)matrix4x4);
}

// --- drawing --- //

void prColor(PRubyte colorIndex)
{
    _stateMachine.colorIndex = colorIndex;
}

void prDrawScreenPoint(PRint x, PRint y, PRubyte colorIndex)
{
    _pr_render_screenspace_point(x, y, colorIndex);
}

void prDrawScreenLine(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex)
{
    _pr_render_screenspace_line(x1, y1, x2, y2, colorIndex);
}

void prDraw(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

void prDrawIndexed(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

