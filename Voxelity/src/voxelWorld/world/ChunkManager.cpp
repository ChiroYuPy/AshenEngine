#include "Voxelity/voxelWorld/world/ChunkManager.h"

#include <ranges>

#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Ashen/core/Logger.h"

namespace voxelity {
    ChunkManager::ChunkManager(ash::Scope<ITerrainGenerator> generator)
        : m_generator(std::move(generator)) {
    }

    Chunk* ChunkManager::getChunk(const ChunkCoord& coord) {
        const auto it = m_chunks.find(coord);
        return it != m_chunks.end() ? it->second.get() : nullptr;
    }

    Chunk* ChunkManager::getOrCreateChunk(const ChunkCoord& coord) {
        const auto it = m_chunks.find(coord);
        if (it != m_chunks.end())
            return it->second.get();

        auto newChunk = std::make_unique<Chunk>(coord);
        Chunk* ptr = newChunk.get();
        m_chunks.emplace(coord, std::move(newChunk));
        return ptr;
    }

    void ChunkManager::unloadChunk(const ChunkCoord& coord) {
        m_chunks.erase(coord);
        m_chunksInQueue.erase(coord);
        // Retirer aussi de la liste des chunks nécessitant mesh build
        m_chunksNeedingMesh.erase(coord);
    }

    void ChunkManager::updateLoadedChunks(const glm::vec3& playerPos, const int renderDistance) {
        const glm::ivec3 playerChunk = World::toChunkCoord(playerPos.x, playerPos.y, playerPos.z);

        // Si le joueur a changé de chunk ou la distance de rendu a changé
        if (playerChunk != m_lastPlayerChunk || renderDistance != m_lastRenderDistance) {
            m_lastPlayerChunk = playerChunk;
            m_lastRenderDistance = renderDistance;

            // 1. Déterminer les chunks à charger
            std::vector<ChunkCoord> requiredChunks = getChunksInRadius(playerChunk, renderDistance);
            const std::unordered_set requiredSet(requiredChunks.begin(), requiredChunks.end());

            // 2. Ajouter les nouveaux chunks à la file
            for (const auto& coord : requiredChunks) {
                if (!m_chunks.contains(coord) && !m_chunksInQueue.contains(coord)) {

                    // Calculer la priorité (distance au joueur)
                    const int dx = coord.x - playerChunk.x;
                    const int dy = coord.y - playerChunk.y;
                    const int dz = coord.z - playerChunk.z;
                    const int priority = dx*dx + dy*dy + dz*dz;

                    queueChunkLoad(coord, priority);
                }
            }

            // 3. Décharger les chunks trop éloignés
            std::vector<ChunkCoord> toUnload;
            for (const auto &coord: m_chunks | std::views::keys)
                if (!requiredSet.contains(coord))
                    toUnload.push_back(coord);

            for (const auto& coord : toUnload)
                unloadChunk(coord);
        }
    }

    void ChunkManager::processLoadQueue(const int maxChunksPerFrame) {
        int processed = 0;
        while (!m_loadQueue.empty() && processed < maxChunksPerFrame) {
            auto [coord, priority] = m_loadQueue.top();
            m_loadQueue.pop();
            m_chunksInQueue.erase(coord);

            // Créer et générer le chunk
            Chunk* chunk = getOrCreateChunk(coord);
            generateChunk(chunk);

            // Ajouter à la liste des chunks nécessitant construction de mesh
            m_chunksNeedingMesh.insert(coord);

            processed++;
        }
    }

    void ChunkManager::processMeshBuilds(const World& world, const int maxBuildsPerFrame) {
        int built = 0;
        auto it = m_chunksNeedingMesh.begin();

        while (it != m_chunksNeedingMesh.end() && built < maxBuildsPerFrame) {
            ChunkCoord coord = *it;
            Chunk* chunk = getChunk(coord);

            if (chunk) {
                // Vérifier que les chunks voisins existent avant de construire le mesh
                bool neighborsReady = true;
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dz = -1; dz <= 1; dz++) {
                            if (dx == 0 && dy == 0 && dz == 0) continue;

                            ChunkCoord neighbor = {coord.x + dx, coord.y + dy, coord.z + dz};
                            if (!getChunk(neighbor)) {
                                neighborsReady = false;
                                break;
                            }
                        }
                        if (!neighborsReady) break;
                    }
                    if (!neighborsReady) break;
                }

                if (neighborsReady) {
                    chunk->buildMesh(world);
                    it = m_chunksNeedingMesh.erase(it);
                    built++;
                } else {
                    ++it; // Passer au suivant si les voisins ne sont pas prêts
                }
            } else {
                it = m_chunksNeedingMesh.erase(it);
            }
        }
    }

    void ChunkManager::markChunkForMeshRebuild(const ChunkCoord& coord) {
        m_chunksNeedingMesh.insert(coord);
    }

    void ChunkManager::forEachChunk(const std::function<void(const ChunkCoord&, Chunk*)>& func) {
        for (auto& [coord, chunk] : m_chunks)
            func(coord, chunk.get());
    }

    void ChunkManager::forEachChunkInRadius(const glm::vec3& center, const int radius,
                                            const std::function<void(const ChunkCoord&, Chunk*)>& func) {
        const glm::ivec3 centerChunk = World::toChunkCoord(
            static_cast<int>(center.x),
            static_cast<int>(center.y),
            static_cast<int>(center.z)
        );

        const std::vector<ChunkCoord> coords = getChunksInRadius(centerChunk, radius);
        for (const auto& coord : coords) {
            if (Chunk* chunk = getChunk(coord)) {
                func(coord, chunk);
            }
        }
    }

    void ChunkManager::clear() {
        m_chunks.clear();
        while (!m_loadQueue.empty()) m_loadQueue.pop();
        m_chunksInQueue.clear();
        m_chunksNeedingMesh.clear();
    }

    void ChunkManager::queueChunkLoad(const ChunkCoord& coord, const int priority) {
        if (!m_chunksInQueue.contains(coord)) {
            m_loadQueue.push({coord, priority});
            m_chunksInQueue.insert(coord);
        }
    }

    void ChunkManager::generateChunk(Chunk* chunk) const {
        if (chunk && m_generator) {
            m_generator->generateChunk(*chunk);
            // NE PAS marquer dirty ici - c'est géré par processMeshBuilds
        }
    }

    std::vector<ChunkCoord> ChunkManager::getChunksInRadius(const glm::ivec3& center, const int radius) {
        std::vector<ChunkCoord> result;

        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            for (int y = center.y - radius; y <= center.y + radius; ++y) {
                for (int z = center.z - radius; z <= center.z + radius; ++z) {
                    result.push_back({x, y, z});
                }
            }
        }
        
        return result;
    }
}