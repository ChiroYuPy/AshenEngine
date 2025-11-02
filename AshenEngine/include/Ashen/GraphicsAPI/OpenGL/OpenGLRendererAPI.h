#ifndef ASHEN_OPENGLRENDERERAPI_H
#define ASHEN_OPENGLRENDERERAPI_H

#include "Ashen/GraphicsAPI/RendererAPI.h"

namespace ash {
    class OpenGLRendererAPI final : public RendererAPI {
    public:
        OpenGLRendererAPI() = default;
        ~OpenGLRendererAPI() override = default;

        // Non-copiable, non-déplaçable
        OpenGLRendererAPI(const OpenGLRendererAPI&) = delete;
        OpenGLRendererAPI& operator=(const OpenGLRendererAPI&) = delete;
        OpenGLRendererAPI(OpenGLRendererAPI&&) = delete;
        OpenGLRendererAPI& operator=(OpenGLRendererAPI&&) = delete;

        // === Lifecycle ===
        void Init() override;
        void Shutdown() override;

        // === Clear Operations ===
        void Clear(ClearBuffer buffers) override;
        void SetClearColor(const Vec4& color) override;
        void SetClearColor(float r, float g, float b, float a) override;

        // === Viewport & Scissor ===
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void EnableScissor() override;
        void DisableScissor() override;
        void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        // === Depth Testing ===
        void EnableDepthTest() override;
        void DisableDepthTest() override;
        void SetDepthFunc(DepthFunc func) override;
        void SetDepthWrite(bool enable) override;

        // === Blending ===
        void EnableBlending() override;
        void DisableBlending() override;
        void SetBlendFunc(BlendFactor src, BlendFactor dst) override;
        void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB,
                                  BlendFactor srcAlpha, BlendFactor dstAlpha) override;
        void SetBlendOp(BlendEquation op) override;
        void SetBlendColor(const Vec4& color) override;

        // === Culling ===
        void EnableCulling() override;
        void DisableCulling() override;
        void SetCullFace(CullFaceMode mode) override;
        void SetFrontFace(FrontFace orientation) override;

        // === Polygon Mode ===
        void SetPolygonMode(CullFaceMode faces, PolygonMode mode) override;
        void EnablePolygonOffset() override;
        void DisablePolygonOffset() override;
        void SetPolygonOffset(float factor, float units) override;

        // === Rendering Primitives ===
        void SetPointSize(float size) override;
        void SetLineWidth(float width) override;

        // === Stencil Testing ===
        void EnableStencil() override;
        void DisableStencil() override;
        void SetStencilFunc(StencilOp func, int ref, uint32_t mask) override;
        void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) override;
        void SetStencilMask(uint32_t mask) override;

        // === Color Mask ===
        void SetColorMask(bool r, bool g, bool b, bool a) override;

        // === Multisampling ===
        void EnableMultisample() override;
        void DisableMultisample() override;

        // === Draw Commands ===
        void DrawArrays(PrimitiveType mode, int first, int count) override;
        void DrawElements(PrimitiveType mode, int count, IndexType type, const void* indices) override;
        void DrawArraysInstanced(PrimitiveType mode, int first, int count, int instanceCount) override;
        void DrawElementsInstanced(PrimitiveType mode, int count, IndexType type,
                                  const void* indices, int instanceCount) override;

        // === State Queries ===
        [[nodiscard]] bool IsDepthTestEnabled() const override { return m_DepthEnabled; }
        [[nodiscard]] bool IsBlendingEnabled() const override { return m_BlendEnabled; }
        [[nodiscard]] bool IsCullingEnabled() const override { return m_CullingEnabled; }
        [[nodiscard]] bool IsStencilEnabled() const override { return m_StencilEnabled; }
        [[nodiscard]] bool IsWireframeEnabled() const override { return m_Wireframe; }
        [[nodiscard]] bool IsScissorEnabled() const override { return m_ScissorEnabled; }

    private:
        // Cache des états pour éviter les appels OpenGL redondants
        bool m_DepthEnabled = false;
        bool m_BlendEnabled = false;
        bool m_CullingEnabled = false;
        bool m_StencilEnabled = false;
        bool m_Wireframe = false;
        bool m_ScissorEnabled = false;
        bool m_DepthWrite = true;
        bool m_MultisampleEnabled = true;
        bool m_Initialized = false;
    };
}

#endif // ASHEN_OPENGLRENDERERAPI_H