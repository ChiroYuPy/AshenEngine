#include "Ashen/GraphicsAPI/RenderCommand.h"
#include "Ashen/GraphicsAPI/GLEnums.h"

namespace ash {
    void RenderCommand::Clear(ClearBuffer buffers) {
        glClear(static_cast<GLbitfield>(buffers));
    }

    void RenderCommand::SetClearColor(const Vec4 &color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::SetClearColor(const float r, const float g, const float b, const float a) {
        glClearColor(r, g, b, a);
    }

    void RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderCommand::SetViewport(const uint32_t width, const uint32_t height) {
        SetViewport(0, 0, width, height);
    }

    void RenderCommand::EnableScissor() {
        if (!s_ScissorEnabled) {
            s_ScissorEnabled = true;
            glEnable(GL_SCISSOR_TEST);
        }
    }

    void RenderCommand::DisableScissor() {
        if (s_ScissorEnabled) {
            s_ScissorEnabled = false;
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderCommand::EnableDepthTest() {
        if (!s_DepthEnabled) {
            s_DepthEnabled = true;
            glEnable(GL_DEPTH_TEST);
        }
    }

    void RenderCommand::DisableDepthTest() {
        if (s_DepthEnabled) {
            s_DepthEnabled = false;
            glDisable(GL_DEPTH_TEST);
        }
    }

    void RenderCommand::SetDepthFunc(DepthFunc func) {
        glDepthFunc(static_cast<GLenum>(func));
    }

    void RenderCommand::SetDepthWrite(const bool enable) {
        if (s_DepthWrite != enable) {
            s_DepthWrite = enable;
            glDepthMask(enable ? GL_TRUE : GL_FALSE);
        }
    }

    void RenderCommand::EnableBlending() {
        if (!s_BlendEnabled) {
            s_BlendEnabled = true;
            glEnable(GL_BLEND);
        }
    }

    void RenderCommand::DisableBlending() {
        if (s_BlendEnabled) {
            s_BlendEnabled = false;
            glDisable(GL_BLEND);
        }
    }

    void RenderCommand::SetBlendFunc(BlendFactor src, BlendFactor dst) {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void RenderCommand::SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) {
        glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    }

    void RenderCommand::SetBlendOp(BlendEquation op) {
        glBlendEquation(static_cast<GLenum>(op));
    }

    void RenderCommand::SetBlendColor(const Vec4 &color) {
        glBlendColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::EnableCulling() {
        if (!s_CullingEnabled) {
            s_CullingEnabled = true;
            glEnable(GL_CULL_FACE);
        }
    }

    void RenderCommand::DisableCulling() {
        if (s_CullingEnabled) {
            s_CullingEnabled = false;
            glDisable(GL_CULL_FACE);
        }
    }

    void RenderCommand::SetCullFace(CullFaceMode mode) {
        glCullFace(static_cast<GLenum>(mode));
    }

    void RenderCommand::SetFrontFace(FrontFace orientation) {
        glFrontFace(static_cast<GLenum>(orientation));
    }

    void RenderCommand::SetPolygonMode(CullFaceMode faces, PolygonMode mode) {
        glPolygonMode(static_cast<GLenum>(faces), static_cast<GLenum>(mode));
    }

    void RenderCommand::EnablePolygonOffset() {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glEnable(GL_POLYGON_OFFSET_POINT);
    }

    void RenderCommand::DisablePolygonOffset() {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDisable(GL_POLYGON_OFFSET_POINT);
    }

    void RenderCommand::SetPolygonOffset(const float factor, const float units) {
        glPolygonOffset(factor, units);
    }

    void RenderCommand::SetPointSize(const float size) {
        glPointSize(size);
    }

    void RenderCommand::SetLineWidth(const float width) {
        glLineWidth(width);
    }

    void RenderCommand::EnableStencil() {
        if (!s_StencilEnabled) {
            s_StencilEnabled = true;
            glEnable(GL_STENCIL_TEST);
        }
    }

    void RenderCommand::DisableStencil() {
        if (s_StencilEnabled) {
            s_StencilEnabled = false;
            glDisable(GL_STENCIL_TEST);
        }
    }

    void RenderCommand::SetStencilFunc(StencilOp func, const int ref, const uint32_t mask) {
        glStencilFunc(static_cast<GLenum>(func), ref, mask);
    }

    void RenderCommand::SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) {
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }

    void RenderCommand::SetStencilMask(const uint32_t mask) {
        glStencilMask(mask);
    }

    void RenderCommand::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE,
                    b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    void RenderCommand::EnableMultisample() {
        if (!s_MultisampleEnabled) {
            s_MultisampleEnabled = true;
            glEnable(GL_MULTISAMPLE);
        }
    }

    void RenderCommand::DisableMultisample() {
        if (s_MultisampleEnabled) {
            s_MultisampleEnabled = false;
            glDisable(GL_MULTISAMPLE);
        }
    }

    void RenderCommand::DrawArrays(PrimitiveType mode, const int first, const int count) {
        glDrawArrays(static_cast<GLenum>(mode), first, count);
    }

    void RenderCommand::DrawElements(PrimitiveType mode, const int count, IndexType type, const void *indices) {
        glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices);
    }

    void RenderCommand::DrawArraysInstanced(PrimitiveType mode, const int first, const int count, const int instanceCount) {
        glDrawArraysInstanced(static_cast<GLenum>(mode), first, count, instanceCount);
    }

    void RenderCommand::DrawElementsInstanced(PrimitiveType mode, const int count, IndexType type, const void *indices, const int instanceCount) {
        glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, instanceCount);
    }

    bool RenderCommand::IsDepthTestEnabled() { return s_DepthEnabled; }
    bool RenderCommand::IsBlendingEnabled() { return s_BlendEnabled; }
    bool RenderCommand::IsCullingEnabled() { return s_CullingEnabled; }
    bool RenderCommand::IsStencilEnabled() { return s_StencilEnabled; }
    bool RenderCommand::IsWireframeEnabled() { return s_Wireframe; }
    bool RenderCommand::IsScissorEnabled() { return s_ScissorEnabled; }
}