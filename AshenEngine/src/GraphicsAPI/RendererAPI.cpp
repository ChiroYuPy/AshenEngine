#include "Ashen/GraphicsAPI/RendererAPI.h"

#include "Ashen/Core/Logger.h"
#include "Ashen/GraphicsAPI/VertexArray.h"
#include "Ashen/GraphicsAPI/OpenGL/OpenGLRendererAPI.h"

namespace ash {
    void RendererAPI::DrawVertexArray(const Ref<VertexArray>& vertexArray) {
        if (!vertexArray) return;

        const auto& config = vertexArray->GetConfig();

        if (vertexArray->HasIndexBuffer()) {
            DrawElements(
                config.primitiveMode,
                static_cast<int>(vertexArray->GetIndexCount()),
                vertexArray->GetIndexBuffer()->GetIndexType(),
                nullptr
            );
        } else if (vertexArray->HasVertices()) {
            DrawArrays(
                config.primitiveMode,
                0,
                static_cast<int>(vertexArray->GetVertexCount())
            );
        }
    }

    void RendererAPI::DrawVertexArrayInstanced(const Ref<VertexArray>& vertexArray, uint32_t instanceCount) {
        if (!vertexArray || instanceCount == 0) return;

        const auto& config = vertexArray->GetConfig();

        if (vertexArray->HasIndexBuffer()) {
            DrawElementsInstanced(
                config.primitiveMode,
                static_cast<int>(vertexArray->GetIndexCount()),
                vertexArray->GetIndexBuffer()->GetIndexType(),
                nullptr,
                static_cast<int>(instanceCount)
            );
        } else if (vertexArray->HasVertices()) {
            DrawArraysInstanced(
                config.primitiveMode,
                0,
                static_cast<int>(vertexArray->GetVertexCount()),
                static_cast<int>(instanceCount)
            );
        }
    }

    RendererAPI::API RendererAPI::s_API = API::OpenGL;

    Ref<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case API::OpenGL: {
                Logger::Info("Creating OpenGL Renderer API");
                return MakeRef<OpenGLRendererAPI>();
            }

            case API::Vulkan: {
                Logger::Error("Vulkan is not yet supported!");
                return nullptr;
            }

            case API::DirectX12: {
                Logger::Error("DirectX12 is not yet supported!");
                return nullptr;
            }

            case API::None:
            default: {
                Logger::Error("No rendering API selected!");
                return nullptr;
            }
        }
    }
}
