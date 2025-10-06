#include "Voxelity/voxelWorld/chunk/ChunkMesh.h"

#include "Ashen/core/Logger.h"
#include "Ashen/renderer/Renderer.h"

namespace voxelity {
    ChunkMesh::ChunkMesh() {
        m_vao = std::make_unique<ash::VertexArray>();
        m_instanceBuffer = std::make_shared<ash::VertexBuffer>(ash::BufferConfig::Dynamic());

        setupVertexAttributes();
    }

    void ChunkMesh::setupVertexAttributes() const {
        m_vao->Bind();

        const ash::VertexBufferLayout layout({
            ash::VertexAttribute::UInt(0, 0, 1) // location 0, offset 0, divisor 1
        });

        m_vao->AddVertexBuffer(m_instanceBuffer, layout);
        m_vao->Unbind();
    }

    void ChunkMesh::uploadInstances(const std::span<const FaceInstance> instances) {
        m_instanceCount = instances.size();
        if (m_instanceCount == 0) return;

        if (instances.size_bytes() > m_instanceBuffer->Size()) {
            m_instanceBuffer->SetData(instances);
        } else {
            m_instanceBuffer->Update(instances);
        }
    }

    void ChunkMesh::draw() const {
        if (IsEmpty()) return;

        ash::Renderer::DrawArraysInstanced(*m_vao, 6, m_instanceCount, 0);
    }
}
