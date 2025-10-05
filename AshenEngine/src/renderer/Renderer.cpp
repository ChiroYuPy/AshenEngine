#include "Ashen/renderer/Renderer.h"

#include <glad/glad.h>
#include "Ashen/core/Logger.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer2D.h"
#include "Ashen/renderer/gfx/VertexArray.h"

namespace pixl {
    Renderer::Statistics Renderer::s_Stats;

    void Renderer::Init() {
        RenderCommand::EnableDepthTest(true);
        RenderCommand::SetDepthFunc(RenderCommand::DepthFunc::Less);
        RenderCommand::EnableCulling(true);
        RenderCommand::SetCullFace(RenderCommand::CullFace::Back);
        RenderCommand::SetFrontFace(RenderCommand::FrontFace::CCW);

        Renderer2D::Init();

        Logger::info("Renderer initialized");
    }

    void Renderer::Shutdown() {
        Renderer2D::Shutdown();
        Logger::info("Renderer shutdown");
    }

    void Renderer::BeginFrame() {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderCommand::Clear();
        ResetStats();
    }

    void Renderer::EndFrame() {
    }

    void Renderer::OnWindowResize(const uint32_t width, const uint32_t height) {
        RenderCommand::SetViewport(width, height);
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