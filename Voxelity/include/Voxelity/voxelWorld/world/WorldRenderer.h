#ifndef VOXELITY_WORLDRENDERER_H
#define VOXELITY_WORLDRENDERER_H

#include "Ashen/renderer/Camera.h"
#include "Ashen/renderer/gfx/Shader.h"
#include "Voxelity/voxelWorld/world/TextureColorPalette.h"
#include "Voxelity/voxelWorld/world/World.h"
#include <unordered_set>

namespace voxelity {
    // Responsabilité unique : rendu du monde
    class WorldRenderer : public IWorldObserver {
    public:
        WorldRenderer(World& world, ash::Camera& camera, ash::ShaderProgram& shader);
        ~WorldRenderer() override;

        void update(float deltaTime);
        void render();

        void addDirtyChunks(const std::vector<ChunkCoord> &chunks);

        // Configuration
        void setChunkSpacing(float spacing) { m_chunkSpacing = spacing; }
        float getChunkSpacing() const { return m_chunkSpacing; }

        void setMaxMeshBuildsPerFrame(int max) { m_maxMeshBuildsPerFrame = max; }
        int getMaxMeshBuildsPerFrame() const { return m_maxMeshBuildsPerFrame; }

        // IWorldObserver
        void onVoxelChanged(const glm::ivec3& worldPos, VoxelType oldType, VoxelType newType) override;
        void onChunkLoaded(const ChunkCoord& coord) override;
        void onChunkUnloaded(const ChunkCoord& coord) override;

    private:
        World& m_world;
        ash::Camera& m_camera;
        ash::ShaderProgram& m_shader;
        TextureColorPalette m_textureColorPalette;

        float m_chunkSpacing = 1.0f;
        int m_maxMeshBuildsPerFrame = 2;

        // Chunks nécessitant une reconstruction de mesh
        std::unordered_set<ChunkCoord> m_dirtyChunks;

        glm::mat4 m_viewProjection{};

        void processDirtyChunks();
        void setupMatrices();
        void bindCommonResources() const;
        void renderOpaquePass() const;
        void renderTransparentPass() const;
    };
}

#endif