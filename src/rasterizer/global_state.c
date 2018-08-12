/*
 * globa_state.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "global_state.h"
#include "static_config.h"
#include "error.h"
#include "render.h"


pr_global_state _globalState;

#define _IMM_VERTICES   _globalState.immModeVertexBuffer.vertices
#define _IMM_CUR_VERTEX _IMM_VERTICES[_globalState.immModeVertCounter]


void _pr_global_state_init()
{
    _pr_texture_singular_init(&(_globalState.singularTexture));

    // Initialize immediate mode
    _pr_vertexbuffer_singular_init(&(_globalState.immModeVertexBuffer), PR_NUM_IMMEDIATE_VERTICES);
    _globalState.immModeActive      = PR_FALSE;
    _globalState.immModeVertCounter = 0;
    _globalState.immModePrimitives  = PR_POINTS;
}

void _pr_global_state_release()
{
    _pr_texture_singular_clear(&(_globalState.singularTexture));
    _pr_vertexbuffer_singular_clear(&(_globalState.immModeVertexBuffer));
}

static void _immediate_mode_flush()
{
    if (_globalState.immModeVertCounter == 0)
        return;

    // Draw current vertex buffer
    switch (_globalState.immModePrimitives)
    {
        case PR_POINTS:
            _pr_render_points(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;

        case PR_LINES:
            _pr_render_lines(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;
        case PR_LINE_STRIP:
            _pr_render_line_strip(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;
        case PR_LINE_LOOP:
            _pr_render_line_loop(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;

        case PR_TRIANGLES:
            _pr_render_triangles(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;
        case PR_TRIANGLE_STRIP:
            _pr_render_triangle_strip(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;
        case PR_TRIANGLE_FAN:
            _pr_render_triangle_fan(_globalState.immModeVertCounter, 0, &(_globalState.immModeVertexBuffer));
            break;

        default:
            PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
            break;
    }

    // Reset vertex counter
    _globalState.immModeVertCounter = 0;
}

void _pr_immediate_mode_begin(PRenum primitives)
{
    if (_globalState.immModeActive)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }
    if (primitives < PR_POINTS || primitives > PR_TRIANGLE_FAN)
    {
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
        return;
    }

    // Store primitive type and reset vertex counter
    _globalState.immModeActive      = PR_TRUE;
    _globalState.immModePrimitives  = primitives;
    _globalState.immModeVertCounter = 0;
}

void _pr_immediate_mode_end()
{
    if (!_globalState.immModeActive)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }

    // Draw vertex buffer with current previously selected primitive
    _immediate_mode_flush();

    _globalState.immModeActive = PR_FALSE;
}

void _pr_immediate_mode_texcoord(PRfloat u, PRfloat v)
{
    // Store texture coordinate for current vertex
    _IMM_CUR_VERTEX.texCoord.x = u;
    _IMM_CUR_VERTEX.texCoord.y = v;
}

void _pr_immediate_mode_vertex(PRfloat x, PRfloat y, PRfloat z, PRfloat w)
{
    // Store vertex coordinate for current vertex
    _IMM_CUR_VERTEX.coord.x = x;
    _IMM_CUR_VERTEX.coord.y = y;
    _IMM_CUR_VERTEX.coord.z = z;
    _IMM_CUR_VERTEX.coord.w = w;

    // Count to next vertex
    ++_globalState.immModeVertCounter;

    // Check if limit is exceeded
    if (_globalState.immModeVertCounter >= PR_NUM_IMMEDIATE_VERTICES)
        _immediate_mode_flush();

    switch (_globalState.immModePrimitives)
    {
        case PR_TRIANGLES:
            if ( _globalState.immModeVertCounter + 3 >= PR_NUM_IMMEDIATE_VERTICES &&
                 _globalState.immModeVertCounter % 3 == 0 )
            {
                _immediate_mode_flush();
            }
            break;
    }
}

