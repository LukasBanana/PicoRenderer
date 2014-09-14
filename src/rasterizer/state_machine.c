/*
 * state_machine.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "state_machine.h"


pr_state_machine _stateMachine;

void _pr_state_machine_init(pr_state_machine* stateMachine)
{
    _pr_matrix_load_identity(&(stateMachine->projectionMatrix));
    _pr_matrix_load_identity(&(stateMachine->viewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewProjectionMatrix));
    _pr_viewport_init(&(stateMachine->viewport));
    
    stateMachine->boundFrameBuffer  = NULL;
    stateMachine->boundTexture      = NULL;
}

void _pr_state_machine_bind_framebuffer(pr_framebuffer* framebuffer)
{
    _stateMachine.boundFrameBuffer = framebuffer;
}

void _pr_state_machine_bind_texture(pr_texture* texture)
{
    _stateMachine.boundTexture = texture;
}

void _pr_state_machine_projection_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(_stateMachine.projectionMatrix), matrix);
}

void _pr_state_machine_view_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(_stateMachine.viewMatrix), matrix);
    _pr_matrix_mul_matrix(
        &(_stateMachine.viewProjectionMatrix),
        &(_stateMachine.projectionMatrix),
        &(_stateMachine.viewMatrix)
    );
}

void _pr_state_machine_world_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(_stateMachine.worldMatrix), matrix);
    _pr_matrix_mul_matrix(
        &(_stateMachine.worldViewProjectionMatrix),
        &(_stateMachine.viewProjectionMatrix),
        &(_stateMachine.worldMatrix)
    );
}

