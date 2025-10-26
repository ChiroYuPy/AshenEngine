#ifndef ASHEN_RENDERCOMMAND_H
#define ASHEN_RENDERCOMMAND_H

#include "GLEnums.h"
#include "VertexArray.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class RenderCommand {
    public:
        // ---- Clear ----
        static void Clear(ClearBuffer buffers = ClearBuffer::Color | ClearBuffer::Depth);

        static void SetClearColor(const Vec4 &color);

        static void SetClearColor(float r, float g, float b, float a);

        // ---- Viewport / Scissor ----
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void SetViewport(uint32_t width, uint32_t height);

        static void EnableScissor();

        static void DisableScissor();

        static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        // ---- Depth ----
        static void EnableDepthTest();

        static void DisableDepthTest();

        static void SetDepthFunc(DepthFunc func = DepthFunc::Less);

        static void SetDepthWrite(bool enable);

        // ---- Blending ----
        static void EnableBlending();

        static void DisableBlending();

        static void SetBlendFunc(BlendFactor src = BlendFactor::SrcAlpha,
                                 BlendFactor dst = BlendFactor::OneMinusSrcAlpha);

        static void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha,
                                         BlendFactor dstAlpha);

        static void SetBlendOp(BlendEquation op = BlendEquation::Add);

        static void SetBlendColor(const Vec4 &color);

        // ---- Culling ----
        static void EnableCulling();

        static void DisableCulling();

        static void SetCullFace(CullFaceMode mode = CullFaceMode::Back);

        static void SetFrontFace(FrontFace orientation = FrontFace::CounterClockwise);

        // ---- Polygon ----
        static void SetPolygonMode(CullFaceMode faces, PolygonMode mode);

        static void EnablePolygonOffset();

        static void DisablePolygonOffset();

        static void SetPolygonOffset(float factor, float units);

        static void SetPointSize(float size);

        static void SetLineWidth(float width);

        // ---- Stencil ----
        static void EnableStencil();

        static void DisableStencil();

        static void SetStencilFunc(StencilOp func, int ref, uint32_t mask);

        static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass);

        static void SetStencilMask(uint32_t mask);

        // ---- Color Mask ----
        static void SetColorMask(bool r, bool g, bool b, bool a);

        // ---- Multisample ----
        static void EnableMultisample();

        static void DisableMultisample();

        // ---- Drawing ----
        static void DrawArrays(PrimitiveType mode, int first, int count);

        static void DrawElements(PrimitiveType mode, int count, IndexType type, const void *indices);

        static void DrawArraysInstanced(PrimitiveType mode, int first, int count, int instanceCount);

        static void DrawElementsInstanced(PrimitiveType mode, int count, IndexType type, const void *indices,
                                          int instanceCount);

        // ---- Queries ----
        [[nodiscard]] static bool IsDepthTestEnabled();

        [[nodiscard]] static bool IsBlendingEnabled();

        [[nodiscard]] static bool IsCullingEnabled();

        [[nodiscard]] static bool IsStencilEnabled();

        [[nodiscard]] static bool IsWireframeEnabled();

        [[nodiscard]] static bool IsScissorEnabled();

    private:
        static inline bool s_DepthEnabled = false;
        static inline bool s_BlendEnabled = false;
        static inline bool s_CullingEnabled = false;
        static inline bool s_StencilEnabled = false;
        static inline bool s_Wireframe = false;
        static inline bool s_ScissorEnabled = false;
        static inline bool s_DepthWrite = true;
        static inline bool s_MultisampleEnabled = true;
    };
} // namespace ash

#endif // ASHEN_RENDERCOMMAND_H