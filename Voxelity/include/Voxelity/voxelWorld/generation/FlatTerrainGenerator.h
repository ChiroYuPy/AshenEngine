#ifndef VOXELITY_FLATTERRAINGENERATOR_H
#define VOXELITY_FLATTERRAINGENERATOR_H

#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"

namespace voxelity {
    class FlatTerrainGenerator final : public ITerrainGenerator {
    public:
        explicit FlatTerrainGenerator(const uint32_t seed) : ITerrainGenerator(seed) {
        }

        VoxelType generateVoxel(const glm::ivec3 &worldPos) override;

        void generateChunk(Chunk &voxelChunk) override;

    private:
        static constexpr int HEIGHT = 4;
    };
}

#endif //VOXELITY_FLATTERRAINGENERATOR_H
