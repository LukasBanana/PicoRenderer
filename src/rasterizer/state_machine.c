/*
 * state_machine.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "state_machine.h"
#include "static_config.h"
#include "error.h"
#include "ext_math.h"


static pr_state_machine _nullStateMachine;
pr_state_machine* _stateMachine = &_nullStateMachine;


static void _state_machine_cliprect(PRint left, PRint top, PRint right, PRint bottom)
{
    _stateMachine->clipRect.left    = left;
    _stateMachine->clipRect.right   = right;

    #ifdef PR_ORIGIN_LEFT_TOP

    if (PR_STATE_MACHINE.boundFrameBuffer != NULL)
    {
        PRint height = (PRint)(PR_STATE_MACHINE.boundFrameBuffer->height);
        _stateMachine->clipRect.top     = height - bottom - 1;
        _stateMachine->clipRect.bottom  = height - top - 1;
    }
    else
    {
        _stateMachine->clipRect.top     = top;
        _stateMachine->clipRect.bottom  = bottom;
    }
    
    #else

    _stateMachine->clipRect.top     = top;
    _stateMachine->clipRect.bottom  = bottom;

    #endif
}

static void _update_cliprect()
{
    // Get dimensions from viewport
    PRint left      = PR_STATE_MACHINE.viewportRect.left;
    PRint top       = PR_STATE_MACHINE.viewportRect.top;
    PRint right     = PR_STATE_MACHINE.viewportRect.right;
    PRint bottom    = PR_STATE_MACHINE.viewportRect.bottom;

    if (PR_STATE_MACHINE.states[PR_SCISSOR] != PR_FALSE)
    {
        // Get dimensions from scissor
        PR_CLAMP_LARGEST(left, PR_STATE_MACHINE.scissorRect.left);
        PR_CLAMP_LARGEST(top, PR_STATE_MACHINE.scissorRect.top);
        PR_CLAMP_SMALLEST(right, PR_STATE_MACHINE.scissorRect.right);
        PR_CLAMP_SMALLEST(bottom, PR_STATE_MACHINE.scissorRect.bottom);
    }

    // Clamp clipping rectangle
    const PRint maxWidth = PR_STATE_MACHINE.boundFrameBuffer->width - 1;
    const PRint maxHeight = PR_STATE_MACHINE.boundFrameBuffer->height - 1;
    
    _state_machine_cliprect(
        PR_CLAMP(left, 0, maxWidth),
        PR_CLAMP(top, 0, maxHeight),
        PR_CLAMP(right, 0, maxWidth),
        PR_CLAMP(bottom, 0, maxHeight)
    );
}

void _pr_ref_add(PRobject obj)
{
    if (obj != NULL)
        ++_stateMachine->refCounter;
}

void _pr_ref_release(PRobject obj)
{
    if (obj != NULL)
    {
        if (_stateMachine->refCounter == 0)
            _pr_error_set(PR_ERROR_INVALID_STATE, "object ref-counter underflow");
        else
            --_stateMachine->refCounter;
    }
}

void _pr_ref_assert(pr_state_machine* stateMachine)
{
    if (stateMachine != NULL && stateMachine->refCounter != 0)
    {
        char msg[64];
        sprintf(msg, "object ref-counter is none zero ( %i )", stateMachine->refCounter);
        _pr_error_set(PR_ERROR_INVALID_STATE, msg);
    }
}

void _pr_state_machine_init(pr_state_machine* stateMachine)
{
    _pr_matrix_load_identity(&(stateMachine->projectionMatrix));
    _pr_matrix_load_identity(&(stateMachine->viewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewMatrix));
    _pr_matrix_load_identity(&(stateMachine->worldViewProjectionMatrix));

    _pr_viewport_init(&(stateMachine->viewport));

    _pr_rect_init(&(stateMachine-> viewportRect));
    _pr_rect_init(&(stateMachine->scissorRect));
    _pr_rect_init(&(stateMachine->clipRect));

    stateMachine->boundFrameBuffer      = NULL;
    stateMachine->boundVertexBuffer     = NULL;
    stateMachine->boundIndexBuffer      = NULL;
    stateMachine->boundTexture          = NULL;

    stateMachine->colorIndex            = 0;
    stateMachine->textureLodBias        = 0;
    stateMachine->cullMode              = PR_CULL_NONE;
    stateMachine->polygonMode           = PR_POLYGON_FILL;

    stateMachine->states[PR_SCISSOR]    = PR_FALSE;

    stateMachine->refCounter            = 0;
}

void _pr_state_machine_init_null()
{
    _pr_state_machine_init(&_nullStateMachine);
}

void _pr_state_machine_makecurrent(pr_state_machine* stateMachine)
{
    if (stateMachine != NULL)
        _stateMachine = stateMachine;
    else
        _stateMachine = &_nullStateMachine;
}

void _pr_state_machine_set_state(PRenum cap, PRboolean state)
{
    if (cap >= PR_NUM_STATES)
    {
        PR_ERROR(PR_ERROR_INDEX_OUT_OF_BOUNDS);
        return;
    }

    // Store new state
    PR_STATE_MACHINE.states[cap] = state;

    // Check for special cases (update functions)
    switch (cap)
    {
        case PR_SCISSOR:
            _update_cliprect();
            break;
    }
}

PRboolean _pr_state_machine_get_state(PRenum cap)
{
    if (cap >= PR_NUM_STATES)
    {
        PR_ERROR(PR_ERROR_INDEX_OUT_OF_BOUNDS);
        return PR_FALSE;
    }
    return PR_STATE_MACHINE.states[cap];
}

void _pr_state_machine_set_texenvi(PRenum param, PRint value)
{
    switch (param)
    {
        case PR_TEXTURE_LOD_BIAS:
            PR_STATE_MACHINE.textureLodBias = (PRubyte)PR_CLAMP(value, 0, 255);
            break;
        default:
            PR_ERROR(PR_ERROR_INDEX_OUT_OF_BOUNDS);
            break;
    }
}

PRint _pr_state_machine_get_texenvi(PRenum param)
{
    switch (param)
    {
        case PR_TEXTURE_LOD_BIAS:
            return (PRint)PR_STATE_MACHINE.textureLodBias;
        default:
            PR_ERROR(PR_ERROR_INDEX_OUT_OF_BOUNDS);
            return 0;
    }
}

void _pr_state_machine_bind_framebuffer(pr_framebuffer* frameBuffer)
{
    PR_STATE_MACHINE.boundFrameBuffer = frameBuffer;
    if (frameBuffer != NULL)
        _state_machine_cliprect(0, 0, (PRint)frameBuffer->width - 1, (PRint)frameBuffer->height - 1);
    else
        _state_machine_cliprect(0, 0, 0, 0);
}

void _pr_state_machine_bind_vertexbuffer(pr_vertexbuffer* vertexBuffer)
{
    PR_STATE_MACHINE.boundVertexBuffer = vertexBuffer;
}

void _pr_state_machine_bind_indexbuffer(pr_indexbuffer* indexBuffer)
{
    PR_STATE_MACHINE.boundIndexBuffer = indexBuffer;
}

void _pr_state_machine_bind_texture(pr_texture* texture)
{
    PR_STATE_MACHINE.boundTexture = texture;
}

void _pr_state_machine_viewport(PRint x, PRint y, PRint width, PRint height)
{
    if (PR_STATE_MACHINE.boundFrameBuffer == NULL)
    {
        PR_ERROR(PR_ERROR_INVALID_STATE);
        return;
    }

    /*
    Store width and height with half size, to avoid this multiplication
    while transforming the normalized device coordinates (NDC) into viewspace.
    */
    PR_STATE_MACHINE.viewport.x = (PRfloat)x;

    #ifdef PR_ORIGIN_LEFT_TOP
    PR_STATE_MACHINE.viewport.y = (PRfloat)(PR_STATE_MACHINE.boundFrameBuffer->height - 1 - y);
    #else
    PR_STATE_MACHINE.viewport.y = (PRfloat)y;
    #endif

    PR_STATE_MACHINE.viewport.halfWidth = 0.5f * (PRfloat)width;
    PR_STATE_MACHINE.viewport.halfHeight = -0.5f * (PRfloat)height;

    // Store viewport rectangle
    PR_STATE_MACHINE.viewportRect.left      = x;
    PR_STATE_MACHINE.viewportRect.top       = y;
    PR_STATE_MACHINE.viewportRect.right     = x + width;
    PR_STATE_MACHINE.viewportRect.bottom    = y + height;

    // Update clipping rectangle
    _update_cliprect();
}

void _pr_state_machine_depth_range(PRfloat minDepth, PRfloat maxDepth)
{
    PR_STATE_MACHINE.viewport.minDepth = minDepth;
    PR_STATE_MACHINE.viewport.maxDepth = maxDepth;
    PR_STATE_MACHINE.viewport.depthSize = maxDepth - minDepth;
}

void _pr_state_machine_scissor(PRint x, PRint y, PRint width, PRint height)
{
    // Store scissor rectangle
    PR_STATE_MACHINE.scissorRect.left   = x;
    PR_STATE_MACHINE.scissorRect.top    = y;
    PR_STATE_MACHINE.scissorRect.right  = x + width;
    PR_STATE_MACHINE.scissorRect.bottom = y + height;

    if (PR_STATE_MACHINE.states[PR_SCISSOR] != PR_FALSE)
    {
        // Update clipping rectangle
        _update_cliprect();
    }
}

void _pr_state_machine_cull_mode(PRenum mode)
{
    if (mode < PR_CULL_NONE || mode > PR_CULL_BACK)
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
    else
        PR_STATE_MACHINE.cullMode = mode;
}

void _pr_state_machine_polygon_mode(PRenum mode)
{
    if (mode < PR_POLYGON_FILL || mode > PR_POLYGON_POINT)
        PR_ERROR(PR_ERROR_INVALID_ARGUMENT);
    else
        PR_STATE_MACHINE.polygonMode = mode;
}

static void _update_viewprojection_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.viewProjectionMatrix),
        &(PR_STATE_MACHINE.projectionMatrix),
        &(PR_STATE_MACHINE.viewMatrix)
    );
}

static void _update_worldview_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.worldViewMatrix),
        &(PR_STATE_MACHINE.viewMatrix),
        &(PR_STATE_MACHINE.worldMatrix)
    );
}

static void _update_worldviewprojection_matrix()
{
    _pr_matrix_mul_matrix(
        &(PR_STATE_MACHINE.worldViewProjectionMatrix),
        &(PR_STATE_MACHINE.viewProjectionMatrix),
        &(PR_STATE_MACHINE.worldMatrix)
    );
}

void _pr_state_machine_projection_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.projectionMatrix), matrix);
    _update_viewprojection_matrix();
    _update_worldviewprojection_matrix();
}

void _pr_state_machine_view_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.viewMatrix), matrix);
    _update_viewprojection_matrix();
    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}

void _pr_state_machine_world_matrix(const pr_matrix4* matrix)
{
    _pr_matrix_copy(&(PR_STATE_MACHINE.worldMatrix), matrix);
    _update_worldview_matrix();
    _update_worldviewprojection_matrix();
}

