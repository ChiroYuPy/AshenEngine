#include "Ashen/Graphics/Rendering/Renderer.h"

#include <glad/glad.h>

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/GraphicsAPI/RenderCommand.h"
#include "Ashen/GraphicsAPI/VertexArray.h"

namespace ash {
    Renderer::Statistics Renderer::s_Stats;

    void Renderer::Init() {
        RenderCommand::EnableDepthTest();
        RenderCommand::SetDepthFunc(DepthFunc::Less);
        RenderCommand::EnableCulling();
        RenderCommand::SetCullFace(CullFaceMode::Back);
        RenderCommand::SetFrontFace(FrontFace::CounterClockwise);

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
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderCommand::Clear();
        ResetStats();
    }

    void Renderer::EndFrame() {
        //TODO post-process, etc...
    }

    void Renderer::OnWindowResize(const uint32_t width, const uint32_t height) {
        RenderCommand::SetViewport(width, height);
    }

    void Renderer::Draw(const VertexArray &vao) {
        if (vao.HasIndexBuffer()) {
            DrawIndexed(vao, vao.GetIndexCount(), 0);
        } else {
            DrawArrays(vao, vao.GetVertexCount(), 0);
        }
    }

    void Renderer::DrawIndexed(const VertexArray &vao, const uint32_t indexCount,
                               const uint32_t indexOffset) {
        vao.Bind();

        const void *offset = reinterpret_cast<const void *>(indexOffset * sizeof(uint32_t));
        RenderCommand::DrawElements(PrimitiveType::Triangles, static_cast<int>(indexCount),
                                    IndexType::UnsignedInt, offset);

        s_Stats.DrawCalls++;
        s_Stats.Indices += indexCount;
        s_Stats.Triangles += indexCount / 3;
    }

    void Renderer::DrawArrays(const VertexArray &vao, const uint32_t vertexCount,
                              const uint32_t vertexOffset) {
        vao.Bind();

        RenderCommand::DrawArrays(PrimitiveType::Triangles, static_cast<int>(vertexOffset),
                                  static_cast<int>(vertexCount));

        s_Stats.DrawCalls++;
        s_Stats.Vertices += vertexCount;
        s_Stats.Triangles += vertexCount / 3;
    }

    void Renderer::DrawInstanced(const VertexArray &vao, const uint32_t instanceCount) {
        if (vao.HasIndexBuffer()) {
            DrawIndexedInstanced(vao, vao.GetIndexCount(), instanceCount, 0);
        } else {
            vao.Bind();
            RenderCommand::DrawArraysInstanced(PrimitiveType::Triangles, 0,
                                               static_cast<int>(vao.GetVertexCount()),
                                               static_cast<int>(instanceCount));

            s_Stats.DrawCalls++;
            s_Stats.Vertices += vao.GetVertexCount() * instanceCount;
            s_Stats.Triangles += vao.GetVertexCount() / 3 * instanceCount;
        }
    }

    void Renderer::DrawIndexedInstanced(const VertexArray &vao, const uint32_t indexCount,
                                        const uint32_t instanceCount, const uint32_t indexOffset) {
        vao.Bind();

        const void *offset = reinterpret_cast<const void *>(indexOffset * sizeof(uint32_t));
        RenderCommand::DrawElementsInstanced(PrimitiveType::Triangles, static_cast<int>(indexCount),
                                             IndexType::UnsignedInt, offset,
                                             static_cast<int>(instanceCount));

        s_Stats.DrawCalls++;
        s_Stats.Indices += indexCount * instanceCount;
        s_Stats.Triangles += (indexCount / 3) * instanceCount;
    }
}