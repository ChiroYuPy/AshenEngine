#include "Ashen/GraphicsAPI/RenderState.h"

#include "Ashen/GraphicsAPI/GLEnums.h"

namespace ash {

    void RenderState::Clear(ClearBuffer buffers) {
        glClear(static_cast<GLbitfield>(buffers));
    }

    void RenderState::SetClearColor(const Vec4 &color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderState::SetClearColor(const float r, const float g, const float b, const float a) {
        glClearColor(r, g, b, a);
    }

    // ---- Viewport / Scissor ----

    void RenderState::SetViewport(const uint32_t x, const uint32_t y,
                                    const uint32_t width, const uint32_t height) {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y),
                   static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderState::SetViewport(const uint32_t width, const uint32_t height) {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderState::EnableScissor(const bool enable) {
        if (s_ScissorEnabled == enable) return;
        s_ScissorEnabled = enable;
        if (enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void RenderState::SetScissor(const uint32_t x, const uint32_t y,
                                   const uint32_t width, const uint32_t height) {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y),
                  static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    // ---- Depth ----

    void RenderState::EnableDepthTest(const bool enable) {
        if (s_DepthEnabled == enable) return;
        s_DepthEnabled = enable;
        if (enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void RenderState::SetDepthFunc(const DepthFunc func) {
        glDepthFunc(static_cast<GLenum>(func));
    }

    void RenderState::SetDepthWrite(const bool enable) {
        if (s_DepthWrite == enable) return;
        s_DepthWrite = enable;
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }

    // ---- Blending ----

    void RenderState::EnableBlending(const bool enable) {
        if (s_BlendEnabled == enable) return;
        s_BlendEnabled = enable;
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    void RenderState::SetBlendFunc(const BlendFactor src, const BlendFactor dst) {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void RenderState::SetBlendFuncSeparate(const BlendFactor srcRGB, const BlendFactor dstRGB,
                                             const BlendFactor srcAlpha, const BlendFactor dstAlpha) {
        glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB),
                            static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    }

    void RenderState::SetBlendOp(const BlendEquation op) {
        glBlendEquation(static_cast<GLenum>(op));
    }

    void RenderState::SetBlendColor(const Vec4 &color) {
        glBlendColor(color.r, color.g, color.b, color.a);
    }

    // ---- Culling ----

    void RenderState::EnableCulling(const bool enable) {
        if (s_CullingEnabled == enable) return;
        s_CullingEnabled = enable;
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void RenderState::SetCullFace(const CullFaceMode mode) {
        glCullFace(static_cast<GLenum>(mode));
    }

    void RenderState::SetFrontFace(const FrontFace orientation) {
        glFrontFace(static_cast<GLenum>(orientation));
    }

    // ---- Polygon ----

    void RenderState::SetPolygonMode(const CullFaceMode faces, const PolygonMode mode) {
        glPolygonMode(static_cast<GLenum>(faces), static_cast<GLenum>(mode));
    }

    void RenderState::SetPointSize(const float size) {
        glPointSize(size);
    }

    void RenderState::SetLineWidth(const float width) {
        glLineWidth(width);
    }

    void RenderState::EnablePolygonOffset(const bool enable) {
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

    void RenderState::SetPolygonOffset(const float factor, const float units) {
        glPolygonOffset(factor, units);
    }

    // ---- Stencil ----

    void RenderState::EnableStencil(const bool enable) {
        if (s_StencilEnabled == enable) return;
        s_StencilEnabled = enable;
        if (enable)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
    }

    void RenderState::SetStencilFunc(const StencilOp func, const int ref, const uint32_t mask) {
        glStencilFunc(static_cast<GLenum>(func), ref, mask);
    }

    void RenderState::SetStencilOp(const StencilOp sfail, const StencilOp dpfail, const StencilOp dppass) {
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }

    void RenderState::SetStencilMask(const uint32_t mask) {
        glStencilMask(mask);
    }

    // ---- Color mask ----

    void RenderState::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE,
                    b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    // ---- Multisample ----

    void RenderState::EnableMultisample(const bool enable) {
        if (s_MultisampleEnabled == enable) return;
        s_MultisampleEnabled = enable;
        if (enable)
            glEnable(GL_MULTISAMPLE);
        else
            glDisable(GL_MULTISAMPLE);
    }

    bool RenderState::IsDepthTestEnabled() { return s_DepthEnabled; }

    bool RenderState::IsBlendingEnabled() { return s_BlendEnabled; }

    bool RenderState::IsCullingEnabled() { return s_CullingEnabled; }

    bool RenderState::IsStencilEnabled() { return s_StencilEnabled; }

    bool RenderState::IsWireframeEnabled() { return s_Wireframe; }

    bool RenderState::IsScissorEnabled() { return s_ScissorEnabled; }
}
