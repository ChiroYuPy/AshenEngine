#include "Voxelity/voxelWorld/chunk/Chunk.h"

#include "Ashen/core/Logger.h"

namespace voxelity {
    Chunk::Chunk(ChunkCoord coord) : m_position(coord) {}

    bool Chunk::isInBounds(int x, int y, int z) {
        return x >= 0 && x < VoxelArray::SIZE &&
               y >= 0 && y < VoxelArray::SIZE &&
               z >= 0 && z < VoxelArray::SIZE;
    }

    VoxelType Chunk::get(int x, int y, int z) const {
        if (!isInBounds(x, y, z)) return VoxelID::AIR;
        std::lock_guard lock(m_storageMutex);
        return m_storage.get(x, y, z);
    }

    VoxelType Chunk::get(const glm::ivec3 &pos) const {
        return get(pos.x, pos.y, pos.z);
    }

    void Chunk::set(int x, int y, int z, VoxelType voxel) {
        if (!isInBounds(x, y, z)) return;
        {
            std::lock_guard lock(m_storageMutex);
            m_storage.set(x, y, z, voxel);
        }
        markDirty();
    }

    void Chunk::set(const glm::ivec3 &pos, VoxelType voxel) {
        set(pos.x, pos.y, pos.z, voxel);
    }

    void Chunk::fill(VoxelType ID) {
        {
            std::lock_guard lock(m_storageMutex);
            m_storage.fill(ID);
        }
        markDirty();
    }

    void Chunk::markDirty() {
        m_dirty = true;
        // Ne pas mettre m_hasMesh à false ici - le mesh sera remplacé lors de l'upload
    }

    glm::ivec3 Chunk::getPosition() const {
        return {m_position.x, m_position.y, m_position.z};
    }

    void Chunk::uploadMesh(const std::vector<FaceInstance>& opaqueFaces,
                          const std::vector<FaceInstance>& transparentFaces) {
        m_opaqueMesh.uploadInstances(opaqueFaces);
        m_transparentMesh.uploadInstances(transparentFaces);
        m_dirty = false;
        m_hasMesh = true;
    }

    void Chunk::drawOpaque(const ash::ShaderProgram &shader) const {
        if (!m_hasMesh) return;
        shader.SetVec3("u_ChunkPos", glm::vec3(getPosition() * VoxelArray::SIZE));
        m_opaqueMesh.draw();
    }

    void Chunk::drawTransparent(const ash::ShaderProgram &shader) const {
        if (!m_hasMesh) return;
        shader.SetVec3("u_ChunkPos", glm::vec3(getPosition() * VoxelArray::SIZE));
        m_transparentMesh.draw();
    }
}
