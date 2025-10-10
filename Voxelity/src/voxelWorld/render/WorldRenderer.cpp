#include "Voxelity/voxelWorld/render/WorldRenderer.h"

#include "Ashen/GraphicsAPI/RenderState.h"

namespace voxelity {
    WorldRenderer::WorldRenderer(World &world, ash::Camera &camera, ash::ShaderProgram &shader)
        : m_world(world), m_camera(camera), m_shader(shader) {
        m_textureColorPalette.updateFromRegistry();
    }

    WorldRenderer::~WorldRenderer() = default;

    void WorldRenderer::render() {
        setupMatrices();
        bindCommonResources();
        renderOpaquePass();
        renderTransparentPass();
    }

    void WorldRenderer::setupMatrices() {
        const glm::mat4 view = m_camera.GetViewMatrix();
        const glm::mat4 proj = m_camera.GetProjectionMatrix();
        m_viewProjection = proj * view;
    }

    void WorldRenderer::bindCommonResources() const {
        m_shader.Bind();
        m_shader.SetMat4("u_ViewProjection", m_viewProjection);
        glBindTextureUnit(0, m_textureColorPalette.GetTexture().ID());
        m_shader.SetInt("u_ColorTex", 0);
        m_shader.SetFloat("u_ChunkSpacing", m_chunkSpacing);
    }

    void WorldRenderer::renderOpaquePass() const {
        ash::RenderState::SetDepthWrite(true);
        ash::RenderState::EnableBlending(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk && chunk->hasMesh())
                chunk->drawOpaque(m_shader);
        });
    }

    void WorldRenderer::renderTransparentPass() const {
        ash::RenderState::EnableBlending(true);
        ash::RenderState::SetBlendFunc(
            ash::BlendFactor::SrcAlpha,
            ash::BlendFactor::OneMinusSrcAlpha
        );
        ash::RenderState::SetDepthWrite(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk && chunk->hasMesh())
                chunk->drawTransparent(m_shader);
        });

        ash::RenderState::SetDepthWrite(true);
        ash::RenderState::EnableBlending(false);
    }
}