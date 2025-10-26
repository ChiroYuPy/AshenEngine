#ifndef ASHEN_RENDERSTATE_H
#define ASHEN_RENDERSTATE_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "GLEnums.h"

namespace ash {
    // ========== Render State Manager ==========
    // Modern state management with caching to minimize GL calls

    class RenderState {
    public:
        // ========== State Structure ==========
        struct State {
            // Depth
            bool depthTest = false;
            bool depthWrite = true;
            DepthFunc depthFunc = DepthFunc::Less;

            // Blending
            bool blending = false;
            BlendFactor srcBlend = BlendFactor::SrcAlpha;
            BlendFactor dstBlend = BlendFactor::OneMinusSrcAlpha;
            BlendFactor srcBlendAlpha = BlendFactor::One;
            BlendFactor dstBlendAlpha = BlendFactor::Zero;
            BlendEquation blendEq = BlendEquation::Add;
            BlendEquation blendEqAlpha = BlendEquation::Add;
            Vec4 blendColor{0.0f};

            // Culling
            bool culling = false;
            CullFaceMode cullFace = CullFaceMode::Back;
            FrontFace frontFace = FrontFace::CounterClockwise;

            // Stencil
            bool stencilTest = false;
            StencilOp stencilFunc = StencilOp::Always;
            int stencilRef = 0;
            u32 stencilMask = 0xFFFFFFFF;
            StencilOp stencilFail = StencilOp::Keep;
            StencilOp stencilDepthFail = StencilOp::Keep;
            StencilOp stencilPass = StencilOp::Keep;
            u32 stencilWriteMask = 0xFFFFFFFF;

            // Color mask
            bool colorMaskR = true;
            bool colorMaskG = true;
            bool colorMaskB = true;
            bool colorMaskA = true;

            // Viewport & Scissor
            IVec4 viewport{0, 0, 1280, 720};
            bool scissorTest = false;
            IVec4 scissorRect{0, 0, 1280, 720};

            // Polygon
            PolygonMode polygonMode = PolygonMode::Fill;
            bool polygonOffsetFill = false;
            bool polygonOffsetLine = false;
            float polygonOffsetFactor = 0.0f;
            float polygonOffsetUnits = 0.0f;

            // Line & Point
            float lineWidth = 1.0f;
            float pointSize = 1.0f;

            // Misc
            bool multisample = true;
            Vec4 clearColor{0.1f, 0.1f, 0.1f, 1.0f};

            bool operator==(const State &other) const = default;

            bool operator!=(const State &other) const = default;
        };

        // ========== Initialization ==========
        static void Init();

        static void Shutdown();

        // ========== State Management ==========
        static void SetState(const State &state);

        static const State &GetState();

        static void ResetToDefault();

        // Save/Restore state stack
        static void PushState();

        static void PopState();

        // ========== Individual State Setters (with caching) ==========

        // Depth
        static void SetDepthTest(bool enabled);

        static void SetDepthWrite(bool enabled);

        static void SetDepthFunc(DepthFunc func);

        // Blending
        static void SetBlending(bool enabled);

        static void SetBlendFunc(BlendFactor src, BlendFactor dst);

        static void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB,
                                         BlendFactor srcAlpha, BlendFactor dstAlpha);

        static void SetBlendEquation(BlendEquation eq);

        static void SetBlendEquationSeparate(BlendEquation rgb, BlendEquation alpha);

        static void SetBlendColor(const Vec4 &color);

        // Culling
        static void SetCulling(bool enabled);

        static void SetCullFace(CullFaceMode mode);

        static void SetFrontFace(FrontFace orientation);

        // Stencil
        static void SetStencilTest(bool enabled);

        static void SetStencilFunc(StencilOp func, int ref, u32 mask);

        static void SetStencilOp(StencilOp fail, StencilOp depthFail, StencilOp pass);

        static void SetStencilMask(u32 mask);

        // Color mask
        static void SetColorMask(bool r, bool g, bool b, bool a);

        // Viewport & Scissor
        static void SetViewport(int x, int y, int width, int height);

        static void SetViewport(int width, int height);

        static void SetViewport(const IVec4 &viewport);

        static void SetScissorTest(bool enabled);

        static void SetScissor(int x, int y, int width, int height);

        static void SetScissor(const IVec4 &rect);

        // Polygon
        static void SetPolygonMode(PolygonMode mode);

        static void SetPolygonOffset(bool enabled, float factor = 0.0f, float units = 0.0f);

        // Line & Point
        static void SetLineWidth(float width);

        static void SetPointSize(float size);

        // Misc
        static void SetMultisample(bool enabled);

        static void SetClearColor(const Vec4 &color);

        static void SetClearColor(float r, float g, float b, float a = 1.0f);

        // ========== Getters ==========
        static bool IsDepthTestEnabled();

        static bool IsBlendingEnabled();

        static bool IsCullingEnabled();

        static bool IsStencilTestEnabled();

        static bool IsScissorTestEnabled();

        static bool IsWireframeMode();

        static DepthFunc GetDepthFunc();

        static IVec4 GetViewport();

        static Vec4 GetClearColor();

        // ========== Utility Functions ==========
        static void Clear(ClearBuffer buffers = ClearBuffer::Color | ClearBuffer::Depth);

        static void ClearColor();

        static void ClearDepth();

        static void ClearStencil();

        // Common state presets
        static State GetDefaultState();

        static State GetOpaqueState();

        static State GetTransparentState();

        static State GetWireframeState();

        static State GetSkyboxState();

        static State Get2DState();

        // ========== Statistics ==========
        struct Statistics {
            u64 stateChanges = 0;
            u64 redundantCalls = 0; // Calls that didn't change state

            void Reset() {
                stateChanges = 0;
                redundantCalls = 0;
            }
        };

        static const Statistics &GetStats();

        static void ResetStats();

    private:
        static State s_CurrentState;
        static Vector<State> s_StateStack;
        static Statistics s_Stats;

        // Internal helpers
        static void ApplyState(const State &newState, const State &oldState);

        static void ApplyDepthState(const State &state);

        static void ApplyBlendState(const State &state);

        static void ApplyCullState(const State &state);

        static void ApplyStencilState(const State &state);

        static void ApplyViewportState(const State &state);

        static void ApplyPolygonState(const State &state);

        static void ApplyMiscState(const State &state);
    };

    // ========== Scoped State Manager ==========
    // RAII-style state management
    class ScopedRenderState {
    public:
        explicit ScopedRenderState(const RenderState::State &state) {
            RenderState::PushState();
            RenderState::SetState(state);
        }

        ~ScopedRenderState() {
            RenderState::PopState();
        }

        ScopedRenderState(const ScopedRenderState &) = delete;

        ScopedRenderState &operator=(const ScopedRenderState &) = delete;
    };

    // ========== State Builders ==========
    // Fluent API for building render states
    class RenderStateBuilder {
    public:
        RenderStateBuilder() = default;

        RenderStateBuilder &WithDepthTest(bool enabled, DepthFunc func = DepthFunc::Less) {
            m_State.depthTest = enabled;
            m_State.depthFunc = func;
            return *this;
        }

        RenderStateBuilder &WithDepthWrite(bool enabled) {
            m_State.depthWrite = enabled;
            return *this;
        }

        RenderStateBuilder &WithBlending(bool enabled) {
            m_State.blending = enabled;
            return *this;
        }

        RenderStateBuilder &WithBlendFunc(BlendFactor src, BlendFactor dst) {
            m_State.srcBlend = src;
            m_State.dstBlend = dst;
            return *this;
        }

        RenderStateBuilder &WithAlphaBlending() {
            m_State.blending = true;
            m_State.srcBlend = BlendFactor::SrcAlpha;
            m_State.dstBlend = BlendFactor::OneMinusSrcAlpha;
            return *this;
        }

        RenderStateBuilder &WithAdditiveBlending() {
            m_State.blending = true;
            m_State.srcBlend = BlendFactor::SrcAlpha;
            m_State.dstBlend = BlendFactor::One;
            return *this;
        }

        RenderStateBuilder &WithCulling(bool enabled, CullFaceMode mode = CullFaceMode::Back) {
            m_State.culling = enabled;
            m_State.cullFace = mode;
            return *this;
        }

        RenderStateBuilder &WithWireframe(bool enabled) {
            m_State.polygonMode = enabled ? PolygonMode::Line : PolygonMode::Fill;
            return *this;
        }

        RenderStateBuilder &WithColorMask(bool r, bool g, bool b, bool a) {
            m_State.colorMaskR = r;
            m_State.colorMaskG = g;
            m_State.colorMaskB = b;
            m_State.colorMaskA = a;
            return *this;
        }

        RenderStateBuilder &WithViewport(int x, int y, int width, int height) {
            m_State.viewport = IVec4(x, y, width, height);
            return *this;
        }

        RenderStateBuilder &WithLineWidth(float width) {
            m_State.lineWidth = width;
            return *this;
        }

        [[nodiscard]] RenderState::State Build() const {
            return m_State;
        }

        explicit operator RenderState::State() const {
            return m_State;
        }

    private:
        RenderState::State m_State;
    };
} // namespace ash

#endif // ASHEN_RENDERSTATE_H