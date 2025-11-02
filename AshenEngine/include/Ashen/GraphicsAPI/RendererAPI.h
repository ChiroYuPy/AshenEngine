#ifndef ASHEN_RENDERERAPI_H
#define ASHEN_RENDERERAPI_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "Ashen/GraphicsAPI/Enums.h"

namespace ash {
    // Forward declarations
    class VertexArray;
    class Shader;

    class RendererAPI {
    public:
        enum class API {
            None = 0,
            OpenGL = 1,
            Vulkan = 2,
            DirectX12 = 3
        };

        virtual ~RendererAPI() = default;

        // === Lifecycle ===
        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        // === Clear Operations ===
        virtual void Clear(ClearBuffer buffers) = 0;
        virtual void SetClearColor(const Vec4& color) = 0;
        virtual void SetClearColor(float r, float g, float b, float a) = 0;

        // === Viewport & Scissor ===
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void EnableScissor() = 0;
        virtual void DisableScissor() = 0;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        // === Depth Testing ===
        virtual void EnableDepthTest() = 0;
        virtual void DisableDepthTest() = 0;
        virtual void SetDepthFunc(DepthFunc func) = 0;
        virtual void SetDepthWrite(bool enable) = 0;

        // === Blending ===
        virtual void EnableBlending() = 0;
        virtual void DisableBlending() = 0;
        virtual void SetBlendFunc(BlendFactor src, BlendFactor dst) = 0;
        virtual void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) = 0;
        virtual void SetBlendOp(BlendEquation op) = 0;
        virtual void SetBlendColor(const Vec4& color) = 0;

        // === Culling ===
        virtual void EnableCulling() = 0;
        virtual void DisableCulling() = 0;
        virtual void SetCullFace(CullFaceMode mode) = 0;
        virtual void SetFrontFace(FrontFace orientation) = 0;

        // === Polygon Mode ===
        virtual void SetPolygonMode(CullFaceMode faces, PolygonMode mode) = 0;
        virtual void EnablePolygonOffset() = 0;
        virtual void DisablePolygonOffset() = 0;
        virtual void SetPolygonOffset(float factor, float units) = 0;

        // === Rendering Primitives ===
        virtual void SetPointSize(float size) = 0;
        virtual void SetLineWidth(float width) = 0;

        // === Stencil Testing ===
        virtual void EnableStencil() = 0;
        virtual void DisableStencil() = 0;
        virtual void SetStencilFunc(StencilOp func, int ref, uint32_t mask) = 0;
        virtual void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) = 0;
        virtual void SetStencilMask(uint32_t mask) = 0;

        // === Color Mask ===
        virtual void SetColorMask(bool r, bool g, bool b, bool a) = 0;

        // === Multisampling ===
        virtual void EnableMultisample() = 0;
        virtual void DisableMultisample() = 0;

        // === Draw Commands ===
        virtual void DrawArrays(PrimitiveType mode, int first, int count) = 0;
        virtual void DrawElements(PrimitiveType mode, int count, IndexType type, const void* indices) = 0;
        virtual void DrawArraysInstanced(PrimitiveType mode, int first, int count, int instanceCount) = 0;
        virtual void DrawElementsInstanced(PrimitiveType mode, int count, IndexType type, const void* indices, int instanceCount) = 0;

        // === Draw Commands with VertexArray ===
        virtual void DrawVertexArray(const Ref<VertexArray>& vertexArray);
        virtual void DrawVertexArrayInstanced(const Ref<VertexArray>& vertexArray, uint32_t instanceCount);

        // === State Queries ===
        [[nodiscard]] virtual bool IsDepthTestEnabled() const = 0;
        [[nodiscard]] virtual bool IsBlendingEnabled() const = 0;
        [[nodiscard]] virtual bool IsCullingEnabled() const = 0;
        [[nodiscard]] virtual bool IsStencilEnabled() const = 0;
        [[nodiscard]] virtual bool IsWireframeEnabled() const = 0;
        [[nodiscard]] virtual bool IsScissorEnabled() const = 0;

        [[nodiscard]] static API GetAPI() { return s_API; }
        static void SetAPI(const API api) { s_API = api; }

        static Ref<RendererAPI> Create();

    protected:
        RendererAPI() = default;

    private:
        static API s_API;
    };
}

#endif //ASHEN_RENDERERAPI_H