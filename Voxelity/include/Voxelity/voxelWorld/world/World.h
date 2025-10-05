#ifndef VOXELITY_WORLD_H
#define VOXELITY_WORLD_H

#include <unordered_map>
#include <functional>

#include "Voxelity/voxelWorld/chunk/Chunk.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"
#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"

namespace voxelity {
    class World {
    public:
        World();

        ~World() = default;

        VoxelType getVoxel(int worldX, int worldY, int worldZ) const;

        void setVoxel(int worldX, int worldY, int worldZ, VoxelType type);

        VoxelType getVoxel(const glm::ivec3 &worldPos) const;

        void setVoxel(const glm::ivec3 &worldPos, VoxelType type);

        Chunk *getOrCreateChunk(int x, int y, int z);

        Chunk *getOrCreateChunk(const glm::ivec3 &pos);

        Chunk *getChunk(int x, int y, int z);

        Chunk *getChunk(const glm::ivec3 &pos);

        static ChunkCoord toChunkCoord(int x, int y, int z);

        static ChunkCoord toChunkCoord(const glm::ivec3 &worldPos);

        static glm::ivec3 toLocalCoord(int x, int y, int z);

        static glm::ivec3 toLocalCoord(const glm::ivec3 &position);

        static glm::ivec3 toWorldPos(const glm::ivec3 &chunkCoord, const glm::ivec3 &localPos = {0, 0, 0});

        void clear();

        void forEachChunk(const std::function<void(const ChunkCoord &, Chunk *)> &func);

        void generateArea(const glm::ivec3 &startPos, const glm::ivec3 &endPos);

    private:
        std::unordered_map<ChunkCoord, ash::Scope<Chunk> > m_chunks;

        ash::Scope<ITerrainGenerator> m_generator;
    };
}

#endif //VOXELITY_WORLD_H