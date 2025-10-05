#ifndef VOXELITY_CHUNKMANAGER_H
#define VOXELITY_CHUNKMANAGER_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "Ashen/core/Types.h"
#include "Voxelity/voxelWorld/chunk/Chunk.h"

namespace voxelity {
    class ITerrainGenerator;
    class World;

    struct ChunkLoadRequest {
        ChunkCoord coord;
        int priority;

        bool operator<(const ChunkLoadRequest& other) const {
            return priority > other.priority;
        }
    };

    class ChunkManager {
    public:
        explicit ChunkManager(ash::Scope<ITerrainGenerator> generator);

        Chunk* getChunk(const ChunkCoord& coord);
        Chunk* getOrCreateChunk(const ChunkCoord& coord);
        void unloadChunk(const ChunkCoord& coord);

        void updateLoadedChunks(const glm::vec3& playerPos, int renderDistance);
        void processLoadQueue(int maxChunksPerFrame = 1);

        // NOUVEAU: Traitement séparé de la construction des meshes
        void processMeshBuilds(const World& world, int maxBuildsPerFrame = 2);
        void markChunkForMeshRebuild(const ChunkCoord& coord);

        void forEachChunk(const std::function<void(const ChunkCoord&, Chunk*)>& func);
        void forEachChunkInRadius(const glm::vec3& center, int radius,
                                   const std::function<void(const ChunkCoord&, Chunk*)>& func);

        size_t getLoadedChunkCount() const { return m_chunks.size(); }
        size_t getPendingLoadCount() const { return m_loadQueue.size(); }
        size_t getPendingMeshCount() const { return m_chunksNeedingMesh.size(); }

        void clear();

    private:
        std::unordered_map<ChunkCoord, ash::Scope<Chunk>> m_chunks;
        ash::Scope<ITerrainGenerator> m_generator;

        std::priority_queue<ChunkLoadRequest> m_loadQueue;
        std::unordered_set<ChunkCoord> m_chunksInQueue;

        // NOUVEAU: Liste des chunks qui ont besoin d'un mesh
        std::unordered_set<ChunkCoord> m_chunksNeedingMesh;

        glm::ivec3 m_lastPlayerChunk{0, 0, 0};
        int m_lastRenderDistance = 0;
        
        void queueChunkLoad(const ChunkCoord& coord, int priority);
        void generateChunk(Chunk* chunk) const;

        static std::vector<ChunkCoord> getChunksInRadius(const glm::ivec3& center, int radius);
    };
}

#endif