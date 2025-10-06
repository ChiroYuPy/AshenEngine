#ifndef VOXELITY_WORLD_H
#define VOXELITY_WORLD_H

#include "Ashen/Core/Types.h"

#include "Voxelity/voxelWorld/chunk/Chunk.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"
#include "Voxelity/voxelWorld/world/ChunkManager.h"

namespace voxelity {
    class World {
    public:
        explicit World(ash::Scope<ITerrainGenerator> generator);

        ~World() = default;

        // Accès aux voxels
        VoxelType getVoxel(int worldX, int worldY, int worldZ) const;

        VoxelType getVoxel(const glm::ivec3 &worldPos) const;

        void setVoxel(int worldX, int worldY, int worldZ, VoxelType type);

        void setVoxel(const glm::ivec3 &worldPos, VoxelType type);

        // Accès aux chunks
        Chunk *getChunk(const ChunkCoord &coord) const;

        Chunk *getChunk(int x, int y, int z) const;

        // Gestion du chargement (thread principal)
        void updateLoadedChunks(const glm::vec3 &playerPos, int renderDistance) const;

        // Traitement des résultats asynchrones (thread principal uniquement)
        void processChunkLoading() const;

        void processMeshBuilding() const;

        // Itération sur les chunks
        void forEachChunk(const std::function<void(const ChunkCoord &, Chunk *)> &func) const;

        void forEachChunkInRadius(const glm::vec3 &center, int radius,
                                  const std::function<void(const ChunkCoord &, Chunk *)> &func) const;

        // Conversions de coordonnées
        static ash::IVec3 toChunkCoord(int x, int y, int z);

        static ash::IVec3 toChunkCoord(const glm::ivec3 &worldPos);

        static ash::IVec3 toLocalCoord(int x, int y, int z);

        static ash::IVec3 toLocalCoord(const glm::ivec3 &position);

        static ash::IVec3 toWorldPos(const glm::ivec3 &chunkCoord, const glm::ivec3 &localPos = {0, 0, 0});

        // Statistiques
        size_t getLoadedChunkCount() const;

        size_t getPendingLoadCount() const;

        size_t getPendingMeshCount() const;

        void clear() const;

    private:
        ash::Scope<ChunkManager> m_chunkManager;

        void markNeighborChunksDirty(const ChunkCoord &chunkCoord, const glm::ivec3 &localPos) const;
    };
}

#endif