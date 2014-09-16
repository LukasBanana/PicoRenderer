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
#include "rect.h"
#include "framebuffer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture.h"


#define PR_STATE_MACHINE (*_stateMachine)


typedef struct pr_state_machine
{
    pr_matrix4          projectionMatrix;
    pr_matrix4          viewMatrix;
    pr_matrix4          worldMatrix;
    pr_matrix4          viewProjectionMatrix;
    pr_matrix4          worldViewMatrix;
    pr_matrix4          worldViewProjectionMatrix;

    pr_viewport         viewport;
    pr_rect             clipRect;
    
    pr_framebuffer*     boundFrameBuffer;
    pr_vertexbuffer*    boundVertexBuffer;
    pr_indexbuffer*     boundIndexBuffer;
    pr_texture*         boundTexture;

    PRubyte             colorIndex;                 //!< Acitve color index.
    PRenum              cullMode;
}
pr_state_machine;


//! Reference to the state machine of the current context.
extern pr_state_machine* _stateMachine;


void _pr_state_machine_init(pr_state_machine* stateMachine);
void _pr_state_machine_init_null();

void _pr_state_machine_makecurrent(pr_state_machine* stateMachine);

void _pr_state_machine_bind_framebuffer(pr_framebuffer* frameBuffer);
void _pr_state_machine_bind_vertexbuffer(pr_vertexbuffer* vertexBuffer);
void _pr_state_machine_bind_indexbuffer(pr_indexbuffer* indexBuffer);
void _pr_state_machine_bind_texture(pr_texture* texture);

void _pr_state_machine_viewport(PRuint x, PRuint y, PRuint width, PRuint height);
void _pr_state_machine_depth_range(PRfloat minDepth, PRfloat maxDepth);
void _pr_state_machine_cull_mode(PRenum mode);

void _pr_state_machine_projection_matrix(const pr_matrix4* matrix);
void _pr_state_machine_view_matrix(const pr_matrix4* matrix);
void _pr_state_machine_world_matrix(const pr_matrix4* matrix);


#endif
