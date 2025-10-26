#include "Voxelity/voxelWorld/render/WorldRenderer.h"
#include "Ashen/GraphicsAPI/RenderCommand.h"

namespace voxelity {
    WorldRenderer::WorldRenderer(World &world, ash::Camera &camera, ash::ShaderProgram &shader)
        : m_world(world), m_camera(camera), m_shader(shader) {
        initializeAtlases();
    }

    WorldRenderer::~WorldRenderer() = default;

    void WorldRenderer::initializeAtlases() {
        auto *colorPalette = m_atlasManager.CreateAtlas<ash::ColorPalette1D>("color_palette");
        colorPalette->UpdateFromRegistry();

        ash::TextureAtlas2D::Config atlas2DConfig;
        atlas2DConfig.tileSize = 16;
        atlas2DConfig.atlasWidth = 256;
        atlas2DConfig.atlasHeight = 256;

        auto *atlas2D = m_atlasManager.CreateAtlas<ash::TextureAtlas2D>("atlas_2d", atlas2DConfig);

        // atlas2D->AddTexture("grass", "textures/grass.png");
        // atlas2D->AddTexture("stone", "textures/stone.png");
        // atlas2D->AddTexture("dirt", "textures/dirt.png");

        atlas2D->AddSolidColor("red", glm::vec4(1, 0, 0, 1));
        atlas2D->AddSolidColor("green", glm::vec4(0, 1, 0, 1));
        atlas2D->AddSolidColor("blue", glm::vec4(0, 0, 1, 1));
        atlas2D->Update();

        ash::TextureArray::Config arrayConfig;
        arrayConfig.layerWidth = 16;
        arrayConfig.layerHeight = 16;
        arrayConfig.maxLayers = 256;

        auto *texArray = m_atlasManager.CreateAtlas<ash::TextureArray>("texture_array", arrayConfig);

        // texArray->AddLayer("grass", "textures/grass.png");
        // texArray->AddLayer("stone", "textures/stone.png");

        texArray->AddSolidColor("white", glm::vec4(1, 1, 1, 1));
        texArray->AddSolidColor("gray", glm::vec4(0.5, 0.5, 0.5, 1));
        texArray->Update();

        m_currentAtlas = colorPalette;
    }

    void WorldRenderer::setTextureMode(const TextureMode mode) {
        m_textureMode = mode;

        switch (mode) {
            case TextureMode::ColorPalette:
                m_currentAtlas = m_atlasManager.GetAtlas("color_palette");
                break;
            case TextureMode::Atlas2D:
                m_currentAtlas = m_atlasManager.GetAtlas("atlas_2d");
                break;
            case TextureMode::TextureArray:
                m_currentAtlas = m_atlasManager.GetAtlas("texture_array");
                break;
        }
    }

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

        if (m_currentAtlas) {
            m_currentAtlas->Bind(0);
            m_shader.SetInt("u_ColorTex", 0);
        }

        m_shader.SetFloat("u_ChunkSpacing", m_chunkSpacing);

        m_shader.SetInt("u_TextureMode", static_cast<int>(m_textureMode));
    }

    void WorldRenderer::renderOpaquePass() const {
        ash::RenderCommand::SetDepthWrite(true);
        ash::RenderCommand::EnableBlending(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk && chunk->hasMesh()) chunk->drawOpaque(m_shader);
        });
    }

    void WorldRenderer::renderTransparentPass() const {
        ash::RenderCommand::EnableBlending(true);
        ash::RenderCommand::SetBlendFunc(ash::BlendFactor::SrcAlpha, ash::BlendFactor::OneMinusSrcAlpha);
        ash::RenderCommand::SetDepthWrite(false);

        m_world.forEachChunk([&](const ChunkCoord &, const Chunk *chunk) {
            if (chunk && chunk->hasMesh()) chunk->drawTransparent(m_shader);
        });

        ash::RenderCommand::SetDepthWrite(true);
        ash::RenderCommand::EnableBlending(false);
    }
}