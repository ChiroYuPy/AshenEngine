#include "Voxelity/voxelWorld/chunk/Chunk.h"

#include "Ashen/renderer/gfx/Shader.h"
#include "Voxelity/voxelWorld/utils/DirectionUtils.h"
#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {
    Chunk::Chunk(const ChunkCoord coord)
        : m_position(coord) {
    }

    bool Chunk::isInBounds(const int x, const int y, const int z) {
        return x >= 0 && x < VoxelArray::SIZE &&
               y >= 0 && y < VoxelArray::SIZE &&
               z >= 0 && z < VoxelArray::SIZE;
    }

    VoxelType Chunk::get(const int x, const int y, const int z) const {
        if (!isInBounds(x, y, z)) return VoxelID::AIR;
        return m_storage.get(x, y, z);
    }

    VoxelType Chunk::get(const glm::ivec3 &pos) const {
        return get(pos.x, pos.y, pos.z);
    }

    void Chunk::set(const int x, const int y, const int z, const VoxelType voxel) {
        if (!isInBounds(x, y, z)) return;
        m_storage.set(x, y, z, voxel);
        markDirty();
    }

    void Chunk::set(const glm::ivec3 &pos, const VoxelType voxel) {
        set(pos.x, pos.y, pos.z, voxel);
    }

    void Chunk::fill(const VoxelType ID) {
        m_storage.fill(ID);
        markDirty();
    }

    void Chunk::markDirty() {
        m_dirty = true;
    }

    glm::ivec3 Chunk::getPosition() const {
        return {m_position.x, m_position.y, m_position.z};
    }

    void Chunk::buildMesh(const World &world) {
        if (!m_dirty) return;

        std::vector<FaceInstance> opaqueFaces;
        std::vector<FaceInstance> transparentFaces;

        for (int x = 0; x < VoxelArray::SIZE; ++x) {
            for (int y = 0; y < VoxelArray::SIZE; ++y) {
                for (int z = 0; z < VoxelArray::SIZE; ++z) {
                    const VoxelType voxelID = get(x, y, z);

                    if (voxelID == VoxelID::AIR) continue;

                    const RenderMode type = getRenderMode(voxelID);

                    for (uint8_t faceID = 0; faceID < 6; ++faceID) {
                        const CubicDirection dir = DirectionUtils::fromIndex(faceID);
                        const glm::ivec3 offset = DirectionUtils::getOffset(dir);
                        const int nx = x + offset.x, ny = y + offset.y, nz = z + offset.z;

                        VoxelType neighborVoxelID;

                        if (nx >= 0 && ny >= 0 && nz >= 0 &&
                            nx < VoxelArray::SIZE && ny < VoxelArray::SIZE && nz < VoxelArray::SIZE) {
                            neighborVoxelID = get(nx, ny, nz);
                        } else {
                            const int wx = m_position.x * VoxelArray::SIZE + nx;
                            const int wy = m_position.y * VoxelArray::SIZE + ny;
                            const int wz = m_position.z * VoxelArray::SIZE + nz;
                            neighborVoxelID = world.getVoxel(wx, wy, wz);
                        }

                        bool visible = false;
                        const RenderMode neighborType = getRenderMode(neighborVoxelID);

                        if (neighborVoxelID == VoxelID::AIR) {
                            visible = true;
                        } else if (type == RenderMode::OPAQUE && neighborType == RenderMode::TRANSPARENT) {
                            visible = true;
                        } else if (type == RenderMode::TRANSPARENT) {
                            if (neighborType == RenderMode::TRANSPARENT)
                                if (voxelID != neighborVoxelID) {
                                    visible = true;
                                }
                        }

                        if (visible) {
                            auto face = FaceInstance{glm::ivec3(x, y, z), faceID, voxelID};

                            switch (type) {
                                case RenderMode::OPAQUE:
                                    opaqueFaces.push_back(face);
                                    break;
                                case RenderMode::TRANSPARENT:
                                    transparentFaces.push_back(face);
                                    break;
                                default:
                                    opaqueFaces.push_back(face); // fallback
                                    break;
                            }
                        }
                    }
                }
            }
        }

        m_opaqueMesh.uploadInstances(opaqueFaces);
        m_transparentMesh.uploadInstances(transparentFaces);

        m_dirty = false;
    }

    void Chunk::drawOpaque(pixl::ShaderProgram &shader) const {
        shader.SetVec3("u_ChunkPos", glm::vec3(getPosition() * VoxelArray::SIZE));
        m_opaqueMesh.draw();
    }

    void Chunk::drawTransparent(pixl::ShaderProgram &shader) const {
        shader.SetVec3("u_ChunkPos", glm::vec3(getPosition() * VoxelArray::SIZE));
        m_transparentMesh.draw();
    }
}
