#ifndef VOXELITY_CHUNK_H
#define VOXELITY_CHUNK_H

#include <atomic>
#include <mutex>
#include "Voxelity/voxelWorld/voxel/VoxelArray.h"
#include "Voxelity/voxelWorld/chunk/ChunkMesh.h"
#include "Ashen/GraphicsAPI/Shader.h"

namespace voxelity {
    class World;

    struct ChunkCoord {
        int x, y, z;

        ChunkCoord(const int x = 0, const int y = 0, const int z = 0) : x(x), y(y), z(z) {
        }

        ChunkCoord(const glm::ivec3 &v) : x(v.x), y(v.y), z(v.z) {
        }

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

        // Lecture thread-safe
        VoxelType get(int x, int y, int z) const;

        VoxelType get(const glm::ivec3 &pos) const;

        // Écriture (à utiliser uniquement depuis le thread principal)
        void set(int x, int y, int z, VoxelType voxel);

        void set(const glm::ivec3 &pos, VoxelType voxel);

        void fill(VoxelType ID);

        void markDirty();

        glm::ivec3 getPosition() const;

        // Upload de mesh (thread principal uniquement - OpenGL)
        void uploadMesh(const std::vector<FaceInstance> &opaqueFaces,
                        const std::vector<FaceInstance> &transparentFaces);

        // Rendu (thread principal uniquement)
        void drawOpaque(const ash::ShaderProgram &shader) const;

        void drawTransparent(const ash::ShaderProgram &shader) const;

        bool isDirty() const { return m_dirty; }
        bool hasMesh() const { return m_hasMesh; }

    private:
        ChunkCoord m_position;
        VoxelArray m_storage;
        mutable std::mutex m_storageMutex; // Pour lecture thread-safe

        ChunkMesh m_opaqueMesh;
        ChunkMesh m_transparentMesh;

        std::atomic<bool> m_dirty{true};
        std::atomic<bool> m_hasMesh{false};

        static bool isInBounds(int x, int y, int z);
    };
}

namespace std {
    template<>
    struct hash<voxelity::ChunkCoord> {
        size_t operator()(const voxelity::ChunkCoord &coord) const noexcept {
            size_t h1 = hash<int>{}(coord.x);
            size_t h2 = hash<int>{}(coord.y);
            size_t h3 = hash<int>{}(coord.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

#endif