#ifndef VOXELITY_WORLDRENDERER_H
#define VOXELITY_WORLDRENDERER_H

#include "Ashen/renderer/Camera.h"
#include "Ashen/renderer/gfx/Shader.h"
#include "Voxelity/voxelWorld/world/TextureColorPalette.h"
#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {

    class WorldRenderer {
    public:
        WorldRenderer(World& world, ash::Camera& camera, ash::ShaderProgram& shader);
        ~WorldRenderer();

        void render();

        // Configuration
        void setChunkSpacing(float spacing) { m_chunkSpacing = spacing; }
        [[nodiscard]] float getChunkSpacing() const { return m_chunkSpacing; }

    private:
        World& m_world;
        ash::Camera& m_camera;
        ash::ShaderProgram& m_shader;
        TextureColorPalette m_textureColorPalette;

        float m_chunkSpacing = 1.0f;
        glm::mat4 m_viewProjection{};

        void setupMatrices();
        void bindCommonResources() const;
        void renderOpaquePass() const;
        void renderTransparentPass() const;
    };
}

#endif