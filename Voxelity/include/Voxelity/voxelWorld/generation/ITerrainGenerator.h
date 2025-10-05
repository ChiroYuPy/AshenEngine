#ifndef VOXELITY_ITERRAINGENERATOR_H
#define VOXELITY_ITERRAINGENERATOR_H

#include "Voxelity/voxelWorld/chunk/Chunk.h"

namespace voxelity {
    class ITerrainGenerator {
    public:
        explicit ITerrainGenerator(const uint32_t seed) : m_seed(seed) {
        }

        virtual ~ITerrainGenerator() = default;

        virtual void generateChunk(Chunk &voxelChunk) = 0;

    protected:
        uint32_t m_seed;

        virtual VoxelType generateVoxel(const glm::ivec3 &worldPos) = 0;
    };
}

#endif //VOXELITY_ITERRAINGENERATOR_H
