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


typedef struct pr_state_machine
{
    pr_matrix4      modelViewMatrix;
    pr_matrix4      projectionMatrix;
    pr_viewport     viewport;
    pr_framebuffer* boundFramebuffer;
    PRboolean       originLeftTop;      //!< Specifies whether the screen origin is left-top (true) or left-bottom (false). By default true.
    PRubyte         colorIndex;         //!< Acitve color index.
}
pr_state_machine;


extern pr_state_machine _stateMachine;


void _pr_state_machine_init(pr_state_machine* stateMachine);

void _pr_state_machine_bind_framebuffer(pr_framebuffer* framebuffer);


#endif
