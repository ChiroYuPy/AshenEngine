#include "Ashen/Graphics/Objects/Mesh.h"

namespace ash {
    // ========== Mesh ==========

    void Mesh::SetData(const VertexData& vertexData, const std::vector<uint32_t>& indices) {
        m_Attributes = vertexData.attributes;
        m_VertexCount = vertexData.vertexCount;
        m_IndexCount = indices.size();

        // 1. Créer le VAO EN PREMIER
        m_VAO = VertexArray(VertexArrayConfig::Default());

        // 2. Créer les buffers (sans données)
        m_VBO = std::make_shared<VertexBuffer>(BufferConfig::Static());
        m_IBO = std::make_shared<IndexBuffer>(IndexType::UnsignedInt, BufferConfig::Static());

        // 3. Configurer le layout et attacher les buffers AU VAO
        const auto layout = CreateLayout(m_Attributes);
        m_VAO.AddVertexBuffer(m_VBO, layout);  // Ceci bind le VAO et le VBO
        m_VAO.SetIndexBuffer(m_IBO);           // Ceci bind l'IBO

        // 4. Uploader les données (maintenant que tout est configuré)
        m_VBO->SetData(std::span(vertexData.data.data(), vertexData.data.size()));
        m_IBO->SetData(std::span(indices.data(), indices.size()));
    }

    void Mesh::AddSubMesh(const SubMesh& submesh) {
        m_SubMeshes.push_back(submesh);
    }

    void Mesh::Draw() const {
        m_VAO.Draw();
    }

    void Mesh::DrawSubMesh(const size_t index) const {
        if (index >= m_SubMeshes.size()) return;

        const auto& submesh = m_SubMeshes[index];
        m_VAO.Draw(submesh.indexCount, submesh.indexOffset);
    }

    VertexBufferLayout Mesh::CreateLayout(const VertexAttribute attrs) const {
        VertexBufferLayout layout;
        size_t offset = 0;
        uint32_t location = 0;

        if (HasAttribute(attrs, VertexAttribute::Position)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attrs, VertexAttribute::Normal)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attrs, VertexAttribute::TexCoord)) {
            layout.AddAttribute(VertexAttributeDescription::Vec2(location++, offset));
            offset += sizeof(Vec2);
        }

        if (HasAttribute(attrs, VertexAttribute::Color)) {
            layout.AddAttribute(VertexAttributeDescription::Vec4(location++, offset));
            offset += sizeof(Vec4);
        }

        if (HasAttribute(attrs, VertexAttribute::Tangent)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attrs, VertexAttribute::Bitangent)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        layout.SetStride(offset);
        return layout;
    }

    // ========== MeshBuilder ==========

    MeshBuilder& MeshBuilder::WithAttributes(const VertexAttribute attrs) {
        m_Attributes = attrs;
        return *this;
    }

    MeshBuilder& MeshBuilder::AddVertex(
        const Vec3& position,
        const std::optional<Vec3>& normal,
        const std::optional<Vec2>& texCoord,
        const std::optional<Vec4>& color,
        const std::optional<Vec3>& tangent,
        const std::optional<Vec3>& bitangent
    ) {
        m_Positions.push_back(position);

        if (HasAttribute(m_Attributes, VertexAttribute::Normal)) {
            m_Normals.push_back(normal.value_or(Vec3(0.0f, 1.0f, 0.0f)));
        }

        if (HasAttribute(m_Attributes, VertexAttribute::TexCoord)) {
            m_TexCoords.push_back(texCoord.value_or(Vec2(0.0f)));
        }

        if (HasAttribute(m_Attributes, VertexAttribute::Color)) {
            m_Colors.push_back(color.value_or(Vec4(1.0f)));
        }

        if (HasAttribute(m_Attributes, VertexAttribute::Tangent)) {
            m_Tangents.push_back(tangent.value_or(Vec3(1.0f, 0.0f, 0.0f)));
        }

        if (HasAttribute(m_Attributes, VertexAttribute::Bitangent)) {
            m_Bitangents.push_back(bitangent.value_or(Vec3(0.0f, 0.0f, 1.0f)));
        }

        return *this;
    }

    MeshBuilder& MeshBuilder::AddTriangle(const uint32_t i0, const uint32_t i1, const uint32_t i2) {
        m_Indices.push_back(i0);
        m_Indices.push_back(i1);
        m_Indices.push_back(i2);
        return *this;
    }

    size_t MeshBuilder::CalculateStride() const {
        size_t stride = 0;

        if (HasAttribute(m_Attributes, VertexAttribute::Position))
            stride += sizeof(Vec3);
        if (HasAttribute(m_Attributes, VertexAttribute::Normal))
            stride += sizeof(Vec3);
        if (HasAttribute(m_Attributes, VertexAttribute::TexCoord))
            stride += sizeof(Vec2);
        if (HasAttribute(m_Attributes, VertexAttribute::Color))
            stride += sizeof(Vec4);
        if (HasAttribute(m_Attributes, VertexAttribute::Tangent))
            stride += sizeof(Vec3);
        if (HasAttribute(m_Attributes, VertexAttribute::Bitangent))
            stride += sizeof(Vec3);

        return stride;
    }

    VertexData MeshBuilder::BuildVertexData() const {
        VertexData vertexData;
        vertexData.attributes = m_Attributes;
        vertexData.vertexCount = m_Positions.size();
        vertexData.stride = CalculateStride();

        const size_t floatsPerVertex = vertexData.stride / sizeof(float);
        vertexData.data.reserve(vertexData.vertexCount * floatsPerVertex);

        // Interleave vertex data
        for (size_t i = 0; i < m_Positions.size(); ++i) {
            // Position
            if (HasAttribute(m_Attributes, VertexAttribute::Position)) {
                vertexData.data.push_back(m_Positions[i].x);
                vertexData.data.push_back(m_Positions[i].y);
                vertexData.data.push_back(m_Positions[i].z);
            }

            // Normal
            if (HasAttribute(m_Attributes, VertexAttribute::Normal)) {
                vertexData.data.push_back(m_Normals[i].x);
                vertexData.data.push_back(m_Normals[i].y);
                vertexData.data.push_back(m_Normals[i].z);
            }

            // TexCoord
            if (HasAttribute(m_Attributes, VertexAttribute::TexCoord)) {
                vertexData.data.push_back(m_TexCoords[i].x);
                vertexData.data.push_back(m_TexCoords[i].y);
            }

            // Color
            if (HasAttribute(m_Attributes, VertexAttribute::Color)) {
                vertexData.data.push_back(m_Colors[i].x);
                vertexData.data.push_back(m_Colors[i].y);
                vertexData.data.push_back(m_Colors[i].z);
                vertexData.data.push_back(m_Colors[i].w);
            }

            // Tangent
            if (HasAttribute(m_Attributes, VertexAttribute::Tangent)) {
                vertexData.data.push_back(m_Tangents[i].x);
                vertexData.data.push_back(m_Tangents[i].y);
                vertexData.data.push_back(m_Tangents[i].z);
            }

            // Bitangent
            if (HasAttribute(m_Attributes, VertexAttribute::Bitangent)) {
                vertexData.data.push_back(m_Bitangents[i].x);
                vertexData.data.push_back(m_Bitangents[i].y);
                vertexData.data.push_back(m_Bitangents[i].z);
            }
        }

        return vertexData;
    }

    void MeshBuilder::Clear() {
        m_Positions.clear();
        m_Normals.clear();
        m_TexCoords.clear();
        m_Colors.clear();
        m_Tangents.clear();
        m_Bitangents.clear();
        m_Indices.clear();
    }

    // ========== MeshPrimitives ==========

    Mesh MeshPrimitives::CreateCube(const float size) {
        const float s = size * 0.5f;

        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        // Face avant (z+)
        builder.AddVertex(Vec3(-s, -s,  s), Vec3(0, 0, 1), Vec2(0, 0));
        builder.AddVertex(Vec3( s, -s,  s), Vec3(0, 0, 1), Vec2(1, 0));
        builder.AddVertex(Vec3( s,  s,  s), Vec3(0, 0, 1), Vec2(1, 1));
        builder.AddVertex(Vec3(-s,  s,  s), Vec3(0, 0, 1), Vec2(0, 1));

        // Face arrière (z-)
        builder.AddVertex(Vec3( s, -s, -s), Vec3(0, 0, -1), Vec2(0, 0));
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(0, 0, -1), Vec2(1, 0));
        builder.AddVertex(Vec3(-s,  s, -s), Vec3(0, 0, -1), Vec2(1, 1));
        builder.AddVertex(Vec3( s,  s, -s), Vec3(0, 0, -1), Vec2(0, 1));

        // Face droite (x+)
        builder.AddVertex(Vec3( s, -s,  s), Vec3(1, 0, 0), Vec2(0, 0));
        builder.AddVertex(Vec3( s, -s, -s), Vec3(1, 0, 0), Vec2(1, 0));
        builder.AddVertex(Vec3( s,  s, -s), Vec3(1, 0, 0), Vec2(1, 1));
        builder.AddVertex(Vec3( s,  s,  s), Vec3(1, 0, 0), Vec2(0, 1));

        // Face gauche (x-)
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(-1, 0, 0), Vec2(0, 0));
        builder.AddVertex(Vec3(-s, -s,  s), Vec3(-1, 0, 0), Vec2(1, 0));
        builder.AddVertex(Vec3(-s,  s,  s), Vec3(-1, 0, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s,  s, -s), Vec3(-1, 0, 0), Vec2(0, 1));

        // Face supérieure (y+)
        builder.AddVertex(Vec3(-s,  s,  s), Vec3(0, 1, 0), Vec2(0, 0));
        builder.AddVertex(Vec3( s,  s,  s), Vec3(0, 1, 0), Vec2(1, 0));
        builder.AddVertex(Vec3( s,  s, -s), Vec3(0, 1, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s,  s, -s), Vec3(0, 1, 0), Vec2(0, 1));

        // Face inférieure (y-)
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(0, -1, 0), Vec2(0, 0));
        builder.AddVertex(Vec3( s, -s, -s), Vec3(0, -1, 0), Vec2(1, 0));
        builder.AddVertex(Vec3( s, -s,  s), Vec3(0, -1, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s, -s,  s), Vec3(0, -1, 0), Vec2(0, 1));

        // Indices : 2 triangles par face
        for (uint32_t i = 0; i < 6; ++i) {
            const uint32_t base = i * 4;
            builder.AddTriangle(base + 0, base + 1, base + 2);
            builder.AddTriangle(base + 0, base + 2, base + 3);
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    Mesh MeshPrimitives::CreateSphere(float radius, uint32_t segments, uint32_t rings) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        constexpr float PI = 3.14159265359f;

        for (uint32_t ring = 0; ring <= rings; ++ring) {
            float v = static_cast<float>(ring) / static_cast<float>(rings);
            float phi = v * PI;

            for (uint32_t seg = 0; seg <= segments; ++seg) {
                float u = static_cast<float>(seg) / static_cast<float>(segments);
                float theta = u * 2.0f * PI;

                float x = std::cos(theta) * std::sin(phi);
                float y = std::cos(phi);
                float z = std::sin(theta) * std::sin(phi);

                Vec3 position = Vec3(x, y, z) * radius;
                Vec3 normal = glm::normalize(Vec3(x, y, z));
                Vec2 texCoord = Vec2(u, v);

                builder.AddVertex(position, normal, texCoord);
            }
        }

        for (uint32_t ring = 0; ring < rings; ++ring) {
            for (uint32_t seg = 0; seg < segments; ++seg) {
                uint32_t a = ring * (segments + 1) + seg;
                uint32_t b = a + segments + 1;

                builder.AddTriangle(a, b, a + 1);
                builder.AddTriangle(b, b + 1, a + 1);
            }
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    Mesh MeshPrimitives::CreatePlane(const float width, const float height, const uint32_t subdivisions) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float halfW = width * 0.5f;
        const float halfH = height * 0.5f;

        for (uint32_t y = 0; y <= subdivisions; ++y) {
            for (uint32_t x = 0; x <= subdivisions; ++x) {
                const float u = static_cast<float>(x) / static_cast<float>(subdivisions);
                const float v = static_cast<float>(y) / static_cast<float>(subdivisions);

                const float px = -halfW + u * width;
                const float pz = -halfH + v * height;

                builder.AddVertex(
                    Vec3(px, 0.0f, pz),
                    Vec3(0.0f, 1.0f, 0.0f),
                    Vec2(u, v)
                );
            }
        }

        for (uint32_t y = 0; y < subdivisions; ++y) {
            for (uint32_t x = 0; x < subdivisions; ++x) {
                const uint32_t a = y * (subdivisions + 1) + x;
                const uint32_t b = a + subdivisions + 1;

                builder.AddTriangle(a, b, a + 1);
                builder.AddTriangle(b, b + 1, a + 1);
            }
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    Mesh MeshPrimitives::CreateQuad() {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        builder.AddVertex(Vec3(-1.0f, -1.0f, 0.0f), Vec3(0, 0, 1), Vec2(0, 0));
        builder.AddVertex(Vec3(1.0f, -1.0f, 0.0f), Vec3(0, 0, 1), Vec2(1, 0));
        builder.AddVertex(Vec3(1.0f, 1.0f, 0.0f), Vec3(0, 0, 1), Vec2(1, 1));
        builder.AddVertex(Vec3(-1.0f, 1.0f, 0.0f), Vec3(0, 0, 1), Vec2(0, 1));

        builder.AddTriangle(0, 1, 2);
        builder.AddTriangle(0, 2, 3);

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }
}