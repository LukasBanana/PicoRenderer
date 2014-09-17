/*
 * globa_state.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "global_state.h"
#include "static_config.h"


pr_global_state _globalState;

void _pr_global_state_init()
{
    _pr_texture_singular_init(&(_globalState.singularTexture));
}

void _pr_global_state_release()
{
    _pr_texture_singular_clear(&(_globalState.singularTexture));
}

