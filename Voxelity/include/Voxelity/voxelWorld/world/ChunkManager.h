#ifndef VOXELITY_CHUNKMANAGER_H
#define VOXELITY_CHUNKMANAGER_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Ashen/Core/Types.h"

#include "Voxelity/voxelWorld/chunk/Chunk.h"

namespace voxelity {
    class ITerrainGenerator;
    class World;

    struct ChunkLoadRequest {
        ChunkCoord coord;
        int priority;

        bool operator<(const ChunkLoadRequest &other) const {
            return priority > other.priority;
        }
    };

    struct GeneratedChunkData {
        ChunkCoord coord;
        std::unique_ptr<VoxelArray> voxelData;
    };

    struct MeshData {
        ChunkCoord coord;
        std::vector<FaceInstance> opaqueFaces;
        std::vector<FaceInstance> transparentFaces;
    };

    struct MeshBuildRequest {
        ChunkCoord coord;
        int priority;

        bool operator<(const MeshBuildRequest &other) const {
            return priority > other.priority; // Priority queue: smaller priority = higher priority
        }
    };

    class ChunkManager {
    public:
        explicit ChunkManager(ash::Scope<ITerrainGenerator> generator, int threadCount = 2);

        ~ChunkManager();

        ChunkManager(const ChunkManager &) = delete;

        ChunkManager &operator=(const ChunkManager &) = delete;

        Chunk *getChunk(const ChunkCoord &coord);

        Chunk *getOrCreateChunk(const ChunkCoord &coord);

        void unloadChunk(const ChunkCoord &coord);

        void updateLoadedChunks(const glm::vec3 &playerPos, int renderDistance);

        // Thread principal - récupération des résultats asynchrones
        void processCompletedGeneration();

        void processCompletedMeshes();

        void markChunkForMeshRebuild(const ChunkCoord &coord, int priority = 9999);

        void forEachChunk(const std::function<void(const ChunkCoord &, Chunk *)> &func);

        void forEachChunkInRadius(const glm::vec3 &center, int radius,
                                  const std::function<void(const ChunkCoord &, Chunk *)> &func);

        size_t getLoadedChunkCount() const { return m_chunks.size(); }

        size_t getPendingLoadCount();

        size_t getPendingMeshCount();

        void clear();

        void shutdown();

    private:
        // Données principales (thread principal uniquement)
        std::unordered_map<ChunkCoord, ash::Scope<Chunk> > m_chunks;
        ash::Scope<ITerrainGenerator> m_generator;

        // Files thread-safe pour communication inter-threads
        std::priority_queue<ChunkLoadRequest> m_generationQueue;
        std::mutex m_generationQueueMutex;
        std::condition_variable m_generationCV;

        std::priority_queue<MeshBuildRequest> m_meshBuildQueue;
        std::mutex m_meshQueueMutex;
        std::condition_variable m_meshCV;

        std::queue<GeneratedChunkData> m_completedGeneration;
        std::mutex m_completedGenerationMutex;

        std::queue<MeshData> m_completedMeshes;
        std::mutex m_completedMeshesMutex;

        // État
        std::unordered_set<ChunkCoord> m_chunksInQueue;
        std::atomic<bool> m_running{true};

        glm::ivec3 m_lastPlayerChunk{0, 0, 0};
        int m_lastRenderDistance = 0;

        // Threads de travail
        std::vector<std::thread> m_generationThreads;
        std::vector<std::thread> m_meshThreads;

        // Fonctions de travail des threads
        void generationWorker();

        void meshWorker();

        void queueChunkLoad(const ChunkCoord &coord, int priority);

        static std::vector<ChunkCoord> getChunksInRadius(const glm::ivec3 &center, int radius);

        // Génération et construction de mesh (appelées depuis les threads)
        std::unique_ptr<VoxelArray> generateChunkData(const ChunkCoord &coord) const;

        MeshData buildChunkMesh(const ChunkCoord &coord);

        // Helper pour vérifier les voisins (thread-safe)
        bool areNeighborsLoaded(const ChunkCoord &coord);

        VoxelType getVoxelSafe(int worldX, int worldY, int worldZ) const;
    };
}

#endif