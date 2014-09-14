/*
 * pico.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "pico.h"
#include "error.h"
#include "context.h"
#include "framebuffer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture.h"
#include "image.h"
#include "state_machine.h"
#include "render.h"
#include "helper.h"


// --- common --- //

PRboolean prInit()
{
    _pr_state_machine_init(&_stateMachine);
    return PR_TRUE;
}

PRboolean prRelease()
{
    //...
    return PR_TRUE;
}

PRenum prGetError()
{
    return _pr_error_get();
}

void prErrorHandler(PR_ERROR_HANDLER_PROC errorHandler)
{
    _pr_error_set_handler(errorHandler);
}

const char* prGetString(PRenum str)
{
    switch (str)
    {
        case PR_STRING_VERSION:
            return PR_VERSION_STR;
        case PR_STRING_RENDERER:
            return "Pico Renderer";
    }
    return NULL;
}

// --- context --- //

PRobject prGenContext(const pr_context_desc* desc, PRuint width, PRuint height)
{
    return (PRobject)_pr_context_create(desc, width, height);
}

void prDeleteContext(PRobject context)
{
    _pr_context_delete((pr_context*)context);
}

void prPresent(PRobject context)
{
    _pr_context_present((pr_context*)context, _stateMachine.boundFrameBuffer);
}

// --- framebuffer --- //

PRobject prGenFrameBuffer(PRuint width, PRuint height)
{
    return (PRobject)_pr_framebuffer_create(width, height);
}

void prDeleteFrameBuffer(PRobject frameBuffer)
{
    _pr_framebuffer_delete((pr_framebuffer*)frameBuffer);
}

void prBindFrameBuffer(PRobject frameBuffer)
{
    _pr_state_machine_bind_framebuffer((pr_framebuffer*)frameBuffer);
}

void prClearFrameBuffer(PRubyte clearColor, float depth)
{
    _pr_framebuffer_clear(_stateMachine.boundFrameBuffer, clearColor, depth);
}

PRubyte prGetColorIndex(PRubyte red, PRubyte green, PRubyte blue)
{
    return _pr_color_to_colorindex_r3g3b2(red, green, blue);
}

// --- texture --- //

PRobject prGenTexture()
{
    return (PRobject)_pr_texture_create();
}

void prDeleteTexture(PRobject texture)
{
    _pr_texture_delete((pr_texture*)texture);
}

void prBindTexture(PRobject texture)
{
    _pr_state_machine_bind_texture((pr_texture*)texture);
}

void prTextureImage2D(
    PRobject texture, PRtexsize width, PRtexsize height, PRenum format,
    const PRvoid* data, PRboolean dither, PRboolean generateMips)
{
    _pr_texture_image2d((pr_texture*)texture, width, height, format, data, dither, generateMips);
}

void prTextureImage2DFromFile(
    PRobject texture, const char* filename, PRboolean dither, PRboolean generateMips)
{
    pr_image* image = _pr_image_load_from_file(filename);

    _pr_texture_image2d(
        (pr_texture*)texture,
        (PRtexsize)(image->width),
        (PRtexsize)(image->height),
        PR_IMAGE_FORMAT_UBYTE_RGB,
        image->colors,
        dither,
        generateMips
    );

    _pr_image_delete(image);
}

// --- vertexbuffer --- //

PRobject prGenVertexBuffer()
{
    return (PRobject)_pr_vertexbuffer_create();
}

void prDeleteVertexBuffer(PRobject vertexBuffer)
{
    _pr_vertexbuffer_delete((pr_vertexbuffer*)vertexBuffer);
}

void prVertexBufferData(PRobject vertexBuffer, const PRvertex* vertices, PRsizei numVertices)
{
    _pr_vertexbuffer_data((pr_vertexbuffer*)vertexBuffer, vertices, numVertices);
}

// --- indexbuffer --- //

PRobject prGenIndexbuffer(PRsizei numIndices)
{
    return (PRobject)_pr_indexbuffer_create(numIndices);
}

void prDeleteIndexbuffer(PRobject indexbuffer)
{
    _pr_indexbuffer_delete((pr_indexbuffer*)indexbuffer);
}

// --- matrices --- //

void prProjectionMatrix(const PRfloat* matrix4x4)
{
    _pr_state_machine_projection_matrix((pr_matrix4*)matrix4x4);
}

void prViewMatrix(const PRfloat* matrix4x4)
{
    _pr_state_machine_view_matrix((pr_matrix4*)matrix4x4);
}

void prWorldMatrix(const PRfloat* matrix4x4)
{
    _pr_state_machine_world_matrix((pr_matrix4*)matrix4x4);
}

void prBuildPerspectiveProjection(
    PRfloat* matrix4x4, PRfloat aspectRatio, PRfloat nearPlane, PRfloat farPlane, PRfloat fov)
{
    _pr_matrix_build_perspective((pr_matrix4*)matrix4x4, aspectRatio, nearPlane, farPlane, fov);
}

void prBuildOrthogonalProjection(
    PRfloat* matrix4x4, PRfloat width, PRfloat height, PRfloat nearPlane, PRfloat farPlane)
{
    _pr_matrix_build_orthogonal((pr_matrix4*)matrix4x4, width, height, nearPlane, farPlane);
}

void prTranslate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z)
{
    _pr_matrix_translate((pr_matrix4*)matrix4x4, x, y, z);
}

void prRotate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z, PRfloat angle)
{
    _pr_matrix_rotate((pr_matrix4*)matrix4x4, x, y, z, angle);
}

void prScale(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z)
{
    _pr_matrix_scale((pr_matrix4*)matrix4x4, x, y, z);
}

void prLoadIdentity(PRfloat* matrix4x4)
{
    _pr_matrix_load_identity((pr_matrix4*)matrix4x4);
}

// --- drawing --- //

void prColor(PRubyte colorIndex)
{
    _stateMachine.colorIndex = colorIndex;
}

void prDrawScreenPoint(PRint x, PRint y, PRubyte colorIndex)
{
    _pr_render_screenspace_point(x, y, colorIndex);
}

void prDrawScreenLine(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex)
{
    _pr_render_screenspace_line(x1, y1, x2, y2, colorIndex);
}

void prDrawScreenImage(PRint left, PRint top, PRint right, PRint bottom)
{
    _pr_render_screenspace_image(left, top, right, bottom);
}

void prDraw(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

void prDrawIndexed(PRenum primitives, PRuint numVertices, PRuint firstVertex)
{
    //...
}

