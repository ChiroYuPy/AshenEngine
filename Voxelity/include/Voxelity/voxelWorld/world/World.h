#ifndef VOXELITY_WORLD_H
#define VOXELITY_WORLD_H

#include "Ashen/core/Types.h"
#include "Voxelity/voxelWorld/chunk/Chunk.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"
#include "Voxelity/voxelWorld/world/ChunkManager.h"

namespace voxelity {
    class IWorldObserver {
    public:
        virtual ~IWorldObserver() = default;
        virtual void onVoxelChanged(const glm::ivec3& worldPos, VoxelType oldType, VoxelType newType) = 0;
        virtual void onChunkLoaded(const ChunkCoord& coord) = 0;
        virtual void onChunkUnloaded(const ChunkCoord& coord) = 0;
    };

    class World {
    public:
        explicit World(ash::Scope<ITerrainGenerator> generator);
        ~World() = default;

        VoxelType getVoxel(int worldX, int worldY, int worldZ) const;
        VoxelType getVoxel(const glm::ivec3& worldPos) const;
        void setVoxel(int worldX, int worldY, int worldZ, VoxelType type);
        void setVoxel(const glm::ivec3& worldPos, VoxelType type);

        Chunk* getChunk(const ChunkCoord& coord) const;
        Chunk* getChunk(int x, int y, int z) const;

        void updateLoadedChunks(const glm::vec3& playerPos, int renderDistance) const;
        void processChunkLoading(int maxChunksPerFrame = 1) const;

        // NOUVEAU: Traitement séparé de la construction des meshes
        void processMeshBuilding(int maxBuildsPerFrame = 2) const;

        void forEachChunk(const std::function<void(const ChunkCoord&, Chunk*)>& func) const;
        void forEachChunkInRadius(const glm::vec3& center, int radius,
                                   const std::function<void(const ChunkCoord&, Chunk*)>& func) const;

        static ash::IVec3 toChunkCoord(int x, int y, int z);
        static ash::IVec3 toChunkCoord(const glm::ivec3& worldPos);
        static ash::IVec3 toLocalCoord(int x, int y, int z);
        static ash::IVec3 toLocalCoord(const glm::ivec3& position);
        static ash::IVec3 toWorldPos(const glm::ivec3& chunkCoord, const glm::ivec3& localPos = {0, 0, 0});

        void addObserver(IWorldObserver* observer);
        void removeObserver(IWorldObserver* observer);

        size_t getLoadedChunkCount() const;
        size_t getPendingLoadCount() const;
        size_t getPendingMeshCount() const; // NOUVEAU

        void clear() const;

    private:
        ash::Scope<ChunkManager> m_chunkManager;
        std::vector<IWorldObserver*> m_observers;

        void notifyVoxelChanged(const glm::ivec3& worldPos, VoxelType oldType, VoxelType newType) const;
        void markNeighborChunksDirty(const ChunkCoord& chunkCoord, const glm::ivec3& localPos);
    };
}

#endif