#include "Ashen/Graphics/Rendering/Renderer.h"

#include <glad/glad.h>

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/GraphicsAPI/RenderContext.h"
#include "Ashen/GraphicsAPI/VertexArray.h"

namespace ash {
    Renderer::Statistics Renderer::s_Stats;

    void Renderer::Init() {
        RenderContext::EnableDepthTest(true);
        RenderContext::SetDepthFunc(DepthFunc::Less);
        RenderContext::EnableCulling(true);
        RenderContext::SetCullFace(CullFaceMode::Back);
        RenderContext::SetFrontFace(FrontFace::CounterClockwise);

        Renderer2D::Init();
        Renderer3D::Init();

        Logger::Info("Renderer initialized");
    }

    void Renderer::Shutdown() {
        Renderer2D::Shutdown();
        Renderer3D::Shutdown();
        Logger::Info("Renderer shutdown");
    }

    void Renderer::BeginFrame() {
        RenderContext::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderContext::Clear();
        ResetStats();
    }

    void Renderer::EndFrame() {
    }

    void Renderer::OnWindowResize(const uint32_t width, const uint32_t height) {
        RenderContext::SetViewport(width, height);
    }

    void Renderer::DrawIndexed(const VertexArray &vao, const uint32_t indexCount) {
        vao.Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);

        s_Stats.DrawCalls++;
        s_Stats.Indices += indexCount;
        s_Stats.Triangles += indexCount / 3;
    }

    void Renderer::DrawArrays(const VertexArray &vao, const uint32_t vertexCount, const uint32_t first) {
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, static_cast<GLsizei>(first), static_cast<GLsizei>(vertexCount));

        s_Stats.DrawCalls++;
        s_Stats.Vertices += vertexCount;
        s_Stats.Triangles += vertexCount / 3;
    }

    void Renderer::DrawIndexedInstanced(const VertexArray &vao, const uint32_t indexCount,
                                        const uint32_t instanceCount) {
        vao.Bind();
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                                GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instanceCount));

        s_Stats.DrawCalls++;
        s_Stats.Indices += indexCount * instanceCount;
        s_Stats.Triangles += (indexCount / 3) * instanceCount;
    }

    void Renderer::DrawArraysInstanced(const VertexArray &vao, const uint32_t vertexCount,
                                       const uint32_t instanceCount, const uint32_t first) {
        vao.Bind();
        glDrawArraysInstanced(GL_TRIANGLES, static_cast<GLsizei>(first),
                              static_cast<GLsizei>(vertexCount), static_cast<GLsizei>(instanceCount));

        s_Stats.DrawCalls++;
        s_Stats.Vertices += vertexCount * instanceCount;
        s_Stats.Triangles += (vertexCount / 3) * instanceCount;
    }

    void Renderer::ResetStats() {
        s_Stats.Reset();
    }
}