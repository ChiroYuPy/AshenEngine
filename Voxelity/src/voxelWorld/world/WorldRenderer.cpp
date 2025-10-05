#include "Voxelity/voxelWorld/world/WorldRenderer.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/VoxelityApp.h"
#include "Ashen/renderer/RenderCommand.h"

namespace voxelity {
    WorldRenderer::WorldRenderer(World &world, pixl::Camera &camera, pixl::ShaderProgram &shader)
        : m_world(world), m_camera(camera), m_shader(shader) {
        m_textureColorpalette.updateFromRegistry();
    }

    void WorldRenderer::buildAll() const {
        m_world.forEachChunk([&](const ChunkCoord &, Chunk *chunk) {
            if (chunk)
                chunk->buildMesh(m_world);
        });
    }

    void WorldRenderer::renderAll() {
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

        glBindTextureUnit(0, m_textureColorpalette.GetTexture().ID());
        m_shader.SetInt("u_ColorTex", 0);
        m_shader.SetFloat("u_ChunkSpacing", chunkSpacing);
    }

    void WorldRenderer::renderOpaquePass() const {
        pixl::RenderCommand::SetDepthWrite(true);
        pixl::RenderCommand::EnableBlending(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk)
                chunk->drawOpaque(m_shader);
        });
    }

    void WorldRenderer::renderTransparentPass() const {
        pixl::RenderCommand::EnableBlending(true);
        pixl::RenderCommand::SetBlendFunc(pixl::RenderCommand::BlendFactor::SrcAlpha,
                                          pixl::RenderCommand::BlendFactor::OneMinusSrcAlpha);
        pixl::RenderCommand::SetDepthWrite(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk)
                chunk->drawTransparent(m_shader);
        });

        pixl::RenderCommand::SetDepthWrite(true);
        pixl::RenderCommand::EnableBlending(false);
    }
}