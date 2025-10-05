#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/chunk/Chunk.h"
#include "Voxelity/voxelWorld/generation/NaturalTerrainGenerator.h"

#include <ranges>

#include "Ashen/core/Logger.h"

namespace voxelity {
    World::World() {
        m_generator = std::make_unique<NaturalTerrainGenerator>(0);
    }

    VoxelType World::getVoxel(const int worldX, const int worldY, const int worldZ) const {
        const ChunkCoord chunkCoord = toChunkCoord(worldX, worldY, worldZ);
        const glm::ivec3 localPos = toLocalCoord(worldX, worldY, worldZ);

        const auto it = m_chunks.find(chunkCoord);
        if (it != m_chunks.end())
            return it->second->get(localPos.x, localPos.y, localPos.z);

        return VoxelID::AIR; // Air par défaut
    }

    void World::setVoxel(const int worldX, const int worldY, const int worldZ, const VoxelType ID) {
        const ChunkCoord chunkCoord = toChunkCoord(worldX, worldY, worldZ);
        const glm::ivec3 localPos = toLocalCoord(worldX, worldY, worldZ);

        Chunk *chunk = getOrCreateChunk(chunkCoord.x, chunkCoord.y, chunkCoord.z);
        if (!chunk) return;

        chunk->set(localPos.x, localPos.y, localPos.z, ID);
        chunk->markDirty();

        // --- Vérifier les bords et marquer les chunks voisins ---
        auto checkNeighbor = [&](const int lx, const int ly, const int lz, const int dx, const int dy, const int dz) {
            if (lx == 0 && dx == -1 || lx == VoxelArray::SIZE - 1 && dx == 1 ||
                ly == 0 && dy == -1 || ly == VoxelArray::SIZE - 1 && dy == 1 ||
                lz == 0 && dz == -1 || lz == VoxelArray::SIZE - 1 && dz == 1) {
                Chunk *neighbor = getChunk(chunkCoord.x + dx, chunkCoord.y + dy, chunkCoord.z + dz);

                if (neighbor)
                    neighbor->markDirty();
            }
        };

        // Vérifier chaque axe
        if (localPos.x == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, -1, 0, 0);
        if (localPos.x == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 1, 0, 0);
        if (localPos.y == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, -1, 0);
        if (localPos.y == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 1, 0);
        if (localPos.z == 0) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 0, -1);
        if (localPos.z == VoxelArray::SIZE - 1) checkNeighbor(localPos.x, localPos.y, localPos.z, 0, 0, 1);
    }

    VoxelType World::getVoxel(const glm::ivec3 &worldPos) const {
        return getVoxel(worldPos.x, worldPos.y, worldPos.z);
    }


    void World::setVoxel(const glm::ivec3 &worldPos, const VoxelType type) {
        setVoxel(worldPos.x, worldPos.y, worldPos.z, type);
    }

    Chunk *World::getOrCreateChunk(const int x, const int y, const int z) {
        auto chunkCoord = ChunkCoord{x, y, z};
        auto it = m_chunks.find(chunkCoord);
        if (it != m_chunks.end())
            return it->second.get();

        auto newChunk = std::make_unique<Chunk>(chunkCoord);
        Chunk *ptr = newChunk.get();
        m_chunks.emplace(chunkCoord, std::move(newChunk));
        return ptr;
    }

    Chunk *World::getOrCreateChunk(const glm::ivec3 &pos) {
        return getOrCreateChunk(pos.x, pos.y, pos.z);
    }

    Chunk *World::getChunk(const int x, const int y, const int z) {
        const auto chunkCoord = ChunkCoord{x, y, z};
        const auto it = m_chunks.find(chunkCoord);
        if (it != m_chunks.end())
            return it->second.get();

        return nullptr;
    }

    Chunk *World::getChunk(const glm::ivec3 &pos) {
        return getChunk(pos.x, pos.y, pos.z);
    }

    void World::clear() {
        m_chunks.clear();
    }

    ChunkCoord World::toChunkCoord(const int x, const int y, const int z) {
        auto divFloor = [](const int v) {
            return v >= 0 ? v / VoxelArray::SIZE : (v - VoxelArray::SIZE + 1) / VoxelArray::SIZE;
        };
        return ChunkCoord{divFloor(x), divFloor(y), divFloor(z)};
    }

    ChunkCoord World::toChunkCoord(const glm::ivec3 &worldPos) {
        return toChunkCoord(worldPos.x, worldPos.y, worldPos.z);
    }

    glm::ivec3 World::toLocalCoord(const int x, const int y, const int z) {
        auto mod = [](const int v) {
            return (v % VoxelArray::SIZE + VoxelArray::SIZE) % VoxelArray::SIZE;
        };
        return {mod(x), mod(y), mod(z)};
    }

    glm::ivec3 World::toLocalCoord(const glm::ivec3 &position) {
        return toLocalCoord(position.x, position.y, position.z);
    }

    glm::ivec3 World::toWorldPos(const glm::ivec3 &chunkCoord, const glm::ivec3 &localPos) {
        return {
            chunkCoord.x * VoxelArray::SIZE + localPos.x,
            chunkCoord.y * VoxelArray::SIZE + localPos.y,
            chunkCoord.z * VoxelArray::SIZE + localPos.z
        };
    }

    void World::forEachChunk(const std::function<void(const ChunkCoord &, Chunk *)> &func) {
        for (auto &[coord, chunk]: m_chunks)
            func(coord, chunk.get());
    }

    void World::generateArea(const glm::ivec3 &startPos, const glm::ivec3 &endPos) {
        for (const auto &chunk: m_chunks | std::views::values)
            chunk->fill(VoxelID::AIR);

        const int total = (endPos.x - startPos.x + 1) *
                          (endPos.y - startPos.y + 1) *
                          (endPos.z - startPos.z + 1);

        int current = 0;
        for (int cx = startPos.x; cx <= endPos.x; ++cx)
            for (int cy = startPos.y; cy <= endPos.y; ++cy)
                for (int cz = startPos.z; cz <= endPos.z; ++cz) {
                    Chunk *chunk = getOrCreateChunk(cx, cy, cz);
                    m_generator->generateChunk(*chunk);
                    ++current;

                    const float progress = static_cast<float>(current) / static_cast<float>(total);
                    pixl::Logger::info() << "Generation Percent: " << progress * 100 << "%";
                }
        pixl::Logger::info();
    }
}
