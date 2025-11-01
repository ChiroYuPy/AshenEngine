#include "Ashen/GraphicsAPI/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace ash {
    void OpenGLRendererAPI::Clear(ClearBuffer buffers) {
        glClear(static_cast<GLenum>(buffers));
    }

    void OpenGLRendererAPI::SetClearColor(const Vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::SetClearColor(const float r, const float g, const float b, const float a) {
        glClearColor(r, g, b, a);
    }

    void OpenGLRendererAPI::SetViewport(const u32 x, const u32 y, const u32 width, const u32 height) {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void OpenGLRendererAPI::EnableScissor() {
        if (!m_ScissorEnabled) {
            m_ScissorEnabled = true;
            glEnable(GL_SCISSOR_TEST);
        }
    }

    void OpenGLRendererAPI::DisableScissor() {
        if (m_ScissorEnabled) {
            m_ScissorEnabled = false;
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void OpenGLRendererAPI::SetScissor(const u32 x, const u32 y, const u32 width, const u32 height) {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void OpenGLRendererAPI::EnableDepthTest() {
        if (!m_DepthEnabled) {
            m_DepthEnabled = true;
            glEnable(GL_DEPTH_TEST);
        }
    }

    void OpenGLRendererAPI::DisableDepthTest() {
        if (m_DepthEnabled) {
            m_DepthEnabled = false;
            glDisable(GL_DEPTH_TEST);
        }
    }

    void OpenGLRendererAPI::SetDepthFunc(DepthFunc func) {
        glDepthFunc(static_cast<GLenum>(func));
    }

    void OpenGLRendererAPI::SetDepthWrite(const bool enable) {
        if (m_DepthWrite != enable) {
            m_DepthWrite = enable;
            glDepthMask(enable ? GL_TRUE : GL_FALSE);
        }
    }

    void OpenGLRendererAPI::EnableBlending() {
        if (!m_BlendEnabled) {
            m_BlendEnabled = true;
            glEnable(GL_BLEND);
        }
    }

    void OpenGLRendererAPI::DisableBlending() {
        if (m_BlendEnabled) {
            m_BlendEnabled = false;
            glDisable(GL_BLEND);
        }
    }

    void OpenGLRendererAPI::SetBlendFunc(BlendFactor src, BlendFactor dst) {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void OpenGLRendererAPI::SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB,
                                                  BlendFactor srcAlpha, BlendFactor dstAlpha) {
        glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    }

    void OpenGLRendererAPI::SetBlendOp(BlendEquation op) {
        glBlendEquation(static_cast<GLenum>(op));
    }

    void OpenGLRendererAPI::SetBlendColor(const Vec4& color) {
        glBlendColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::EnableCulling() {
        if (!m_CullingEnabled) {
            m_CullingEnabled = true;
            glEnable(GL_CULL_FACE);
        }
    }

    void OpenGLRendererAPI::DisableCulling() {
        if (m_CullingEnabled) {
            m_CullingEnabled = false;
            glDisable(GL_CULL_FACE);
        }
    }

    void OpenGLRendererAPI::SetCullFace(CullFaceMode mode) {
        glCullFace(static_cast<GLenum>(mode));
    }

    void OpenGLRendererAPI::SetFrontFace(FrontFace orientation) {
        glFrontFace(static_cast<GLenum>(orientation));
    }

    void OpenGLRendererAPI::SetPolygonMode(CullFaceMode faces, PolygonMode mode) {
        glPolygonMode(static_cast<GLenum>(faces), static_cast<GLenum>(mode));
    }

    void OpenGLRendererAPI::EnablePolygonOffset() {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glEnable(GL_POLYGON_OFFSET_POINT);
    }

    void OpenGLRendererAPI::DisablePolygonOffset() {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDisable(GL_POLYGON_OFFSET_POINT);
    }

    void OpenGLRendererAPI::SetPolygonOffset(const float factor, const float units) {
        glPolygonOffset(factor, units);
    }

    void OpenGLRendererAPI::SetPointSize(const float size) {
        glPointSize(size);
    }

    void OpenGLRendererAPI::SetLineWidth(const float width) {
        glLineWidth(width);
    }

    void OpenGLRendererAPI::EnableStencil() {
        if (!m_StencilEnabled) {
            m_StencilEnabled = true;
            glEnable(GL_STENCIL_TEST);
        }
    }

    void OpenGLRendererAPI::DisableStencil() {
        if (m_StencilEnabled) {
            m_StencilEnabled = false;
            glDisable(GL_STENCIL_TEST);
        }
    }

    void OpenGLRendererAPI::SetStencilFunc(StencilOp func, const int ref, const u32 mask) {
        glStencilFunc(static_cast<GLenum>(func), ref, mask);
    }

    void OpenGLRendererAPI::SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) {
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }

    void OpenGLRendererAPI::SetStencilMask(const u32 mask) {
        glStencilMask(mask);
    }

    void OpenGLRendererAPI::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE, b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    void OpenGLRendererAPI::EnableMultisample() {
        if (!m_MultisampleEnabled) {
            m_MultisampleEnabled = true;
            glEnable(GL_MULTISAMPLE);
        }
    }

    void OpenGLRendererAPI::DisableMultisample() {
        if (m_MultisampleEnabled) {
            m_MultisampleEnabled = false;
            glDisable(GL_MULTISAMPLE);
        }
    }

    void OpenGLRendererAPI::DrawArrays(PrimitiveType mode, const int first, const int count) {
        glDrawArrays(static_cast<GLenum>(mode), first, count);
    }

    void OpenGLRendererAPI::DrawElements(PrimitiveType mode, const int count, IndexType type, const void* indices) {
        glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices);
    }

    void OpenGLRendererAPI::DrawArraysInstanced(PrimitiveType mode, const int first, const int count, const int instanceCount) {
        glDrawArraysInstanced(static_cast<GLenum>(mode), first, count, instanceCount);
    }

    void OpenGLRendererAPI::DrawElementsInstanced(PrimitiveType mode, const int count, IndexType type, const void* indices, const int instanceCount) {
        glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, instanceCount);
    }

    bool OpenGLRendererAPI::IsDepthTestEnabled() const {
        return m_DepthEnabled;
    }

    bool OpenGLRendererAPI::IsBlendingEnabled() const {
        return m_BlendEnabled;
    }

    bool OpenGLRendererAPI::IsCullingEnabled() const {
        return m_CullingEnabled;
    }

    bool OpenGLRendererAPI::IsStencilEnabled() const {
        return m_StencilEnabled;
    }

    bool OpenGLRendererAPI::IsWireframeEnabled() const {
        return m_Wireframe;
    }

    bool OpenGLRendererAPI::IsScissorEnabled() const {
        return m_ScissorEnabled;
    }
}