#ifndef ASHEN_SKYBOXCUBEMAP_H
#define ASHEN_SKYBOXCUBEMAP_H

#include "Ashen/renderer/Renderer.h"
#include "Ashen/renderer/RenderCommand.h"

#include <array>
#include <memory>

#include "Ashen/graphics/skybox/ISkybox.h"
#include "Ashen/core/Types.h"
#include "Ashen/renderer/gfx/Buffer.h"
#include "Ashen/renderer/gfx/Shader.h"
#include "Ashen/renderer/gfx/Texture.h"
#include "Ashen/renderer/gfx/VertexArray.h"

namespace ash {
    class SkyboxCubeMap final : public ISkybox {
    public:
        SkyboxCubeMap(const std::array<std::string, 6> &facesPaths, Ref<ShaderProgram> shader)
            : m_shader(std::move(shader)) {
            setupMesh();
            m_texture = TextureCubeMap::LoadFromFiles(facesPaths);
            m_shader->Bind();
            m_shader->SetInt("skybox", 0);
        }

        void Render(const glm::mat4 &view, const glm::mat4 &projection) override {
            RenderCommand::SetDepthWrite(false);
            RenderCommand::SetDepthFunc(RenderCommand::DepthFunc::LessEqual);

            m_shader->Bind();
            m_shader->SetMat4("uView", glm::mat4(glm::mat3(view)));
            m_shader->SetMat4("uProjection", projection);

            glActiveTexture(GL_TEXTURE0);
            m_texture->Bind();

            Renderer::DrawArrays(*m_vao, 36);

            RenderCommand::SetDepthWrite(true);
            RenderCommand::SetDepthFunc(RenderCommand::DepthFunc::Less);
        }

        [[nodiscard]] Ref<TextureCubeMap> GetTexture() const { return m_texture; }

    private:
        Ref<VertexArray> m_vao;
        Ref<VertexBuffer> m_vbo;
        Ref<TextureCubeMap> m_texture;
        Ref<ShaderProgram> m_shader;

        void setupMesh() {
            constexpr std::array<float, 36 * 3> vertices = {
                -1, 1, -1, -1, -1, -1, 1, -1, -1,
                1, -1, -1, 1, 1, -1, -1, 1, -1,
                -1, -1, 1, -1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, -1, 1, -1, -1, 1,
                -1, 1, -1, -1, 1, 1, -1, -1, 1,
                -1, -1, 1, -1, -1, -1, -1, 1, -1,
                1, 1, 1, 1, 1, -1, 1, -1, -1,
                1, -1, -1, 1, -1, 1, 1, 1, 1,
                -1, 1, 1, -1, 1, -1, 1, 1, -1,
                1, 1, -1, 1, 1, 1, -1, 1, 1,
                -1, -1, -1, -1, -1, 1, 1, -1, 1,
                1, -1, 1, 1, -1, -1, -1, -1, -1
            };

            m_vao = MakeRef<VertexArray>();
            m_vbo = MakeRef<VertexBuffer>(BufferConfig::Static());
            m_vbo->SetData<float>(vertices);

            const VertexBufferLayout layout({
                VertexAttribute::Vec3(0, 0),
            });

            m_vao->AddVertexBuffer(m_vbo, layout);
        }
    };
}

#endif // ASHEN_SKYBOXCUBEMAP_H
