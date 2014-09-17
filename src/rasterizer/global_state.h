/*
 * globa_state.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_GLOBAL_STATE_H__
#define __PR_GLOBAL_STATE_H__


#include "texture.h"
#include "vertexbuffer.h"


#define PR_SINGULAR_TEXTURE         _globalState.singularTexture
#define PR_SINGULAR_VERTEXBUFFER    _globalState.singularVertexBuffer

// Number of vertices for the vertex buffer of the immediate draw mode (prBegin/prEnd)
#define PR_NUM_IMMEDIATE_VERTICES   32


typedef struct pr_global_state
{
    pr_texture      singularTexture;        // Texture with single color

    // Immediate mode
    pr_vertexbuffer immModeVertexBuffer;
    PRboolean       immModeActive;
    PRsizei         immModeVertCounter;
    PRenum          immModePrimitives;
}
pr_global_state;


//! Global render engine state.
extern pr_global_state _globalState;


void _pr_global_state_init();
void _pr_global_state_release();

void _pr_immediate_mode_begin(PRenum primitives);
void _pr_immediate_mode_end();

void _pr_immediate_mode_texcoord(PRfloat u, PRfloat v);
void _pr_immediate_mode_vertex(PRfloat x, PRfloat y, PRfloat z, PRfloat w);


#endif
