#ifndef VOXELITY_NATURALTERRAINGENERATOR_H
#define VOXELITY_NATURALTERRAINGENERATOR_H

#include "OpenSimplex2S.hpp"

#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"

namespace voxelity {
    enum class BiomeType : int {
        OCEAN = 0,
        BEACH = 1,
        PLAINS = 2,
        FOREST = 3,
        DESERT = 4,
        MOUNTAINS = 5,
        SWAMP = 6,
        TUNDRA = 7
    };

    struct BiomeData {
        VoxelType surfaceBlock;
        VoxelType subSurfaceBlock;
        VoxelType deepBlock;
        bool hasWater;
        bool hasTrees;
        bool hasOres;
        double treeChance;
    };

    class NaturalTerrainGenerator final : public ITerrainGenerator {
        OpenSimplex2S noise;

        BiomeType getBiome(const glm::ivec3 &worldPos, double elevation);

        double getCaveNoise(const glm::ivec3 &worldPos);

        VoxelType getOreType(const glm::ivec3 &worldPos, double depth);

        bool shouldGenerateTree(const glm::ivec3 &worldPos, const BiomeData &biome);

        static void generateTree(Chunk &voxelChunk, const glm::ivec3 &localPos, const glm::ivec3 &chunkPos);

    public:
        explicit NaturalTerrainGenerator(const uint32_t seed) : ITerrainGenerator(seed) {
        }

        VoxelType generateVoxel(const glm::ivec3 &worldPos) override;

        void generateChunk(Chunk &voxelChunk) override;
    };
}

#endif //VOXELITY_NATURALTERRAINGENERATOR_H