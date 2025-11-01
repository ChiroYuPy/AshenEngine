#include "Ashen/GraphicsAPI/RenderCommand.h"
#include "Ashen/GraphicsAPI/OpenGL/OpenGLRendererAPI.h"

namespace ash {

    Own<RendererAPI> RenderCommand::s_API = nullptr;

    void RenderCommand::Init() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL:
                s_API = MakeOwn<OpenGLRendererAPI>();
                break;

            case RendererAPI::API::None:
            default:
                // No rendering API
                break;
        }
    }

    void RenderCommand::Shutdown() {
        s_API.reset();
    }

    void RenderCommand::Clear(const ClearBuffer buffers) {
        s_API->Clear(buffers);
    }

    void RenderCommand::SetClearColor(const Vec4 &color) {
        s_API->SetClearColor(color);
    }

    void RenderCommand::SetClearColor(const float r, const float g, const float b, const float a) {
        s_API->SetClearColor(r, g, b, a);
    }

    void RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {
        s_API->SetViewport(x, y, width, height);
    }

    void RenderCommand::SetViewport(const uint32_t width, const uint32_t height) {
        s_API->SetViewport(0, 0, width, height);
    }

    void RenderCommand::EnableScissor() {
        s_API->EnableScissor();
    }

    void RenderCommand::DisableScissor() {
        s_API->DisableScissor();
    }

    void RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {
        s_API->SetScissor(x, y, width, height);
    }

    void RenderCommand::EnableDepthTest() {
        s_API->EnableDepthTest();
    }

    void RenderCommand::DisableDepthTest() {
        s_API->DisableDepthTest();
    }

    void RenderCommand::SetDepthFunc(const DepthFunc func) {
        s_API->SetDepthFunc(func);
    }

    void RenderCommand::SetDepthWrite(const bool enable) {
        s_API->SetDepthWrite(enable);
    }

    void RenderCommand::EnableBlending() {
        s_API->EnableBlending();
    }

    void RenderCommand::DisableBlending() {
        s_API->DisableBlending();
    }

    void RenderCommand::SetBlendFunc(const BlendFactor src, const BlendFactor dst) {
        s_API->SetBlendFunc(src, dst);
    }

    void RenderCommand::SetBlendFuncSeparate(const BlendFactor srcRGB, const BlendFactor dstRGB,
        const BlendFactor srcAlpha, const BlendFactor dstAlpha) {
        s_API->SetBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    }

    void RenderCommand::SetBlendOp(const BlendEquation op) {
        s_API->SetBlendOp(op);
    }

    void RenderCommand::SetBlendColor(const Vec4 &color) {
        s_API->SetBlendColor(color);
    }

    void RenderCommand::EnableCulling() {
        s_API->EnableCulling();
    }

    void RenderCommand::DisableCulling() {
        s_API->DisableCulling();
    }

    void RenderCommand::SetCullFace(const CullFaceMode mode) {
        s_API->SetCullFace(mode);
    }

    void RenderCommand::SetFrontFace(const FrontFace orientation) {
        s_API->SetFrontFace(orientation);
    }

    void RenderCommand::SetPolygonMode(const CullFaceMode faces, const PolygonMode mode) {
        s_API->SetPolygonMode(faces, mode);
    }

    void RenderCommand::EnablePolygonOffset() {
        s_API->EnablePolygonOffset();
    }

    void RenderCommand::DisablePolygonOffset() {
        s_API->DisablePolygonOffset();
    }

    void RenderCommand::SetPolygonOffset(const float factor, const float units) {
        s_API->SetPolygonOffset(factor, units);
    }

    void RenderCommand::SetPointSize(const float size) {
        s_API->SetPointSize(size);
    }

    void RenderCommand::SetLineWidth(const float width) {
        s_API->SetLineWidth(width);
    }

    void RenderCommand::EnableStencil() {
        s_API->EnableStencil();
    }

    void RenderCommand::DisableStencil() {
        s_API->DisableStencil();
    }

    void RenderCommand::SetStencilFunc(const StencilOp func, const int ref, const uint32_t mask) {
        s_API->SetStencilFunc(func, ref, mask);
    }

    void RenderCommand::SetStencilOp(const StencilOp sfail, const StencilOp dpfail, const StencilOp dppass) {
        s_API->SetStencilOp(sfail, dpfail, dppass);
    }

    void RenderCommand::SetStencilMask(const uint32_t mask) {
        s_API->SetStencilMask(mask);
    }

    void RenderCommand::SetColorMask(const bool r, const bool g, const bool b, const bool a) {
        s_API->SetColorMask(r, g, b, a);
    }

    void RenderCommand::EnableMultisample() {
        s_API->EnableMultisample();
    }

    void RenderCommand::DisableMultisample() {
        s_API->DisableMultisample();
    }

    void RenderCommand::DrawArrays(const PrimitiveType mode, const int first, const int count) {
        s_API->DrawArrays(mode, first, count);
    }

    void RenderCommand::DrawElements(const PrimitiveType mode, const int count, const IndexType type,
        const void *indices) {
        s_API->DrawElements(mode, count, type, indices);
    }

    void RenderCommand::DrawArraysInstanced(const PrimitiveType mode, const int first, const int count,
        const int instanceCount) {
        s_API->DrawArraysInstanced(mode, first, count, instanceCount);
    }

    void RenderCommand::DrawElementsInstanced(const PrimitiveType mode, const int count, const IndexType type,
        const void *indices, const int instanceCount) {
        s_API->DrawElementsInstanced(mode, count, type, indices, instanceCount);
    }

    bool RenderCommand::IsDepthTestEnabled() {
        return s_API->IsDepthTestEnabled();
    }

    bool RenderCommand::IsBlendingEnabled() {
        return s_API->IsBlendingEnabled();
    }

    bool RenderCommand::IsCullingEnabled() {
        return s_API->IsCullingEnabled();
    }

    bool RenderCommand::IsStencilEnabled() {
        return s_API->IsStencilEnabled();
    }

    bool RenderCommand::IsWireframeEnabled() {
        return s_API->IsWireframeEnabled();
    }

    bool RenderCommand::IsScissorEnabled() {
        return s_API->IsScissorEnabled();
    }
}