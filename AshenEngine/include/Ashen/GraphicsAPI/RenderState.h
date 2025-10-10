#ifndef ASHEN_RENDERCOMMAND_H
#define ASHEN_RENDERCOMMAND_H

#include "GLEnums.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class RenderState {
    public:
        static void Clear(ClearBuffer buffers = ClearBuffer::Color | ClearBuffer::Depth);

        static void SetClearColor(const Vec4 &color);

        static void SetClearColor(float r, float g, float b, float a);

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void SetViewport(uint32_t width, uint32_t height);

        static void EnableScissor(bool enable);

        static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void EnableDepthTest(bool enable);

        static void SetDepthFunc(DepthFunc func = DepthFunc::Less);

        static void SetDepthWrite(bool enable);

        static void EnableBlending(bool enable);

        static void SetBlendFunc(BlendFactor src = BlendFactor::SrcAlpha,
                                 BlendFactor dst = BlendFactor::OneMinusSrcAlpha);

        static void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha,
                                         BlendFactor dstAlpha);

        static void SetBlendOp(BlendEquation op = BlendEquation::Add);

        static void SetBlendColor(const Vec4 &color);

        static void EnableCulling(bool enable);

        static void SetCullFace(CullFaceMode mode = CullFaceMode::Back);

        static void SetFrontFace(FrontFace orientation = FrontFace::CounterClockwise);

        static void SetPolygonMode(CullFaceMode faces, PolygonMode mode);

        static void SetWireframe(bool enable);

        static void SetPointSize(float size);

        static void SetLineWidth(float width);

        static void EnablePolygonOffset(bool enable);

        static void SetPolygonOffset(float factor, float units);

        static void EnableStencil(bool enable);

        static void SetStencilFunc(StencilOp func, int ref, uint32_t mask);

        static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass);

        static void SetStencilMask(uint32_t mask);

        static void SetColorMask(bool r, bool g, bool b, bool a);

        static void EnableMultisample(bool enable);

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
}

#endif //ASHEN_RENDERCOMMAND_H
