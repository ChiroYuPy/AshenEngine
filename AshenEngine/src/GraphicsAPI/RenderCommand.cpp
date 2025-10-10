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

    // ---- Viewport / Scissor ----

    void RenderCommand::SetViewport(const uint32_t x, const uint32_t y,
                                    const uint32_t width, const uint32_t height) {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y),
                   static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderCommand::SetViewport(const uint32_t width, const uint32_t height) {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderCommand::EnableScissor(const bool enable) {
        if (s_ScissorEnabled == enable) return;
        s_ScissorEnabled = enable;
        if (enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void RenderCommand::SetScissor(const uint32_t x, const uint32_t y,
                                   const uint32_t width, const uint32_t height) {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y),
                  static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    // ---- Depth ----

    void RenderCommand::EnableDepthTest(const bool enable) {
        if (s_DepthEnabled == enable) return;
        s_DepthEnabled = enable;
        if (enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void RenderCommand::SetDepthFunc(const DepthFunc func) {
        glDepthFunc(static_cast<GLenum>(func));
    }

    void RenderCommand::SetDepthWrite(const bool enable) {
        if (s_DepthWrite == enable) return;
        s_DepthWrite = enable;
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }

    // ---- Blending ----

    void RenderCommand::EnableBlending(const bool enable) {
        if (s_BlendEnabled == enable) return;
        s_BlendEnabled = enable;
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    void RenderCommand::SetBlendFunc(const BlendFactor src, const BlendFactor dst) {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void RenderCommand::SetBlendFuncSeparate(const BlendFactor srcRGB, const BlendFactor dstRGB,
                                             const BlendFactor srcAlpha, const BlendFactor dstAlpha) {
        glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB),
                            static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    }

    void RenderCommand::SetBlendOp(const BlendEquation op) {
        glBlendEquation(static_cast<GLenum>(op));
    }

    void RenderCommand::SetBlendColor(const Vec4 &color) {
        glBlendColor(color.r, color.g, color.b, color.a);
    }

    // ---- Culling ----

    void RenderCommand::EnableCulling(const bool enable) {
        if (s_CullingEnabled == enable) return;
        s_CullingEnabled = enable;
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void RenderCommand::SetCullFace(const CullFaceMode mode) {
        glCullFace(static_cast<GLenum>(mode));
    }

    void RenderCommand::SetFrontFace(const FrontFace orientation) {
        glFrontFace(static_cast<GLenum>(orientation));
    }

    // ---- Polygon ----

    void RenderCommand::SetPolygonMode(const CullFaceMode faces, const PolygonMode mode) {
        glPolygonMode(static_cast<GLenum>(faces), static_cast<GLenum>(mode));
    }

    void RenderCommand::SetPointSize(const float size) {
        glPointSize(size);
    }

    void RenderCommand::SetLineWidth(const float width) {
        glLineWidth(width);
    }

    void RenderCommand::EnablePolygonOffset(const bool enable) {
        if (enable) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_POINT);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
            glDisable(GL_POLYGON_OFFSET_POINT);
        }
    }

    void RenderCommand::SetPolygonOffset(const float factor, const float units) {
        glPolygonOffset(factor, units);
    }

    // ---- Stencil ----

    void RenderCommand::EnableStencil(const bool enable) {
        if (s_StencilEnabled == enable) return;
        s_StencilEnabled = enable;
        if (enable)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
    }

    void RenderCommand::SetStencilFunc(const StencilOp func, const int ref, const uint32_t mask) {
        glStencilFunc(static_cast<GLenum>(func), ref, mask);
    }

    void RenderCommand::SetStencilOp(const StencilOp sfail, const StencilOp dpfail, const StencilOp dppass) {
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }

    void RenderCommand::SetStencilMask(const uint32_t mask) {
        glStencilMask(mask);
    }

    // ---- Color mask ----

    void RenderCommand::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE,
                    b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    // ---- Multisample ----

    void RenderCommand::EnableMultisample(const bool enable) {
        if (s_MultisampleEnabled == enable) return;
        s_MultisampleEnabled = enable;
        if (enable)
            glEnable(GL_MULTISAMPLE);
        else
            glDisable(GL_MULTISAMPLE);
    }

    bool RenderCommand::IsDepthTestEnabled() { return s_DepthEnabled; }

    bool RenderCommand::IsBlendingEnabled() { return s_BlendEnabled; }

    bool RenderCommand::IsCullingEnabled() { return s_CullingEnabled; }

    bool RenderCommand::IsStencilEnabled() { return s_StencilEnabled; }

    bool RenderCommand::IsWireframeEnabled() { return s_Wireframe; }

    bool RenderCommand::IsScissorEnabled() { return s_ScissorEnabled; }
}
