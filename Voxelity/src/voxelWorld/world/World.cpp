#include "Voxelity/voxelWorld/world/World.h"

#include "Voxelity/voxelWorld/generation/ITerrainGenerator.h"

namespace voxelity {
    World::World(ash::Own<ITerrainGenerator> generator)
        : m_chunkManager(std::make_unique<ChunkManager>(std::move(generator))) {
    }

    VoxelType World::getVoxel(const int worldX, const int worldY, const int worldZ) const {
        const ChunkCoord chunkCoord = toChunkCoord(worldX, worldY, worldZ);
        const ash::IVec3 localPos = toLocalCoord(worldX, worldY, worldZ);

        const Chunk *chunk = m_chunkManager.get()->getChunk(chunkCoord);
        if (chunk)
            return chunk->get(localPos.x, localPos.y, localPos.z);

        return VoxelID::AIR;
    }

    VoxelType World::getVoxel(const ash::IVec3 &worldPos) const {
        return getVoxel(worldPos.x, worldPos.y, worldPos.z);
    }

    void World::setVoxel(const int worldX, const int worldY, const int worldZ, const VoxelType type) {
        const ChunkCoord chunkCoord = toChunkCoord(worldX, worldY, worldZ);
        const ash::IVec3 localPos = toLocalCoord(worldX, worldY, worldZ);

        Chunk *chunk = m_chunkManager->getOrCreateChunk(chunkCoord);
        if (!chunk) return;

        const VoxelType oldType = chunk->get(localPos.x, localPos.y, localPos.z);
        if (oldType == type) return;

        chunk->set(localPos.x, localPos.y, localPos.z, type);

        // Marquer le chunk pour reconstruction de mesh
        m_chunkManager->markChunkForMeshRebuild(chunkCoord);

        // Marquer les chunks voisins si on est sur un bord
        markNeighborChunksDirty(chunkCoord, localPos);
    }

    void World::setVoxel(const ash::IVec3 &worldPos, const VoxelType type) {
        setVoxel(worldPos.x, worldPos.y, worldPos.z, type);
    }

    Chunk *World::getChunk(const ChunkCoord &coord) const {
        return m_chunkManager->getChunk(coord);
    }

    Chunk *World::getChunk(const int x, const int y, const int z) const {
        return m_chunkManager->getChunk(ChunkCoord{x, y, z});
    }

    void World::updateLoadedChunks(const glm::vec3 &playerPos, const int renderDistance) const {
        m_chunkManager->updateLoadedChunks(playerPos, renderDistance);
    }

    void World::processChunkLoading() const {
        // Récupérer les chunks générés par les threads
        m_chunkManager->processCompletedGeneration();
    }

    void World::processMeshBuilding() const {
        // Récupérer les meshes construits par les threads
        m_chunkManager->processCompletedMeshes();
    }

    void World::forEachChunk(const std::function < void(const ChunkCoord &, Chunk *) > &func)
    const
 {
        m_chunkManager->forEachChunk(func);
    }

    void World::forEachChunkInRadius(const glm::vec3 &center, const int radius,
                                     const std::function<void(const ChunkCoord &, Chunk *)> &func) const {
        m_chunkManager->forEachChunkInRadius(center, radius, func);
    }

    ash::IVec3 World::toChunkCoord(const int x, const int y, const int z) {
        auto divFloor = [](const int v) {
            return v >= 0 ? v / VoxelArray::SIZE : (v - VoxelArray::SIZE + 1) / VoxelArray::SIZE;
        };
        return {divFloor(x), divFloor(y), divFloor(z)};
    }

    ash::IVec3 World::toChunkCoord(const ash::IVec3 &worldPos) {
        return toChunkCoord(worldPos.x, worldPos.y, worldPos.z);
    }

    ash::IVec3 World::toLocalCoord(const int x, const int y, const int z) {
        auto mod = [](const int v) {
            return (v % VoxelArray::SIZE + VoxelArray::SIZE) % VoxelArray::SIZE;
        };
        return {mod(x), mod(y), mod(z)};
    }

    ash::IVec3 World::toLocalCoord(const ash::IVec3 &position) {
        return toLocalCoord(position.x, position.y, position.z);
    }

    ash::IVec3 World::toWorldPos(const ash::IVec3 &chunkCoord, const ash::IVec3 &localPos) {
        return {
            chunkCoord.x * VoxelArray::SIZE + localPos.x,
            chunkCoord.y * VoxelArray::SIZE + localPos.y,
            chunkCoord.z * VoxelArray::SIZE + localPos.z
        };
    }

    size_t World::getLoadedChunkCount() const {
        return m_chunkManager->getLoadedChunkCount();
    }

    size_t World::getPendingLoadCount() const {
        return m_chunkManager->getPendingLoadCount();
    }

    size_t World::getPendingMeshCount() const {
        return m_chunkManager->getPendingMeshCount();
    }

    void World::clear() const {
        m_chunkManager->clear();
    }

    void World::markNeighborChunksDirty(const ChunkCoord &chunkCoord, const ash::IVec3 &localPos) const {
        auto checkNeighbor = [&](const int lx, const int ly, const int lz, const int dx, const int dy, const int dz) {
            if ((lx == 0 && dx == -1) || (lx == VoxelArray::SIZE - 1 && dx == 1) ||
                (ly == 0 && dy == -1) || (ly == VoxelArray::SIZE - 1 && dy == 1) ||
                (lz == 0 && dz == -1) || (lz == VoxelArray::SIZE - 1 && dz == 1)) {
                const ChunkCoord neighborCoord = {chunkCoord.x + dx, chunkCoord.y + dy, chunkCoord.z + dz};
                const Chunk *neighbor = getChunk(neighborCoord);
                if (neighbor) {
                    m_chunkManager->markChunkForMeshRebuild(neighborCoord);
                }
            }
        };

        if (localPos.x == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, -1, 0, 0);
        if (localPos.x == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 1, 0, 0);
        if (localPos.y == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, -1, 0);
        if (localPos.y == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 1, 0);
        if (localPos.z == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 0, -1);
        if (localPos.z == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 0, 1);
    }
}