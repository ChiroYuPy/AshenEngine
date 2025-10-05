#ifndef VOXELITY_CHUNKMESH_H
#define VOXELITY_CHUNKMESH_H

#include "Ashen/core/Types.h"
#include "Ashen/renderer/gfx/VertexArray.h"
#include "Ashen/renderer/gfx/Buffer.h"

namespace voxelity {
    struct FaceInstance {
        union {
            uint32_t data = 0;

            struct {
                uint32_t x: 5;
                uint32_t y: 5;
                uint32_t z: 5;
                uint32_t faceID: 3;
                uint32_t voxelID: 8;
                uint32_t _unused: 6;
            };
        };

        FaceInstance() = default;

        FaceInstance(const uint8_t x, const uint8_t y, const uint8_t z, const uint8_t faceID, const uint8_t voxelID) {
            set(x, y, z, faceID, voxelID);
        }

        explicit FaceInstance(const glm::ivec3 &pos, const uint8_t faceID, const uint8_t voxelID)
            : FaceInstance(pos.x, pos.y, pos.z, faceID, voxelID) {
        }

        void set(const uint8_t _x, const uint8_t _y, const uint8_t _z, const uint8_t f, const uint8_t v) {
            data = 0;
            x = _x & 0x1F;
            y = _y & 0x1F;
            z = _z & 0x1F;
            faceID = f & 0x07;
            voxelID = v & 0xFF;
            _unused = 0;
        }
    };

    class ChunkMesh {
    public:
        ChunkMesh();

        ~ChunkMesh() = default;

        ChunkMesh(const ChunkMesh &) = delete;

        ChunkMesh &operator=(const ChunkMesh &) = delete;

        void uploadInstances(std::span<const FaceInstance> instances);

        void draw() const;

        [[nodiscard]] size_t getInstanceCount() const { return m_instanceCount; }
        [[nodiscard]] bool IsEmpty() const { return m_instanceCount == 0; }

    private:
        void setupVertexAttributes() const;

        pixl::Ref<pixl::VertexArray> m_vao;
        pixl::Ref<pixl::VertexBuffer> m_instanceBuffer;
        size_t m_instanceCount = 0;
    };
}

#endif //VOXELITY_CHUNKMESH_H