#include "Voxelity/voxelWorld/generation/NaturalTerrainGenerator.h"

#include <format>

namespace voxelity {
    // Optimisé : échelles ajustées pour moins de calculs
    constexpr double CONTINENT_SCALE = 0.0008;
    constexpr double ELEVATION_SCALE = 0.015;
    constexpr double DETAIL_SCALE = 0.05;
    constexpr double CAVE_SCALE = 0.03;
    constexpr double ORE_SCALE = 0.1;
    constexpr double TEMPERATURE_SCALE = 0.004;
    constexpr double HUMIDITY_SCALE = 0.003;

    constexpr int HEIGHT = 24;
    constexpr int SEA_LEVEL = HEIGHT;
    constexpr int BEACH_HEIGHT = SEA_LEVEL + 3;
    constexpr int MOUNTAIN_HEIGHT = HEIGHT + 60;

    // Configuration des biomes
    const BiomeData biomeConfigs[] = {
        {VoxelID::SAND, VoxelID::SAND, VoxelID::STONE, true, false, false, 0.0}, // BiomeType::OCEAN
        {VoxelID::SAND, VoxelID::SAND, VoxelID::STONE, false, false, false, 0.0}, // BiomeType::BEACH
        {VoxelID::GRASS, VoxelID::DIRT, VoxelID::STONE, false, true, true, 0.02}, // BiomeType::PLAINS
        {VoxelID::GRASS, VoxelID::DIRT, VoxelID::STONE, false, true, true, 0.08}, // BiomeType::FOREST
        {VoxelID::SAND, VoxelID::SAND, VoxelID::STONE, false, false, true, 0.001}, // BiomeType::DESERT
        {VoxelID::STONE, VoxelID::STONE, VoxelID::STONE, false, false, true, 0.01}, // BiomeType::MOUNTAINS
        {VoxelID::DIRT, VoxelID::DIRT, VoxelID::STONE, true, true, false, 0.03}, // BiomeType::SWAMP
        {VoxelID::DIRT, VoxelID::DIRT, VoxelID::STONE, false, false, true, 0.005} // BiomeType::TUNDRA
    };

    BiomeType NaturalTerrainGenerator::getBiome(const glm::ivec3 &worldPos, const double elevation) {
        double temperature = noise.noise2(worldPos.x * TEMPERATURE_SCALE, worldPos.z * TEMPERATURE_SCALE);
        double humidity = noise.noise2(worldPos.x * HUMIDITY_SCALE + 1000, worldPos.z * HUMIDITY_SCALE + 1000);

        // Normaliser les valeurs entre 0 et 1
        temperature = (temperature + 1.0) * 0.5;
        humidity = (humidity + 1.0) * 0.5;

        // Ajuster la température selon l'altitude
        double altitudeTemp = std::max(0.0, temperature - (elevation - SEA_LEVEL) * 0.01);

        if (elevation < SEA_LEVEL - 5) return BiomeType::OCEAN;
        if (elevation < BEACH_HEIGHT && elevation > SEA_LEVEL - 5) return BiomeType::BEACH;
        if (elevation > MOUNTAIN_HEIGHT - 20) return BiomeType::MOUNTAINS;

        if (altitudeTemp < 0.3) return BiomeType::TUNDRA;
        if (altitudeTemp > 0.7 && humidity < 0.3) return BiomeType::DESERT;
        if (humidity > 0.6 && altitudeTemp > 0.4) return BiomeType::SWAMP;
        if (humidity > 0.5 && altitudeTemp > 0.3) return BiomeType::FOREST;

        return BiomeType::PLAINS;
    }

    double NaturalTerrainGenerator::getCaveNoise(const glm::ivec3 &worldPos) {
        double cave1 = noise.noise3_XYBeforeZ(worldPos.x * CAVE_SCALE, worldPos.y * CAVE_SCALE,
                                              worldPos.z * CAVE_SCALE);
        double cave2 = noise.noise3_XYBeforeZ(worldPos.x * CAVE_SCALE * 1.5 + 500, worldPos.y * CAVE_SCALE * 1.5,
                                              worldPos.z * CAVE_SCALE * 1.5 + 500);
        return std::abs(cave1) + std::abs(cave2);
    }

    VoxelType NaturalTerrainGenerator::getOreType(const glm::ivec3 &worldPos, const double depth) {
        double oreNoise = noise.noise3_XYBeforeZ(worldPos.x * ORE_SCALE, worldPos.y * ORE_SCALE,
                                                 worldPos.z * ORE_SCALE);
        oreNoise = (oreNoise + 1.0) * 0.5; // Normaliser entre 0 et 1

        // Plus on descend, plus les minerais rares apparaissent
        if (depth > 40 && oreNoise > 0.95) return VoxelID::LAVA; // Très rare, très profond
        if (depth > 20 && oreNoise > 0.92) return VoxelID::GLASS; // Cristaux rares
        if (depth > 10 && oreNoise > 0.88) return VoxelID::STONE; // Minerai commun

        return VoxelID::STONE; // Pierre normale
    }

    bool NaturalTerrainGenerator::shouldGenerateTree(const glm::ivec3 &worldPos, const BiomeData &biome) {
        if (!biome.hasTrees) return false;

        const double treeNoise = noise.noise2(worldPos.x * 0.1, worldPos.z * 0.1);
        return (treeNoise + 1.0) * 0.5 < biome.treeChance;
    }

    void NaturalTerrainGenerator::generateTree(Chunk &voxelChunk, const glm::ivec3 &localPos,
                                               const glm::ivec3 &chunkPos) {
        // Générer un arbre simple (tronc + feuilles)
        const int treeHeight = 4 + (rand() % 3); // Hauteur entre 4 et 6
        const glm::ivec3 worldPos = chunkPos * VoxelArray::SIZE + localPos;

        // Tronc
        for (int i = 0; i < treeHeight && localPos.y + i < VoxelArray::SIZE; i++) {
            glm::ivec3 trunkPos = localPos + glm::ivec3(0, i, 0);
            if (trunkPos.y >= 0 && trunkPos.y < VoxelArray::SIZE) {
                voxelChunk.set(trunkPos, VoxelID::WOOD);
            }
        }

        // Feuilles (couronne simple)
        for (int dx = -2; dx <= 2; dx++) {
            for (int dz = -2; dz <= 2; dz++) {
                for (int dy = 0; dy < 3; dy++) {
                    if (abs(dx) + abs(dz) + dy < 4) {
                        // Forme de couronne
                        glm::ivec3 leafPos = localPos + glm::ivec3(dx, treeHeight + dy - 1, dz);
                        if (leafPos.x >= 0 && leafPos.x < VoxelArray::SIZE &&
                            leafPos.y >= 0 && leafPos.y < VoxelArray::SIZE &&
                            leafPos.z >= 0 && leafPos.z < VoxelArray::SIZE) {
                            if (voxelChunk.get(leafPos) == VoxelID::AIR) {
                                voxelChunk.set(leafPos, VoxelID::LEAVES);
                            }
                        }
                    }
                }
            }
        }
    }

    VoxelType NaturalTerrainGenerator::generateVoxel(const glm::ivec3 &worldPos) {
        // Bruit continental pour les grandes formes
        double continentNoise = noise.noise2(worldPos.x * CONTINENT_SCALE, worldPos.z * CONTINENT_SCALE);

        // Bruit d'élévation principal
        double elevationNoise = noise.noise2(worldPos.x * ELEVATION_SCALE, worldPos.z * ELEVATION_SCALE);

        // Bruit de détail pour les variations fines
        double detailNoise = noise.noise2(worldPos.x * DETAIL_SCALE, worldPos.z * DETAIL_SCALE);

        // Combiner les bruits pour l'élévation finale
        double combinedElevation = continentNoise * 30.0 + elevationNoise * 20.0 + detailNoise * 8.0;
        int groundHeight = static_cast<int>(SEA_LEVEL + combinedElevation);

        // Déterminer le biome
        BiomeType biome = getBiome(worldPos, groundHeight);
        const BiomeData &biomeData = biomeConfigs[static_cast<int>(biome)];

        // Gestion des cavernes
        if (worldPos.y < groundHeight - 5 && worldPos.y > 10) {
            double caveValue = getCaveNoise(worldPos);
            if (caveValue < 0.1) {
                // Seuil pour créer des cavernes
                return VoxelID::AIR;
            }
        }

        // Gestion de l'eau
        if (worldPos.y < SEA_LEVEL && worldPos.y >= groundHeight)
            return VoxelID::WATER;

        // Génération des couches de terrain
        if (worldPos.y < groundHeight - 10) {
            // Couche profonde avec minerais
            const double depthFromSurface = groundHeight - worldPos.y;
            if (biomeData.hasOres && depthFromSurface > 5) {
                const VoxelType oreType = getOreType(worldPos, depthFromSurface);
                if (oreType != VoxelID::STONE) return oreType;
            }
            return biomeData.deepBlock;
        }
        if (worldPos.y < groundHeight - 2)
            return biomeData.subSurfaceBlock;

        if (worldPos.y < groundHeight)
            return biomeData.surfaceBlock;

        return VoxelID::AIR;
    }

    void NaturalTerrainGenerator::generateChunk(Chunk &voxelChunk) {
        const glm::ivec3 chunkPos = voxelChunk.getPosition();

        // Optimisation : pré-calculer les hauteurs pour chaque colonne (X,Z)
        int heightMap[VoxelArray::SIZE][VoxelArray::SIZE];
        BiomeType biomeMap[VoxelArray::SIZE][VoxelArray::SIZE];

        for (int x = 0; x < VoxelArray::SIZE; ++x) {
            const int worldX = chunkPos.x * VoxelArray::SIZE + x;
            for (int z = 0; z < VoxelArray::SIZE; ++z) {
                const int worldZ = chunkPos.z * VoxelArray::SIZE + z;

                // Calculer l'élévation une seule fois par colonne
                double continentNoise = noise.noise2(worldX * CONTINENT_SCALE, worldZ * CONTINENT_SCALE);
                double elevationNoise = noise.noise2(worldX * ELEVATION_SCALE, worldZ * ELEVATION_SCALE);
                double detailNoise = noise.noise2(worldX * DETAIL_SCALE, worldZ * DETAIL_SCALE);

                double combinedElevation = continentNoise * 30.0 + elevationNoise * 20.0 + detailNoise * 8.0;
                int groundHeight = static_cast<int>(SEA_LEVEL + combinedElevation);

                heightMap[x][z] = groundHeight;
                biomeMap[x][z] = getBiome(glm::ivec3(worldX, groundHeight, worldZ), groundHeight);
            }
        }

        // Première passe : génération du terrain de base (optimisée)
        for (int y = 0; y < VoxelArray::SIZE; ++y) {
            const int worldY = chunkPos.y * VoxelArray::SIZE + y;
            for (int x = 0; x < VoxelArray::SIZE; ++x) {
                const int worldX = chunkPos.x * VoxelArray::SIZE + x;
                for (int z = 0; z < VoxelArray::SIZE; ++z) {
                    const int worldZ = chunkPos.z * VoxelArray::SIZE + z;
                    const int groundHeight = heightMap[x][z];
                    const BiomeType biome = biomeMap[x][z];
                    const BiomeData &biomeData = biomeConfigs[static_cast<int>(biome)];

                    VoxelType voxelID = VoxelID::AIR;

                    // Génération optimisée par hauteur
                    if (worldY < groundHeight - 10) {
                        // Cavernes
                        if (worldY > 10) {
                            glm::ivec3 worldPos(worldX, worldY, worldZ);
                            double caveValue = getCaveNoise(worldPos);
                            if (caveValue < 0.1) {
                                voxelID = VoxelID::AIR;
                            } else {
                                voxelID = biomeData.deepBlock;
                            }
                        } else {
                            voxelID = biomeData.deepBlock;
                        }
                    } else if (worldY < groundHeight - 2) {
                        voxelID = biomeData.subSurfaceBlock;
                    } else if (worldY < groundHeight) {
                        voxelID = biomeData.surfaceBlock;
                    } else if (worldY < SEA_LEVEL) {
                        voxelID = VoxelID::WATER;
                    }

                    voxelChunk.set({x, y, z}, voxelID);
                }
            }
        }

        // Deuxième passe : génération des structures (arbres) - optimisée
        for (int x = 0; x < VoxelArray::SIZE; ++x) {
            for (int z = 0; z < VoxelArray::SIZE; ++z) {
                const int groundHeight = heightMap[x][z];
                const int worldY = chunkPos.y * VoxelArray::SIZE;

                // Vérifier si la surface est dans ce chunk
                const int surfaceLocalY = groundHeight - worldY;
                if (surfaceLocalY >= 0 && surfaceLocalY < VoxelArray::SIZE - 6) {
                    const BiomeType biome = biomeMap[x][z];
                    const BiomeData &biomeData = biomeConfigs[static_cast<int>(biome)];

                    const int worldX = chunkPos.x * VoxelArray::SIZE + x;
                    const int worldZ = chunkPos.z * VoxelArray::SIZE + z;
                    glm::ivec3 worldPos(worldX, groundHeight, worldZ);

                    // Vérifier si on doit générer un arbre
                    if (shouldGenerateTree(worldPos, biomeData)) {
                        // Vérifier l'espace disponible
                        bool canPlaceTree = true;
                        for (int checkY = 1; checkY <= 6 && canPlaceTree; checkY++) {
                            if (surfaceLocalY + checkY >= VoxelArray::SIZE) {
                                canPlaceTree = false;
                            } else if (voxelChunk.get({x, surfaceLocalY + checkY, z}) != VoxelID::AIR) {
                                canPlaceTree = false;
                            }
                        }

                        if (canPlaceTree) {
                            generateTree(voxelChunk, {x, surfaceLocalY + 1, z}, chunkPos);
                        }
                    }
                }
            }
        }
    }
}