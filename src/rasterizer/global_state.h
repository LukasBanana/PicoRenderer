/*
 * globa_state.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_GLOBAL_STATE_H__
#define __PR_GLOBAL_STATE_H__


#include "texture.h"


#define PR_SINGULAR_TEXTURE _globalState.singularTexture


typedef struct pr_global_state
{
    pr_texture singularTexture; // Texture with single color
}
pr_global_state;


//! Global render engine state.
extern pr_global_state _globalState;


void _pr_global_state_init();
void _pr_global_state_release();


#endif
