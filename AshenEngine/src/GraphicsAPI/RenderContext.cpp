#include "Ashen/GraphicsAPI/RenderContext.h"

#include "Ashen/GraphicsAPI/GLEnums.h"

namespace ash {

    void RenderContext::Clear(ClearBuffer buffers) {
        glClear(static_cast<GLbitfield>(buffers));
    }

    void RenderContext::SetClearColor(const Vec4 &color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderContext::SetClearColor(const float r, const float g, const float b, const float a) {
        glClearColor(r, g, b, a);
    }

    // ---- Viewport / Scissor ----

    void RenderContext::SetViewport(const uint32_t x, const uint32_t y,
                                    const uint32_t width, const uint32_t height) {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y),
                   static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderContext::SetViewport(const uint32_t width, const uint32_t height) {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderContext::EnableScissor(const bool enable) {
        if (s_ScissorEnabled == enable) return;
        s_ScissorEnabled = enable;
        if (enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void RenderContext::SetScissor(const uint32_t x, const uint32_t y,
                                   const uint32_t width, const uint32_t height) {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y),
                  static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    // ---- Depth ----

    void RenderContext::EnableDepthTest(const bool enable) {
        if (s_DepthEnabled == enable) return;
        s_DepthEnabled = enable;
        if (enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void RenderContext::SetDepthFunc(const DepthFunc func) {
        glDepthFunc(static_cast<GLenum>(func));
    }

    void RenderContext::SetDepthWrite(const bool enable) {
        if (s_DepthWrite == enable) return;
        s_DepthWrite = enable;
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }

    // ---- Blending ----

    void RenderContext::EnableBlending(const bool enable) {
        if (s_BlendEnabled == enable) return;
        s_BlendEnabled = enable;
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    void RenderContext::SetBlendFunc(const BlendFactor src, const BlendFactor dst) {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void RenderContext::SetBlendFuncSeparate(const BlendFactor srcRGB, const BlendFactor dstRGB,
                                             const BlendFactor srcAlpha, const BlendFactor dstAlpha) {
        glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB),
                            static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    }

    void RenderContext::SetBlendOp(const BlendEquation op) {
        glBlendEquation(static_cast<GLenum>(op));
    }

    void RenderContext::SetBlendColor(const Vec4 &color) {
        glBlendColor(color.r, color.g, color.b, color.a);
    }

    // ---- Culling ----

    void RenderContext::EnableCulling(const bool enable) {
        if (s_CullingEnabled == enable) return;
        s_CullingEnabled = enable;
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void RenderContext::SetCullFace(const CullFaceMode mode) {
        glCullFace(static_cast<GLenum>(mode));
    }

    void RenderContext::SetFrontFace(const FrontFace orientation) {
        glFrontFace(static_cast<GLenum>(orientation));
    }

    // ---- Polygon ----

    void RenderContext::SetPolygonMode(const CullFaceMode faces, const PolygonMode mode) {
        glPolygonMode(static_cast<GLenum>(faces), static_cast<GLenum>(mode));
    }

    void RenderContext::SetPointSize(const float size) {
        glPointSize(size);
    }

    void RenderContext::SetLineWidth(const float width) {
        glLineWidth(width);
    }

    void RenderContext::EnablePolygonOffset(const bool enable) {
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

    void RenderContext::SetPolygonOffset(const float factor, const float units) {
        glPolygonOffset(factor, units);
    }

    // ---- Stencil ----

    void RenderContext::EnableStencil(const bool enable) {
        if (s_StencilEnabled == enable) return;
        s_StencilEnabled = enable;
        if (enable)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
    }

    void RenderContext::SetStencilFunc(const StencilOp func, const int ref, const uint32_t mask) {
        glStencilFunc(static_cast<GLenum>(func), ref, mask);
    }

    void RenderContext::SetStencilOp(const StencilOp sfail, const StencilOp dpfail, const StencilOp dppass) {
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }

    void RenderContext::SetStencilMask(const uint32_t mask) {
        glStencilMask(mask);
    }

    // ---- Color mask ----

    void RenderContext::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE,
                    b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    // ---- Multisample ----

    void RenderContext::EnableMultisample(const bool enable) {
        if (s_MultisampleEnabled == enable) return;
        s_MultisampleEnabled = enable;
        if (enable)
            glEnable(GL_MULTISAMPLE);
        else
            glDisable(GL_MULTISAMPLE);
    }

    bool RenderContext::IsDepthTestEnabled() { return s_DepthEnabled; }

    bool RenderContext::IsBlendingEnabled() { return s_BlendEnabled; }

    bool RenderContext::IsCullingEnabled() { return s_CullingEnabled; }

    bool RenderContext::IsStencilEnabled() { return s_StencilEnabled; }

    bool RenderContext::IsWireframeEnabled() { return s_Wireframe; }

    bool RenderContext::IsScissorEnabled() { return s_ScissorEnabled; }
}
