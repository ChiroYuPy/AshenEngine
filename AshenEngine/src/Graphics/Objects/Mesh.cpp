#include "Ashen/Graphics/Objects/Mesh.h"

#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Rendering/Renderer.h"

namespace ash {
    // ========== Mesh ==========

    void Mesh::SetData(const VertexData &vertexData, const Vector<uint32_t> &indices) {
        m_Attributes = vertexData.attributes;
        m_VertexCount = vertexData.vertexCount;
        m_IndexCount = indices.size();

        // 1. Create VAO first
        m_VAO = VertexArray(VertexArrayConfig::Default());

        // 2. Create buffers
        m_VBO = std::make_shared<VertexBuffer>(BufferConfig::Static());
        m_IBO = std::make_shared<IndexBuffer>(IndexType::UnsignedInt, BufferConfig::Static());

        // 3. Configure layout and attach to VAO
        const auto layout = CreateLayout(m_Attributes);
        m_VAO.AddVertexBuffer(m_VBO, layout);
        m_VAO.SetIndexBuffer(m_IBO);

        // 4. Upload data
        m_VBO->SetData(std::span(vertexData.data.data(), vertexData.data.size()));
        m_IBO->SetData(std::span(indices.data(), indices.size()));

        // 5. Compute bounding box
        ComputeBounds(vertexData);
    }

    void Mesh::SetSubMeshes(const Vector<SubMesh> &submeshes) {
        m_SubMeshes = submeshes;
    }

    VertexBufferLayout Mesh::CreateLayout(const VertexAttribute attributes) {
        VertexBufferLayout layout;
        uint32_t location = 0;
        size_t offset = 0;

        if (HasAttribute(attributes, VertexAttribute::Position)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attributes, VertexAttribute::Normal)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attributes, VertexAttribute::TexCoord)) {
            layout.AddAttribute(VertexAttributeDescription::Vec2(location++, offset));
            offset += sizeof(Vec2);
        }

        if (HasAttribute(attributes, VertexAttribute::Color)) {
            layout.AddAttribute(VertexAttributeDescription::Vec4(location++, offset));
            offset += sizeof(Vec4);
        }

        if (HasAttribute(attributes, VertexAttribute::Tangent)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        if (HasAttribute(attributes, VertexAttribute::Bitangent)) {
            layout.AddAttribute(VertexAttributeDescription::Vec3(location++, offset));
            offset += sizeof(Vec3);
        }

        layout.SetStride(static_cast<uint32_t>(offset));
        return layout;
    }

    void Mesh::ComputeBounds(const VertexData &vertexData) {
        if (!HasAttribute(vertexData.attributes, VertexAttribute::Position) ||
            vertexData.vertexCount == 0) {
            m_BoundsMin = Vec3(0.0f);
            m_BoundsMax = Vec3(0.0f);
            return;
            }

        const auto layout = CreateLayout(vertexData.attributes);
        const size_t stride = layout.GetStride() / sizeof(float);
        const size_t positionOffset = 0; // Position is always first

        m_BoundsMin = Vec3(std::numeric_limits<float>::max());
        m_BoundsMax = Vec3(std::numeric_limits<float>::lowest());

        for (size_t i = 0; i < vertexData.vertexCount; ++i) {
            const size_t index = i * stride + positionOffset;
            const Vec3 position(
                vertexData.data[index],
                vertexData.data[index + 1],
                vertexData.data[index + 2]
            );

            m_BoundsMin = glm::min(m_BoundsMin, position);
            m_BoundsMax = glm::max(m_BoundsMax, position);
        }
    }

    // ========== MeshRenderer ==========

    void MeshRenderer::Draw(const Mesh& mesh) {
        Renderer::Draw(mesh.GetVAO());
    }

    void MeshRenderer::DrawSubMesh(const Mesh& mesh, const size_t submeshIndex) {
        if (submeshIndex >= mesh.GetSubMeshCount()) {
            return;
        }

        const auto& submesh = mesh.GetSubMeshes()[submeshIndex];
        Renderer::DrawIndexed(mesh.GetVAO(), submesh.indexCount, submesh.indexOffset);
    }

    void MeshRenderer::DrawInstanced(const Mesh& mesh, const uint32_t instanceCount) {
        Renderer::DrawInstanced(mesh.GetVAO(), instanceCount);
    }

    void MeshRenderer::DrawSubMeshInstanced(const Mesh& mesh, const size_t submeshIndex,
                                             const uint32_t instanceCount) {
        if (submeshIndex >= mesh.GetSubMeshCount()) {
            return;
        }

        const auto& submesh = mesh.GetSubMeshes()[submeshIndex];
        Renderer::DrawIndexedInstanced(mesh.GetVAO(), submesh.indexCount,
                                        instanceCount, submesh.indexOffset);
    }

    // ========== MeshBuilder ==========

    MeshBuilder &MeshBuilder::WithAttributes(const VertexAttribute attrs) {
        m_Attributes = attrs;
        return *this;
    }

    MeshBuilder &MeshBuilder::AddVertex(
        const Vec3 &position,
        const std::optional<Vec3> &normal,
        const std::optional<Vec2> &texCoord,
        const std::optional<Vec4> &color,
        const std::optional<Vec3> &tangent,
        const std::optional<Vec3> &bitangent
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

    MeshBuilder &MeshBuilder::AddTriangle(const uint32_t i0, const uint32_t i1, const uint32_t i2) {
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
}


namespace ash::MeshPrimitives {
    // Helper functions
    namespace {
        constexpr float PI = 3.14159265359f;
        constexpr float TWO_PI = 6.28318530718f;

        Vec3 CalculateNormal(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
            return glm::normalize(glm::cross(v1 - v0, v2 - v0));
        }
    }

    Mesh CreateCube(const float size) {
        const float s = size * 0.5f;

        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        // Face avant (z+)
        builder.AddVertex(Vec3(-s, -s, s), Vec3(0, 0, 1), Vec2(0, 0));
        builder.AddVertex(Vec3(s, -s, s), Vec3(0, 0, 1), Vec2(1, 0));
        builder.AddVertex(Vec3(s, s, s), Vec3(0, 0, 1), Vec2(1, 1));
        builder.AddVertex(Vec3(-s, s, s), Vec3(0, 0, 1), Vec2(0, 1));

        // Face arrière (z-)
        builder.AddVertex(Vec3(s, -s, -s), Vec3(0, 0, -1), Vec2(0, 0));
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(0, 0, -1), Vec2(1, 0));
        builder.AddVertex(Vec3(-s, s, -s), Vec3(0, 0, -1), Vec2(1, 1));
        builder.AddVertex(Vec3(s, s, -s), Vec3(0, 0, -1), Vec2(0, 1));

        // Face droite (x+)
        builder.AddVertex(Vec3(s, -s, s), Vec3(1, 0, 0), Vec2(0, 0));
        builder.AddVertex(Vec3(s, -s, -s), Vec3(1, 0, 0), Vec2(1, 0));
        builder.AddVertex(Vec3(s, s, -s), Vec3(1, 0, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(s, s, s), Vec3(1, 0, 0), Vec2(0, 1));

        // Face gauche (x-)
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(-1, 0, 0), Vec2(0, 0));
        builder.AddVertex(Vec3(-s, -s, s), Vec3(-1, 0, 0), Vec2(1, 0));
        builder.AddVertex(Vec3(-s, s, s), Vec3(-1, 0, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s, s, -s), Vec3(-1, 0, 0), Vec2(0, 1));

        // Face supérieure (y+)
        builder.AddVertex(Vec3(-s, s, s), Vec3(0, 1, 0), Vec2(0, 0));
        builder.AddVertex(Vec3(s, s, s), Vec3(0, 1, 0), Vec2(1, 0));
        builder.AddVertex(Vec3(s, s, -s), Vec3(0, 1, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s, s, -s), Vec3(0, 1, 0), Vec2(0, 1));

        // Face inférieure (y-)
        builder.AddVertex(Vec3(-s, -s, -s), Vec3(0, -1, 0), Vec2(0, 0));
        builder.AddVertex(Vec3(s, -s, -s), Vec3(0, -1, 0), Vec2(1, 0));
        builder.AddVertex(Vec3(s, -s, s), Vec3(0, -1, 0), Vec2(1, 1));
        builder.AddVertex(Vec3(-s, -s, s), Vec3(0, -1, 0), Vec2(0, 1));

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

    Mesh CreateSphere(float radius, uint32_t rings, uint32_t segments) {
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

    Mesh CreatePlane(const float width, const float height, const uint32_t subdivisionsX,
                     const uint32_t subdivisionsZ) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float halfW = width * 0.5f;
        const float halfH = height * 0.5f;

        for (uint32_t y = 0; y <= subdivisionsZ; ++y) {
            for (uint32_t x = 0; x <= subdivisionsX; ++x) {
                const float u = static_cast<float>(x) / static_cast<float>(subdivisionsZ);
                const float v = static_cast<float>(y) / static_cast<float>(subdivisionsX);

                const float px = -halfW + u * width;
                const float pz = -halfH + v * height;

                builder.AddVertex(
                    Vec3(px, 0.0f, pz),
                    Vec3(0.0f, 1.0f, 0.0f),
                    Vec2(u, v)
                );
            }
        }

        for (uint32_t y = 0; y < subdivisionsZ; ++y) {
            for (uint32_t x = 0; x < subdivisionsX; ++x) {
                const uint32_t a = y * (subdivisionsZ + 1) + x;
                const uint32_t b = a + subdivisionsX + 1;

                builder.AddTriangle(a, b, a + 1);
                builder.AddTriangle(b, b + 1, a + 1);
            }
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    Mesh CreateQuad(const float width, const float height) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float hw = width * 0.5f;
        const float hh = height * 0.5f;

        // Vertices avec largeur et hauteur correctes
        builder.AddVertex(Vec3(-hw, -hh, 0.0f), Vec3(0, 0, 1), Vec2(0, 0));
        builder.AddVertex(Vec3(hw, -hh, 0.0f), Vec3(0, 0, 1), Vec2(1, 0));
        builder.AddVertex(Vec3(hw, hh, 0.0f), Vec3(0, 0, 1), Vec2(1, 1));
        builder.AddVertex(Vec3(-hw, hh, 0.0f), Vec3(0, 0, 1), Vec2(0, 1));

        // Triangles
        builder.AddTriangle(0, 1, 2);
        builder.AddTriangle(0, 2, 3);

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === CYLINDER ===
    Mesh CreateCylinder(float radius, float height, uint32_t sides, uint32_t heightSegments) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float halfHeight = height * 0.5f;
        const float angleStep = TWO_PI / sides;

        // Generate vertices
        for (uint32_t h = 0; h <= heightSegments; ++h) {
            const float y = -halfHeight + (h * height / heightSegments);
            const float v = static_cast<float>(h) / heightSegments;

            for (uint32_t s = 0; s <= sides; ++s) {
                const float angle = s * angleStep;
                const float x = std::cos(angle) * radius;
                const float z = std::sin(angle) * radius;
                const float u = static_cast<float>(s) / sides;

                Vec3 pos(x, y, z);
                Vec3 normal = glm::normalize(Vec3(x, 0, z));
                Vec2 uv(u, v);

                builder.AddVertex(pos, normal, uv);
            }
        }

        // Generate indices for sides
        for (uint32_t h = 0; h < heightSegments; ++h) {
            for (uint32_t s = 0; s < sides; ++s) {
                const uint32_t i0 = h * (sides + 1) + s;
                const uint32_t i1 = i0 + 1;
                const uint32_t i2 = i0 + (sides + 1);
                const uint32_t i3 = i2 + 1;

                builder.AddTriangle(i0, i2, i1);
                builder.AddTriangle(i1, i2, i3);
            }
        }

        // Top and bottom caps
        const uint32_t baseVertexTop = builder.GetIndices().size();
        const uint32_t centerTop = baseVertexTop;
        builder.AddVertex(Vec3(0, halfHeight, 0), Vec3(0, 1, 0), Vec2(0.5f, 0.5f));

        for (uint32_t s = 0; s <= sides; ++s) {
            const float angle = s * angleStep;
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            builder.AddVertex(Vec3(x, halfHeight, z), Vec3(0, 1, 0),
                              Vec2(0.5f + x / (2 * radius), 0.5f + z / (2 * radius)));
        }

        for (uint32_t s = 0; s < sides; ++s) {
            builder.AddTriangle(centerTop, centerTop + s + 1, centerTop + s + 2);
        }

        // Bottom cap
        const uint32_t baseVertexBottom = builder.GetIndices().size();
        const uint32_t centerBottom = baseVertexBottom;
        builder.AddVertex(Vec3(0, -halfHeight, 0), Vec3(0, -1, 0), Vec2(0.5f, 0.5f));

        for (uint32_t s = 0; s <= sides; ++s) {
            const float angle = s * angleStep;
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            builder.AddVertex(Vec3(x, -halfHeight, z), Vec3(0, -1, 0),
                              Vec2(0.5f + x / (2 * radius), 0.5f + z / (2 * radius)));
        }

        for (uint32_t s = 0; s < sides; ++s) {
            builder.AddTriangle(centerBottom, centerBottom + s + 2, centerBottom + s + 1);
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === CONE ===
    Mesh CreateCone(float radius, float height, uint32_t sides) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float angleStep = TWO_PI / sides;
        const Vec3 apex(0, height, 0);

        // Apex vertex
        builder.AddVertex(apex, Vec3(0, 1, 0), Vec2(0.5f, 1.0f));

        // Side vertices
        for (uint32_t s = 0; s <= sides; ++s) {
            const float angle = s * angleStep;
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;

            Vec3 pos(x, 0, z);
            Vec3 toApex = glm::normalize(apex - pos);
            Vec3 outward = glm::normalize(Vec3(x, 0, z));
            Vec3 normal = glm::normalize(toApex + outward);

            builder.AddVertex(pos, normal, Vec2(static_cast<float>(s) / sides, 0));
        }

        // Side triangles
        for (uint32_t s = 0; s < sides; ++s) {
            builder.AddTriangle(0, s + 1, s + 2);
        }

        // Base cap
        const uint32_t centerBase = builder.GetIndices().size();
        builder.AddVertex(Vec3(0, 0, 0), Vec3(0, -1, 0), Vec2(0.5f, 0.5f));

        for (uint32_t s = 0; s <= sides; ++s) {
            const float angle = s * angleStep;
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            builder.AddVertex(Vec3(x, 0, z), Vec3(0, -1, 0),
                              Vec2(0.5f + x / (2 * radius), 0.5f + z / (2 * radius)));
        }

        for (uint32_t s = 0; s < sides; ++s) {
            builder.AddTriangle(centerBase, centerBase + s + 2, centerBase + s + 1);
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === CAPSULE ===
    Mesh CreateCapsule(float radius, float height, uint32_t radialSegments, uint32_t rings) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float cylinderHeight = height - 2 * radius;
        const float halfCylinder = cylinderHeight * 0.5f;

        // Top hemisphere
        for (uint32_t ring = 0; ring <= rings; ++ring) {
            const float phi = (PI / 2.0f) * (static_cast<float>(ring) / rings);
            const float y = radius * std::sin(phi) + halfCylinder;
            const float ringRadius = radius * std::cos(phi);

            for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
                const float theta = TWO_PI * (static_cast<float>(seg) / radialSegments);
                const float x = ringRadius * std::cos(theta);
                const float z = ringRadius * std::sin(theta);

                Vec3 pos(x, y, z);
                Vec3 normal = glm::normalize(pos - Vec3(0, halfCylinder, 0));
                Vec2 uv(static_cast<float>(seg) / radialSegments,
                        static_cast<float>(ring) / rings * 0.5f + 0.5f);

                builder.AddVertex(pos, normal, uv);
            }
        }

        // Cylinder middle
        const uint32_t cylinderStart = (rings + 1) * (radialSegments + 1);
        for (uint32_t h = 0; h <= 1; ++h) {
            const float y = h == 0 ? halfCylinder : -halfCylinder;

            for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
                const float theta = TWO_PI * (static_cast<float>(seg) / radialSegments);
                const float x = radius * std::cos(theta);
                const float z = radius * std::sin(theta);

                Vec3 pos(x, y, z);
                Vec3 normal = glm::normalize(Vec3(x, 0, z));
                Vec2 uv(static_cast<float>(seg) / radialSegments, 0.5f);

                builder.AddVertex(pos, normal, uv);
            }
        }

        // Bottom hemisphere
        const uint32_t bottomStart = cylinderStart + 2 * (radialSegments + 1);
        for (uint32_t ring = 0; ring <= rings; ++ring) {
            const float phi = (PI / 2.0f) * (static_cast<float>(ring) / rings);
            const float y = -radius * std::sin(phi) - halfCylinder;
            const float ringRadius = radius * std::cos(phi);

            for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
                const float theta = TWO_PI * (static_cast<float>(seg) / radialSegments);
                const float x = ringRadius * std::cos(theta);
                const float z = ringRadius * std::sin(theta);

                Vec3 pos(x, y, z);
                Vec3 normal = glm::normalize(pos - Vec3(0, -halfCylinder, 0));
                Vec2 uv(static_cast<float>(seg) / radialSegments,
                        static_cast<float>(ring) / rings * 0.5f);

                builder.AddVertex(pos, normal, uv);
            }
        }

        // Generate indices (top hemisphere + cylinder + bottom hemisphere)
        auto addRingIndices = [&](const uint32_t start, const uint32_t ringCount) {
            for (uint32_t ring = 0; ring < ringCount; ++ring) {
                for (uint32_t seg = 0; seg < radialSegments; ++seg) {
                    const uint32_t i0 = start + ring * (radialSegments + 1) + seg;
                    const uint32_t i1 = i0 + 1;
                    const uint32_t i2 = i0 + (radialSegments + 1);
                    const uint32_t i3 = i2 + 1;

                    builder.AddTriangle(i0, i2, i1);
                    builder.AddTriangle(i1, i2, i3);
                }
            }
        };

        addRingIndices(0, rings);
        addRingIndices(cylinderStart, 1);
        addRingIndices(bottomStart, rings);

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === TORUS ===
    Mesh CreateTorus(float innerRadius, float outerRadius, uint32_t rings, uint32_t sides) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float tubeRadius = (outerRadius - innerRadius) * 0.5f;
        const float centerRadius = innerRadius + tubeRadius;

        for (uint32_t ring = 0; ring <= rings; ++ring) {
            const float u = static_cast<float>(ring) / rings;
            const float theta = u * TWO_PI;

            const float cosTheta = std::cos(theta);
            const float sinTheta = std::sin(theta);

            for (uint32_t side = 0; side <= sides; ++side) {
                const float v = static_cast<float>(side) / sides;
                const float phi = v * TWO_PI;

                const float cosPhi = std::cos(phi);
                const float sinPhi = std::sin(phi);

                const float x = (centerRadius + tubeRadius * cosPhi) * cosTheta;
                const float y = tubeRadius * sinPhi;
                const float z = (centerRadius + tubeRadius * cosPhi) * sinTheta;

                Vec3 pos(x, y, z);
                Vec3 center(centerRadius * cosTheta, 0, centerRadius * sinTheta);
                Vec3 normal = glm::normalize(pos - center);

                builder.AddVertex(pos, normal, Vec2(u, v));
            }
        }

        // Generate indices
        for (uint32_t ring = 0; ring < rings; ++ring) {
            for (uint32_t side = 0; side < sides; ++side) {
                const uint32_t i0 = ring * (sides + 1) + side;
                const uint32_t i1 = i0 + 1;
                const uint32_t i2 = i0 + (sides + 1);
                const uint32_t i3 = i2 + 1;

                builder.AddTriangle(i0, i2, i1);
                builder.AddTriangle(i1, i2, i3);
            }
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === PRISM ===
    Mesh CreatePrism(const uint32_t sides, const float radius, const float height) {
        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        const float halfHeight = height * 0.5f;
        const float angleStep = TWO_PI / sides;

        // Top and bottom vertices
        for (uint32_t h = 0; h < 2; ++h) {
            const float y = h == 0 ? halfHeight : -halfHeight;

            for (uint32_t s = 0; s < sides; ++s) {
                const float angle = s * angleStep;
                const float x = std::cos(angle) * radius;
                const float z = std::sin(angle) * radius;

                builder.AddVertex(Vec3(x, y, z), Vec3(0, h == 0 ? 1 : -1, 0),
                                  Vec2(static_cast<float>(s) / sides, h));
            }
        }

        // Side faces
        for (uint32_t s = 0; s < sides; ++s) {
            const uint32_t i0 = s;
            const uint32_t i1 = (s + 1) % sides;
            const uint32_t i2 = s + sides;
            const uint32_t i3 = ((s + 1) % sides) + sides;

            builder.AddTriangle(i0, i2, i1);
            builder.AddTriangle(i1, i2, i3);
        }

        // Top cap
        for (uint32_t s = 1; s < sides - 1; ++s) {
            builder.AddTriangle(0, s, s + 1);
        }

        // Bottom cap
        for (uint32_t s = 1; s < sides - 1; ++s) {
            builder.AddTriangle(sides, sides + s + 1, sides + s);
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }

    // === ICOSPHERE ===
    Mesh CreateIcosphere(float radius, uint32_t subdivisions) {
        // Golden ratio
        const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

        MeshBuilder builder;
        builder.WithAttributes(
            VertexAttribute::Position |
            VertexAttribute::Normal |
            VertexAttribute::TexCoord
        );

        // Initial 12 vertices of icosahedron
        Vector<Vec3> vertices = {
            {-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0},
            {0, -1, t}, {0, 1, t}, {0, -1, -t}, {0, 1, -t},
            {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1}
        };

        // Normalize to unit sphere
        for (auto &v: vertices) {
            v = glm::normalize(v) * radius;
        }

        // Initial 20 triangular faces
        Vector<uint32_t> indices = {
            0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
            1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
            3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
            4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
        };

        for (const auto &v: vertices) {
            Vec3 normal = glm::normalize(v);
            float u = 0.5f + std::atan2(normal.z, normal.x) / TWO_PI;
            float v_coord = 0.5f - std::asin(normal.y) / PI;
            builder.AddVertex(v, normal, Vec2(u, v_coord));
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            builder.AddTriangle(indices[i], indices[i + 1], indices[i + 2]);
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), builder.GetIndices());
        return mesh;
    }
}
