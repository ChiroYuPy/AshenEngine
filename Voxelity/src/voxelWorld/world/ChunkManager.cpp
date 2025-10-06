#include "Voxelity/voxelWorld/world/ChunkManager.h"
#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/utils/DirectionUtils.h"
#include "Ashen/core/Logger.h"
#include <ranges>

namespace voxelity {
    ChunkManager::ChunkManager(ash::Scope<ITerrainGenerator> generator, const int threadCount)
        : m_generator(std::move(generator)) {
        // Lancer les threads de génération
        for (int i = 0; i < threadCount; ++i) {
            m_generationThreads.emplace_back(&ChunkManager::generationWorker, this);
            m_meshThreads.emplace_back(&ChunkManager::meshWorker, this);
        }
    }

    ChunkManager::~ChunkManager() {
        shutdown();
    }

    void ChunkManager::shutdown() {
        m_running = false;

        m_generationCV.notify_all();
        m_meshCV.notify_all();

        for (auto &thread: m_generationThreads) {
            if (thread.joinable()) thread.join();
        }
        for (auto &thread: m_meshThreads) {
            if (thread.joinable()) thread.join();
        }
    }

    Chunk *ChunkManager::getChunk(const ChunkCoord &coord) {
        const auto it = m_chunks.find(coord);
        return it != m_chunks.end() ? it->second.get() : nullptr;
    }

    Chunk *ChunkManager::getOrCreateChunk(const ChunkCoord &coord) {
        const auto it = m_chunks.find(coord);
        if (it != m_chunks.end())
            return it->second.get();

        auto newChunk = std::make_unique<Chunk>(coord);
        Chunk *ptr = newChunk.get();
        m_chunks.emplace(coord, std::move(newChunk));
        return ptr;
    }

    void ChunkManager::unloadChunk(const ChunkCoord &coord) {
        m_chunks.erase(coord);
        m_chunksInQueue.erase(coord);
    }

    void ChunkManager::updateLoadedChunks(const glm::vec3 &playerPos, const int renderDistance) {
        glm::ivec3 playerChunk = World::toChunkCoord(playerPos.x, playerPos.y, playerPos.z);

        if (playerChunk != m_lastPlayerChunk || renderDistance != m_lastRenderDistance) {
            m_lastPlayerChunk = playerChunk;
            m_lastRenderDistance = renderDistance;

            std::vector<ChunkCoord> requiredChunks = getChunksInRadius(playerChunk, renderDistance);
            std::unordered_set<ChunkCoord> requiredSet(requiredChunks.begin(), requiredChunks.end());

            // Ajouter nouveaux chunks à générer
            for (const auto &coord: requiredChunks) {
                if (!m_chunks.contains(coord) && !m_chunksInQueue.contains(coord)) {
                    int dx = coord.x - playerChunk.x;
                    int dy = coord.y - playerChunk.y;
                    int dz = coord.z - playerChunk.z;
                    int priority = dx * dx + dy * dy + dz * dz;
                    queueChunkLoad(coord, priority);
                }
            }

            // Décharger chunks éloignés
            std::vector<ChunkCoord> toUnload;
            for (const auto &coord: m_chunks | std::views::keys) {
                if (!requiredSet.contains(coord))
                    toUnload.push_back(coord);
            }

            for (const auto &coord: toUnload)
                unloadChunk(coord);
        }
    }

    void ChunkManager::processCompletedGeneration() {
        std::vector<ChunkCoord> newlyGeneratedChunks; {
            std::lock_guard lock(m_completedGenerationMutex);

            while (!m_completedGeneration.empty()) {
                auto &data = m_completedGeneration.front();

                Chunk *chunk = getOrCreateChunk(data.coord);
                if (chunk && data.voxelData) {
                    // Copier les données générées dans le chunk
                    for (int x = 0; x < VoxelArray::SIZE; ++x) {
                        for (int y = 0; y < VoxelArray::SIZE; ++y) {
                            for (int z = 0; z < VoxelArray::SIZE; ++z) {
                                chunk->set(x, y, z, data.voxelData->get(x, y, z));
                            }
                        }
                    }

                    newlyGeneratedChunks.push_back(data.coord);
                }

                m_completedGeneration.pop();
            }
        }

        if (!newlyGeneratedChunks.empty()) {
            // ash::Logger::info() << "Processed " << newlyGeneratedChunks.size() << " generated chunks";

            // Après avoir ajouté les nouveaux chunks, vérifier tous les chunks qui peuvent maintenant être meshés
            // (y compris ceux qui ont été générés précédemment mais n'avaient pas tous leurs voisins)
            for (const auto &coord: newlyGeneratedChunks) {
                // Calculer la priorité basée sur la distance au dernier chunk du joueur
                int dx = coord.x - m_lastPlayerChunk.x;
                int dy = coord.y - m_lastPlayerChunk.y;
                int dz = coord.z - m_lastPlayerChunk.z;
                int priority = dx * dx + dy * dy + dz * dz;

                // Essayer de mesher le chunk nouvellement généré
                markChunkForMeshRebuild(coord, priority);

                // Essayer de mesher les 6 voisins qui pourraient maintenant avoir tous leurs voisins
                const std::array<ChunkCoord, 6> neighbors = {
                    {
                        {coord.x + 1, coord.y, coord.z},
                        {coord.x - 1, coord.y, coord.z},
                        {coord.x, coord.y + 1, coord.z},
                        {coord.x, coord.y - 1, coord.z},
                        {coord.x, coord.y, coord.z + 1},
                        {coord.x, coord.y, coord.z - 1}
                    }
                };

                for (const auto &neighbor: neighbors) {
                    if (Chunk *neighborChunk = getChunk(neighbor)) {
                        if (neighborChunk->isDirty()) {
                            // Calculer la priorité pour le voisin
                            int ndx = neighbor.x - m_lastPlayerChunk.x;
                            int ndy = neighbor.y - m_lastPlayerChunk.y;
                            int ndz = neighbor.z - m_lastPlayerChunk.z;
                            int neighborPriority = ndx * ndx + ndy * ndy + ndz * ndz;

                            markChunkForMeshRebuild(neighbor, neighborPriority);
                        }
                    }
                }
            }
        }
    }

    void ChunkManager::processCompletedMeshes() {
        std::lock_guard lock(m_completedMeshesMutex);

        int processedCount = 0;
        while (!m_completedMeshes.empty()) {
            auto &[coord, opaqueFaces, transparentFaces] = m_completedMeshes.front();
            if (Chunk *chunk = getChunk(coord)) {
                chunk->uploadMesh(opaqueFaces, transparentFaces);
                processedCount++;
            }

            m_completedMeshes.pop();
        }

        if (processedCount > 0) {
            // ash::Logger::info() << "Uploaded " << processedCount << " chunk meshes";
        }
    }

    void ChunkManager::markChunkForMeshRebuild(const ChunkCoord &coord, int priority) {
        if (!areNeighborsLoaded(coord)) {
            return;
        }

        std::lock_guard lock(m_meshQueueMutex);
        m_meshBuildQueue.push({coord, priority});
        m_meshCV.notify_one();
    }

    void ChunkManager::forEachChunk(const std::function<void(const ChunkCoord &, Chunk *)> &func) {
        for (auto &[coord, chunk]: m_chunks)
            func(coord, chunk.get());
    }

    void ChunkManager::forEachChunkInRadius(const glm::vec3 &center, const int radius,
                                            const std::function<void(const ChunkCoord &, Chunk *)> &func) {
        glm::ivec3 centerChunk = World::toChunkCoord(
            static_cast<int>(center.x),
            static_cast<int>(center.y),
            static_cast<int>(center.z)
        );

        std::vector<ChunkCoord> coords = getChunksInRadius(centerChunk, radius);
        for (const auto &coord: coords) {
            if (Chunk *chunk = getChunk(coord)) {
                func(coord, chunk);
            }
        }
    }

    void ChunkManager::clear() {
        m_chunks.clear(); {
            std::lock_guard lock(m_generationQueueMutex);
            while (!m_generationQueue.empty()) m_generationQueue.pop();
        } {
            std::lock_guard lock(m_meshQueueMutex);
            while (!m_meshBuildQueue.empty()) m_meshBuildQueue.pop();
        }

        m_chunksInQueue.clear();
    }

    size_t ChunkManager::getPendingLoadCount() {
        std::lock_guard lock(m_generationQueueMutex);
        return m_generationQueue.size();
    }

    size_t ChunkManager::getPendingMeshCount() {
        std::lock_guard lock(m_meshQueueMutex);
        return m_meshBuildQueue.size();
    }

    void ChunkManager::queueChunkLoad(const ChunkCoord &coord, const int priority) {
        if (m_chunksInQueue.contains(coord)) return;

        std::lock_guard lock(m_generationQueueMutex);
        m_generationQueue.push({coord, priority});
        m_chunksInQueue.insert(coord);
        m_generationCV.notify_one();
    }

    void ChunkManager::generationWorker() {
        // ash::Logger::info("ChunkManager::generationWorker");
        while (m_running.load()) {
            ChunkLoadRequest request; {
                std::unique_lock lock(m_generationQueueMutex);
                m_generationCV.wait(lock, [this] {
                    return !m_generationQueue.empty() || !m_running;
                });

                if (!m_running) break;
                if (m_generationQueue.empty()) continue;

                request = m_generationQueue.top();
                m_generationQueue.pop();
            }

            // Générer les données (hors mutex)
            auto voxelData = generateChunkData(request.coord);

            // Ajouter aux résultats
            {
                std::lock_guard lock(m_completedGenerationMutex);
                m_completedGeneration.push({request.coord, std::move(voxelData)});
            }
        }
    }

    void ChunkManager::meshWorker() {
        // ash::Logger::info("ChunkManager::meshWorker");
        while (m_running.load()) {
            ChunkCoord coord; {
                std::unique_lock lock(m_meshQueueMutex);
                m_meshCV.wait(lock, [this] {
                    return !m_meshBuildQueue.empty() || !m_running;
                });

                if (!m_running) break;
                if (m_meshBuildQueue.empty()) continue;

                MeshBuildRequest request = m_meshBuildQueue.top();
                m_meshBuildQueue.pop();
                coord = request.coord;
            }

            // Construire le mesh (hors mutex)
            MeshData meshData = buildChunkMesh(coord);

            // Ajouter aux résultats
            {
                std::lock_guard lock(m_completedMeshesMutex);
                m_completedMeshes.push(std::move(meshData));
            }
        }
    }

    std::unique_ptr<VoxelArray> ChunkManager::generateChunkData(const ChunkCoord &coord) const {
        auto voxelData = std::make_unique<VoxelArray>();

        if (m_generator) {
            Chunk tempChunk(coord);
            m_generator->generateChunk(tempChunk);

            // Copier les données
            for (int x = 0; x < VoxelArray::SIZE; ++x) {
                for (int y = 0; y < VoxelArray::SIZE; ++y) {
                    for (int z = 0; z < VoxelArray::SIZE; ++z) {
                        voxelData->set(x, y, z, tempChunk.get(x, y, z));
                    }
                }
            }
        }

        return voxelData;
    }

    MeshData ChunkManager::buildChunkMesh(const ChunkCoord &coord) {
        MeshData meshData;
        meshData.coord = coord;

        const Chunk *chunk = getChunk(coord);
        if (!chunk) return meshData;

        for (int x = 0; x < VoxelArray::SIZE; ++x) {
            for (int y = 0; y < VoxelArray::SIZE; ++y) {
                for (int z = 0; z < VoxelArray::SIZE; ++z) {
                    const VoxelType voxelID = chunk->get(x, y, z);
                    if (voxelID == VoxelID::AIR) continue;

                    const RenderMode type = getRenderMode(voxelID);

                    for (uint8_t faceID = 0; faceID < 6; ++faceID) {
                        const CubicDirection dir = DirectionUtils::fromIndex(faceID);
                        const glm::ivec3 offset = DirectionUtils::getOffset(dir);
                        const int nx = x + offset.x, ny = y + offset.y, nz = z + offset.z;

                        VoxelType neighborVoxelID;
                        if (nx >= 0 && ny >= 0 && nz >= 0 &&
                            nx < VoxelArray::SIZE && ny < VoxelArray::SIZE && nz < VoxelArray::SIZE) {
                            neighborVoxelID = chunk->get(nx, ny, nz);
                        } else {
                            const int wx = coord.x * VoxelArray::SIZE + nx;
                            const int wy = coord.y * VoxelArray::SIZE + ny;
                            const int wz = coord.z * VoxelArray::SIZE + nz;
                            neighborVoxelID = getVoxelSafe(wx, wy, wz);
                        }

                        bool visible = false;
                        const RenderMode neighborType = getRenderMode(neighborVoxelID);

                        if (neighborVoxelID == VoxelID::AIR) {
                            visible = true;
                        } else if (type == RenderMode::OPAQUE && neighborType == RenderMode::TRANSPARENT) {
                            visible = true;
                        } else if (type == RenderMode::TRANSPARENT && neighborType == RenderMode::TRANSPARENT) {
                            if (voxelID != neighborVoxelID) visible = true;
                        }

                        if (visible) {
                            FaceInstance face{glm::ivec3(x, y, z), faceID, voxelID};

                            if (type == RenderMode::TRANSPARENT) {
                                meshData.transparentFaces.push_back(face);
                            } else {
                                meshData.opaqueFaces.push_back(face);
                            }
                        }
                    }
                }
            }
        }

        return meshData;
    }

    bool ChunkManager::areNeighborsLoaded(const ChunkCoord &coord) {
        // Vérifier uniquement les 6 faces adjacentes (pas les diagonales)
        // Cela permet de construire le mesh beaucoup plus rapidement
        const std::array<ChunkCoord, 6> neighbors = {
            {
                {coord.x + 1, coord.y, coord.z},
                {coord.x - 1, coord.y, coord.z},
                {coord.x, coord.y + 1, coord.z},
                {coord.x, coord.y - 1, coord.z},
                {coord.x, coord.y, coord.z + 1},
                {coord.x, coord.y, coord.z - 1}
            }
        };

        for (const auto &neighbor: neighbors) {
            if (!getChunk(neighbor)) return false;
        }
        return true;
    }

    VoxelType ChunkManager::getVoxelSafe(const int worldX, const int worldY, const int worldZ) const {
        ChunkCoord chunkCoord = World::toChunkCoord(worldX, worldY, worldZ);
        glm::ivec3 localPos = World::toLocalCoord(worldX, worldY, worldZ);

        Chunk *chunk = const_cast<ChunkManager *>(this)->getChunk(chunkCoord);
        if (chunk)
            return chunk->get(localPos.x, localPos.y, localPos.z);

        return VoxelID::AIR;
    }

    std::vector<ChunkCoord> ChunkManager::getChunksInRadius(const glm::ivec3 &center, const int radius) {
        std::vector<ChunkCoord> result;
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            for (int y = center.y - radius; y <= center.y + radius; ++y) {
                for (int z = center.z - radius; z <= center.z + radius; ++z) {
                    result.emplace_back(x, y, z);
                }
            }
        }
        return result;
    }
}
