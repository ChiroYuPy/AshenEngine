#ifndef ASHEN_RENDERCOMMAND_H
#define ASHEN_RENDERCOMMAND_H

#include "Ashen/GraphicsAPI/RendererAPI.h"

namespace ash {
    class RenderCommand {
    public:
        // === Lifecycle ===
        static void Init();
        static void Shutdown();

        // === Clear Operations ===
        static void Clear(ClearBuffer buffers = ClearBuffer::Color | ClearBuffer::Depth);
        static void SetClearColor(const Vec4& color);
        static void SetClearColor(float r, float g, float b, float a);

        // === Viewport & Scissor ===
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        static void SetViewport(uint32_t width, uint32_t height);

        static void EnableScissor();
        static void DisableScissor();
        static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        // === Depth Testing ===
        static void EnableDepthTest();
        static void DisableDepthTest();
        static void SetDepthFunc(DepthFunc func = DepthFunc::Less);
        static void SetDepthWrite(bool enable);

        // === Blending ===
        static void EnableBlending();
        static void DisableBlending();
        static void SetBlendFunc(BlendFactor src = BlendFactor::SrcAlpha, BlendFactor dst = BlendFactor::OneMinusSrcAlpha);
        static void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha);
        static void SetBlendOp(BlendEquation op = BlendEquation::Add);
        static void SetBlendColor(const Vec4& color);

        // === Culling ===
        static void EnableCulling();
        static void DisableCulling();
        static void SetCullFace(CullFaceMode mode = CullFaceMode::Back);
        static void SetFrontFace(FrontFace orientation = FrontFace::CounterClockwise);

        // === Polygon Mode ===
        static void SetPolygonMode(CullFaceMode faces, PolygonMode mode);
        static void EnablePolygonOffset();
        static void DisablePolygonOffset();
        static void SetPolygonOffset(float factor, float units);

        // === Rendering Primitives ===
        static void SetPointSize(float size);
        static void SetLineWidth(float width);

        // === Stencil Testing ===
        static void EnableStencil();
        static void DisableStencil();
        static void SetStencilFunc(StencilOp func, int ref, uint32_t mask);
        static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass);
        static void SetStencilMask(uint32_t mask);

        // === Color Mask ===
        static void SetColorMask(bool r, bool g, bool b, bool a);

        // === Multisampling ===
        static void EnableMultisample();
        static void DisableMultisample();

        // === Draw Commands (Low Level) ===
        static void DrawArrays(PrimitiveType mode, int first, int count);
        static void DrawElements(PrimitiveType mode, int count, IndexType type, const void* indices);
        static void DrawArraysInstanced(PrimitiveType mode, int first, int count, int instanceCount);
        static void DrawElementsInstanced(PrimitiveType mode, int count, IndexType type, const void* indices, int instanceCount);

        // === Draw Commands (High Level - Abstraits) ===
        /**
         * @brief Dessine un VertexArray automatiquement
         * Détecte si indexed ou non, et utilise la bonne méthode
         */
        static void Submit(const Ref<VertexArray>& vertexArray);

        /**
         * @brief Dessine un VertexArray avec instancing
         */
        static void SubmitInstanced(const Ref<VertexArray>& vertexArray, uint32_t instanceCount);

        // === State Queries ===
        [[nodiscard]] static bool IsDepthTestEnabled();
        [[nodiscard]] static bool IsBlendingEnabled();
        [[nodiscard]] static bool IsCullingEnabled();
        [[nodiscard]] static bool IsStencilEnabled();
        [[nodiscard]] static bool IsWireframeEnabled();
        [[nodiscard]] static bool IsScissorEnabled();

        // === API Access ===
        [[nodiscard]] static RendererAPI* GetAPI() { return s_API.get(); }

    private:
        RenderCommand() = delete;

        static Ref<RendererAPI> s_API;
    };

    class RenderState {
    public:
        struct Blend {
            static void Default() {
                RenderCommand::EnableBlending();
                RenderCommand::SetBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
            }

            static void Additive() {
                RenderCommand::EnableBlending();
                RenderCommand::SetBlendFunc(BlendFactor::SrcAlpha, BlendFactor::One);
            }

            static void Multiply() {
                RenderCommand::EnableBlending();
                RenderCommand::SetBlendFunc(BlendFactor::DstColor, BlendFactor::Zero);
            }

            static void None() {
                RenderCommand::DisableBlending();
            }
        };

        struct Depth {
            static void ReadWrite() {
                RenderCommand::EnableDepthTest();
                RenderCommand::SetDepthWrite(true);
                RenderCommand::SetDepthFunc(DepthFunc::Less);
            }

            static void ReadOnly() {
                RenderCommand::EnableDepthTest();
                RenderCommand::SetDepthWrite(false);
            }

            static void None() {
                RenderCommand::DisableDepthTest();
                RenderCommand::SetDepthWrite(false);
            }
        };

        struct Cull {
            static void Back() {
                RenderCommand::EnableCulling();
                RenderCommand::SetCullFace(CullFaceMode::Back);
            }

            static void Front() {
                RenderCommand::EnableCulling();
                RenderCommand::SetCullFace(CullFaceMode::Front);
            }

            static void None() {
                RenderCommand::DisableCulling();
            }
        };
    };
}

#endif // ASHEN_RENDERCOMMAND_H