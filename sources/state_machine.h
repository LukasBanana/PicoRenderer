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


typedef struct pr_state_machine
{
    pr_matrix4 modelViewMatrix;
    pr_matrix4 projectionMatrix;
    pr_viewport viewport;
}
pr_state_machine;


extern pr_state_machine _stateMachine;


void _pr_state_machine_init(pr_state_machine* stateMachine);


#endif
