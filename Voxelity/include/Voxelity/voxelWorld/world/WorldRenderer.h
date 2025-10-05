#ifndef VOXELITY_WORLDRENDERER_H
#define VOXELITY_WORLDRENDERER_H

#include "Ashen/renderer/Camera.h"
#include "Ashen/renderer/gfx/Shader.h"

#include "Voxelity/voxelWorld/world/TextureColorPalette.h"

namespace voxelity {
    class World;

    class WorldRenderer {
    public:
        WorldRenderer(World &world, pixl::Camera &camera, pixl::ShaderProgram &shader);

        void buildAll() const;

        void renderAll();

        float chunkSpacing = 1.0f;

    private:
        World &m_world;
        pixl::Camera &m_camera;
        pixl::ShaderProgram &m_shader;

        TextureColorPalette m_textureColorpalette;

        glm::mat4 m_viewProjection{};

        void setupMatrices();

        void bindCommonResources() const;

        void renderOpaquePass() const;

        void renderTransparentPass() const;
    };
}


#endif //VOXELITY_WORLDRENDERER_H