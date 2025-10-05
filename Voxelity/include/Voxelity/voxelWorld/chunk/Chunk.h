#ifndef VOXELITY_CHUNK_H
#define VOXELITY_CHUNK_H

#include "Voxelity/voxelWorld/voxel/VoxelArray.h"
#include "Voxelity/voxelWorld/chunk/ChunkMesh.h"

#include "Ashen/renderer/gfx/Shader.h"

namespace voxelity {
    class World;

    struct ChunkCoord {
        int x, y, z;

        ChunkCoord(const int x = 0, const int y = 0, const int z = 0) : x(x), y(y), z(z) {}
        ChunkCoord(const glm::ivec3& v) : x(v.x), y(v.y), z(v.z) {}

        bool operator==(const ChunkCoord &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const ChunkCoord &other) const {
            return !(*this == other);
        }
    };

    class Chunk {
    public:
        explicit Chunk(ChunkCoord coord);

        VoxelType get(int x, int y, int z) const;

        VoxelType get(const glm::ivec3 &pos) const;

        void set(int x, int y, int z, VoxelType voxel);

        void set(const glm::ivec3 &pos, VoxelType voxel);

        void fill(VoxelType ID);

        void markDirty();

        glm::ivec3 getPosition() const;

        void buildMesh(const World &world);

        void drawOpaque(ash::ShaderProgram &shader) const;

        void drawTransparent(ash::ShaderProgram &shader) const;

        bool isDirty() const { return m_dirty; }

    private:
        ChunkCoord m_position;
        VoxelArray m_storage;

        ChunkMesh m_opaqueMesh;
        ChunkMesh m_transparentMesh;

        bool m_dirty = true;

        static bool isInBounds(int x, int y, int z);
    };
}

namespace std {
    template<>
    struct hash<voxelity::ChunkCoord> {
        size_t operator()(const voxelity::ChunkCoord &coord) const noexcept {
            const size_t h1 = hash<int>{}(coord.x);
            const size_t h2 = hash<int>{}(coord.y);
            const size_t h3 = hash<int>{}(coord.z);
            return h1 ^ h2 << 1 ^ h3 << 2;
        }
    };
}

#endif //VOXELITY_CHUNK_H