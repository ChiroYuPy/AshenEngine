#ifndef VOXELITY_WORLDRENDERER_H
#define VOXELITY_WORLDRENDERER_H

#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/TextureAtlas.h"
#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {
    enum class TextureMode {
        ColorPalette, // 1D texture avec couleurs (actuel)
        Atlas2D, // Atlas 2D classique
        TextureArray // Texture array moderne
    };

    class WorldRenderer {
    public:
        WorldRenderer(World &world, ash::Camera &camera, ash::ShaderProgram &shader);

        ~WorldRenderer();

        void render();

        void setTextureMode(TextureMode mode);

        TextureMode getTextureMode() const { return m_textureMode; }

        ash::ITextureAtlas *getCurrentAtlas() const { return m_currentAtlas; }
        ash::TextureAtlasManager &getAtlasManager() { return m_atlasManager; }

        void setChunkSpacing(const float spacing) { m_chunkSpacing = spacing; }
        [[nodiscard]] float getChunkSpacing() const { return m_chunkSpacing; }

    private:
        World &m_world;
        ash::Camera &m_camera;
        ash::ShaderProgram &m_shader;

        // Gestion des atlas
        ash::TextureAtlasManager m_atlasManager;
        ash::ITextureAtlas *m_currentAtlas = nullptr;
        TextureMode m_textureMode = TextureMode::ColorPalette;

        float m_chunkSpacing = 1.0f;
        glm::mat4 m_viewProjection{};

        void setupMatrices();

        void bindCommonResources() const;

        void renderOpaquePass() const;

        void renderTransparentPass() const;

        void initializeAtlases();
    };
}

#endif