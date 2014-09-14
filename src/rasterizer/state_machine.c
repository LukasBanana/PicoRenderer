/*
 * state_machine.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "state_machine.h"
#include "static_config.h"
#include "error.h"


static pr_state_machine _nullStateMachine;
pr_state_machine* _stateMachine = &_nullStateMachine;

void _pr_state_machine_init(pr_state_machine* stateMachine)
{
    _pr_matrix_load_identity(&(stateMachine->projectionMatrix));
    _pr_matrix_load_identity(&(stateMachine->viewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewProjectionMatrix));

    _pr_viewport_init(&(stateMachine->viewport));
    
    stateMachine->boundFrameBuffer  = NULL;
    stateMachine->boundVertexBuffer = NULL;
    stateMachine->boundIndexBuffer  = NULL;
    stateMachine->boundTexture      = NULL;
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

static void _update_viewprojection_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.viewProjectionMatrix),
        &(PR_STATE_MACHINE.projectionMatrix),
        &(PR_STATE_MACHINE.viewMatrix)
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
    _update_worldviewprojection_matrix();
}

void _pr_state_machine_world_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.worldMatrix), matrix);
    _update_worldviewprojection_matrix();
}

