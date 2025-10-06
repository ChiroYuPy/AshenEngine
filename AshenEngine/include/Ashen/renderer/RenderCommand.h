#ifndef ASHEN_RENDERCOMMAND_H
#define ASHEN_RENDERCOMMAND_H

#include <glm/vec4.hpp>

#include "glad/glad.h"
#include "Ashen/math/Math.h"

namespace ash {
    class RenderCommand {
    public:
        enum class DepthFunc : GLenum {
            Less = GL_LESS,
            LessEqual = GL_LEQUAL,
            Equal = GL_EQUAL,
            Always = GL_ALWAYS,
            Greater = GL_GREATER,
            NotEqual = GL_NOTEQUAL,
            GreaterEqual = GL_GEQUAL,
            Never = GL_NEVER
        };

        enum class CullFace : GLenum {
            Back = GL_BACK,
            Front = GL_FRONT,
            FrontAndBack = GL_FRONT_AND_BACK
        };

        enum class FrontFace : GLenum {
            CCW = GL_CCW,
            CW = GL_CW
        };

        enum class BlendFactor : GLenum {
            Zero = GL_ZERO,
            One = GL_ONE,
            SrcColor = GL_SRC_COLOR,
            OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
            DstColor = GL_DST_COLOR,
            OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
            SrcAlpha = GL_SRC_ALPHA,
            OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
            DstAlpha = GL_DST_ALPHA,
            OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA
        };

        enum class BlendOp : GLenum {
            Add = GL_FUNC_ADD,
            Subtract = GL_FUNC_SUBTRACT,
            ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
            Min = GL_MIN,
            Max = GL_MAX
        };

        enum class StencilOp : GLenum {
            Keep = GL_KEEP,
            Zero = GL_ZERO,
            Replace = GL_REPLACE,
            Incr = GL_INCR,
            IncrWrap = GL_INCR_WRAP,
            Decr = GL_DECR,
            DecrWrap = GL_DECR_WRAP,
            Invert = GL_INVERT
        };

        enum class StencilFunc : GLenum {
            Never = GL_NEVER,
            Less = GL_LESS,
            LessEqual = GL_LEQUAL,
            Greater = GL_GREATER,
            GreaterEqual = GL_GEQUAL,
            Equal = GL_EQUAL,
            NotEqual = GL_NOTEQUAL,
            Always = GL_ALWAYS
        };

        static void Clear();

        static void Clear(bool color, bool depth, bool stencil);

        static void SetClearColor(const Vec4 &color);

        static void SetClearColor(float r, float g, float b, float a);

        static void ClearColorBuffer();

        static void ClearDepthBuffer();

        static void ClearStencilBuffer();

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

        static void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB,
                                         BlendFactor srcAlpha, BlendFactor dstAlpha);

        static void SetBlendOp(BlendOp op = BlendOp::Add);

        static void SetBlendColor(const Vec4 &color);

        static void EnableCulling(bool enable);

        static void SetCullFace(CullFace mode = CullFace::Back);

        static void SetFrontFace(FrontFace orientation = FrontFace::CCW);

        static void SetWireframe(bool enable);

        static void SetPointSize(float size);

        static void SetLineWidth(float width);

        static void EnablePolygonOffset(bool enable);

        static void SetPolygonOffset(float factor, float units);

        static void EnableStencil(bool enable);

        static void SetStencilFunc(StencilFunc func, int ref, uint32_t mask);

        static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass);

        static void SetStencilMask(uint32_t mask);

        static void SetColorMask(bool r, bool g, bool b, bool a);

        static void EnableMultisample(bool enable);

        [[nodiscard]] static bool IsDepthTestEnabled() { return s_DepthEnabled; }
        [[nodiscard]] static bool IsBlendingEnabled() { return s_BlendEnabled; }
        [[nodiscard]] static bool IsCullingEnabled() { return s_CullingEnabled; }
        [[nodiscard]] static bool IsStencilEnabled() { return s_StencilEnabled; }
        [[nodiscard]] static bool IsWireframeEnabled() { return s_Wireframe; }
        [[nodiscard]] static bool IsScissorEnabled() { return s_ScissorEnabled; }

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