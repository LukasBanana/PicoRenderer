/*
 * state_machine.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "state_machine.h"
#include "static_config.h"
#include "error.h"
#include "ext_math.h"


static pr_state_machine _nullStateMachine;
pr_state_machine* _stateMachine = &_nullStateMachine;

static void _state_machine_cliprect(PRint left, PRint top, PRint right, PRint bottom)
{
    _stateMachine->clipRect.left    = left;
    _stateMachine->clipRect.right   = right;
    #ifdef PR_ORIGIN_LEFT_TOP
    if (PR_STATE_MACHINE.boundFrameBuffer != NULL)
    {
        PRint height = (PRint)(PR_STATE_MACHINE.boundFrameBuffer->height);
        _stateMachine->clipRect.top     = height - bottom - 1;
        _stateMachine->clipRect.bottom  = height - top - 1;
    }
    else
    {
        _stateMachine->clipRect.top     = top;
        _stateMachine->clipRect.bottom  = bottom;
    }
    #else
    _stateMachine->clipRect.top     = top;
    _stateMachine->clipRect.bottom  = bottom;
    #endif
}

void _pr_state_machine_init(pr_state_machine* stateMachine)
{
    _pr_matrix_load_identity(&(stateMachine->projectionMatrix));
    _pr_matrix_load_identity(&(stateMachine->viewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewProjectionMatrix));

    _pr_viewport_init(&(stateMachine->viewport));
    
    stateMachine->clipRect.left     = 0;
    stateMachine->clipRect.top      = 0;
    stateMachine->clipRect.right    = 0;
    stateMachine->clipRect.bottom   = 0;

    stateMachine->boundFrameBuffer  = NULL;
    stateMachine->boundVertexBuffer = NULL;
    stateMachine->boundIndexBuffer  = NULL;
    stateMachine->boundTexture      = NULL;

    stateMachine->colorIndex        = 0;
    stateMachine->cullMode          = PR_CULL_NONE;
}

void _pr_state_machine_init_null()
{
    _pr_state_machine_init(&_nullStateMachine);
}

void _pr_state_machine_makecurrent(pr_state_machine* stateMachine)
{
    if (stateMachine != NULL)
        _stateMachine = stateMachine;
    else
        _stateMachine = &_nullStateMachine;
}

void _pr_state_machine_bind_framebuffer(pr_framebuffer* frameBuffer)
{
    PR_STATE_MACHINE.boundFrameBuffer = frameBuffer;
    if (frameBuffer != NULL)
        _state_machine_cliprect(0, 0, (PRint)frameBuffer->width/* - 1*/, (PRint)frameBuffer->height - 1);
    else
        _state_machine_cliprect(0, 0, 0, 0);
}

void _pr_state_machine_bind_vertexbuffer(pr_vertexbuffer* vertexBuffer)
{
    PR_STATE_MACHINE.boundVertexBuffer = vertexBuffer;
}

void _pr_state_machine_bind_indexbuffer(pr_indexbuffer* indexBuffer)
{
    PR_STATE_MACHINE.boundIndexBuffer = indexBuffer;
}

void _pr_state_machine_bind_texture(pr_texture* texture)
{
    PR_STATE_MACHINE.boundTexture = texture;
}

void _pr_state_machine_viewport(PRuint x, PRuint y, PRuint width, PRuint height)
{
    if (PR_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }

    // Clamp clipping rectangle
    const PRint maxWidth = (PRint)(PR_STATE_MACHINE.boundFrameBuffer->width/* - 1*/);
    const PRint maxHeight = (PRint)(PR_STATE_MACHINE.boundFrameBuffer->height - 1);
    
    _state_machine_cliprect(
        PR_CLAMP((PRint)x, 0, maxWidth),
        PR_CLAMP((PRint)y, 0, maxHeight),
        PR_CLAMP((PRint)(x + width), 0, maxWidth),
        PR_CLAMP((PRint)(y + height), 0, maxHeight)
    );

    /*
    Store width and height with half size, to avoid this multiplication
    while transforming the normalized device coordinates (NDC) into viewspace.
    */
    #ifdef PR_ORIGIN_LEFT_TOP
    PR_STATE_MACHINE.viewport.x             = (PRfloat)x;
    PR_STATE_MACHINE.viewport.y             = (PRfloat)(PR_STATE_MACHINE.boundFrameBuffer->height - y - 1);
    PR_STATE_MACHINE.viewport.halfWidth     = 0.5f * (PRfloat)width;
    PR_STATE_MACHINE.viewport.halfHeight    = -0.5f * (PRfloat)height;
    #else
    PR_STATE_MACHINE.viewport.x             = (PRfloat)x;
    PR_STATE_MACHINE.viewport.y             = (PRfloat)y;
    PR_STATE_MACHINE.viewport.halfWidth     = 0.5f * (PRfloat)width;
    PR_STATE_MACHINE.viewport.halfHeight    = 0.5f * (PRfloat)height;
    #endif
}

void _pr_state_machine_depth_range(PRfloat minDepth, PRfloat maxDepth)
{
    PR_STATE_MACHINE.viewport.minDepth = minDepth;
    PR_STATE_MACHINE.viewport.maxDepth = maxDepth;
    PR_STATE_MACHINE.viewport.depthSize = maxDepth - minDepth;
}

void _pr_state_machine_cull_mode(PRenum mode)
{
    PR_STATE_MACHINE.cullMode = mode;
}

static void _update_viewprojection_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.viewProjectionMatrix),
        &(PR_STATE_MACHINE.projectionMatrix),
        &(PR_STATE_MACHINE.viewMatrix)
    );
}

static void _update_worldview_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.worldViewMatrix),
        &(PR_STATE_MACHINE.viewMatrix),
        &(PR_STATE_MACHINE.worldMatrix)
    );
}

static void _update_worldviewprojection_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.worldViewProjectionMatrix),
        &(PR_STATE_MACHINE.viewProjectionMatrix),
        &(PR_STATE_MACHINE.worldMatrix)
    );
}

void _pr_state_machine_projection_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.projectionMatrix), matrix);
    _update_viewprojection_matrix();
    _update_worldviewprojection_matrix();
}

void _pr_state_machine_view_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.viewMatrix), matrix);
    _update_viewprojection_matrix();
    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}

void _pr_state_machine_world_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.worldMatrix), matrix);
    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}

