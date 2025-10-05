#include <format>
#include "Voxelity/voxelWorld/generation/FlatTerrainGenerator.h"

#include "Ashen/core/Logger.h"

namespace voxelity {
    VoxelType FlatTerrainGenerator::generateVoxel(const glm::ivec3 &worldPos) {
        if (worldPos.y < HEIGHT - 1) return VoxelID::DIRT;
        if (worldPos.y == HEIGHT - 1) return VoxelID::GRASS;
        return VoxelID::AIR;
    }

    void FlatTerrainGenerator::generateChunk(Chunk &voxelChunk) {
        const glm::ivec3 chunkPos = voxelChunk.getPosition();
        pixl::Logger::info() << std::format("chunkPos: {}, {}, {}", chunkPos.x, chunkPos.y, chunkPos.z);

        for (int y = 0; y < VoxelArray::SIZE; ++y) {
            const int worldY = chunkPos.y * VoxelArray::SIZE + y;
            for (int x = 0; x < VoxelArray::SIZE; ++x) {
                const int worldX = chunkPos.x * VoxelArray::SIZE + x;
                for (int z = 0; z < VoxelArray::SIZE; ++z) {
                    const int worldZ = chunkPos.z * VoxelArray::SIZE + z;
                    glm::ivec3 worldPos(worldX, worldY, worldZ);
                    const VoxelType voxelID = generateVoxel(worldPos);
                    voxelChunk.set({x, y, z}, voxelID);
                }
            }
        }
    }
}
