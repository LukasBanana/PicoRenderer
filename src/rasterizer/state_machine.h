/*
 * state_machine.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_STATE_MACHINE_H__
#define __PR_STATE_MACHINE_H__


#include "matrix4.h"
#include "viewport.h"
#include "framebuffer.h"
#include "texture.h"


typedef struct pr_state_machine
{
    pr_matrix4      projectionMatrix;
    pr_matrix4      viewMatrix;
    pr_matrix4      worldMatrix;
    pr_matrix4      viewProjectionMatrix;
    pr_matrix4      worldViewProjectionMatrix;
    pr_viewport     viewport;
    
    pr_framebuffer* boundFrameBuffer;
    pr_texture*     boundTexture;

    PRubyte         colorIndex;         //!< Acitve color index.
}
pr_state_machine;


extern pr_state_machine _stateMachine;


void _pr_state_machine_init(pr_state_machine* stateMachine);

void _pr_state_machine_bind_framebuffer(pr_framebuffer* framebuffer);
void _pr_state_machine_bind_texture(pr_texture* texture);

void _pr_state_machine_projection_matrix(const pr_matrix4* matrix);
void _pr_state_machine_view_matrix(const pr_matrix4* matrix);
void _pr_state_machine_world_matrix(const pr_matrix4* matrix);


#endif
