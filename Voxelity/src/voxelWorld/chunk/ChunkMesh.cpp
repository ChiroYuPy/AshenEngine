#include "Voxelity/voxelWorld/chunk/ChunkMesh.h"

#include "Ashen/renderer/Renderer.h"

namespace voxelity {
    ChunkMesh::ChunkMesh() {
        m_vao = std::make_unique<ash::VertexArray>();
        m_instanceBuffer = std::make_shared<ash::VertexBuffer>();

        setupVertexAttributes();
    }

    void ChunkMesh::setupVertexAttributes() const {
        m_vao->Bind();

        ash::VertexBufferLayout layout(sizeof(FaceInstance));

        layout.AddAttribute<unsigned int>(0, 0, 1); // index 0, offset 0, divisor = 1 (instancing)

        m_vao->AddVertexBuffer(m_instanceBuffer, layout);

        m_vao->Unbind();
    }

    void ChunkMesh::uploadInstances(const std::span<const FaceInstance> instances) {
        m_instanceCount = instances.size();
        if (m_instanceCount == 0) return;

        if (instances.size_bytes() > m_instanceBuffer->Size()) {
            m_instanceBuffer->SetData(instances, ash::BufferUsage::Dynamic);
        } else {
            m_instanceBuffer->Update(instances);
        }
    }

    void ChunkMesh::draw() const {
        if (IsEmpty()) return;

        ash::Renderer::DrawArraysInstanced(*m_vao, 6, m_instanceCount, 0);
    }
}