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
    _pr_matrix_load_identity(&(stateMachine->modelViewMatrix));
    _pr_matrix_load_identity(&(stateMachine->projectionMatrix));
    _pr_viewport_init(&(stateMachine->viewport));
    
    stateMachine->boundFramebuffer  = NULL;
    stateMachine->boundTexture      = NULL;
}

void _pr_state_machine_bind_framebuffer(pr_framebuffer* framebuffer)
{
    _stateMachine.boundFramebuffer = framebuffer;
}

void _pr_state_machine_bind_texture(pr_texture* texture)
{
    _stateMachine.boundTexture = texture;
}

