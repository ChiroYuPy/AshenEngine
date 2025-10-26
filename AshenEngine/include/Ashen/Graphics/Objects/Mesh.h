#ifndef ASHEN_MESH_H
#define ASHEN_MESH_H

#include <functional>
#include <string>
#include <optional>

#include "Ashen/Core/Types.h"
#include "Ashen/GraphicsAPI/VertexArray.h"
#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/Math/Math.h"

namespace ash {
    /**
     * @brief Flags indicating which vertex attributes are present
     */
    enum class VertexAttribute : uint32_t {
        None = 0,
        Position = 1 << 0,
        Normal = 1 << 1,
        TexCoord = 1 << 2,
        Color = 1 << 3,
        Tangent = 1 << 4,
        Bitangent = 1 << 5
    };

    inline VertexAttribute operator|(VertexAttribute a, VertexAttribute b) {
        return static_cast<VertexAttribute>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline VertexAttribute operator&(VertexAttribute a, VertexAttribute b) {
        return static_cast<VertexAttribute>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline bool HasAttribute(const VertexAttribute flags, const VertexAttribute attr) {
        return (flags & attr) == attr;
    }

    /**
     * @brief Interleaved vertex data structure
     */
    struct VertexData {
        Vector<float> data;
        VertexAttribute attributes = VertexAttribute::None;
        size_t vertexCount = 0;
        size_t stride = 0;

        void Reserve(const size_t count) {
            const size_t floatsPerVertex = stride / sizeof(float);
            data.reserve(count * floatsPerVertex);
        }

        void Clear() {
            data.clear();
            vertexCount = 0;
        }
    };

    /**
     * @brief Mesh submesh for multi-material support
     */
    struct SubMesh {
        size_t indexOffset = 0;
        size_t indexCount = 0;
        std::string materialName;
    };

    /**
     * @brief Mesh class with flexible vertex structure
     */
    class Mesh {
    public:
        Mesh() = default;

        ~Mesh() = default;

        Mesh(const Mesh &) = delete;

        Mesh &operator=(const Mesh &) = delete;

        Mesh(Mesh &&) noexcept = default;

        Mesh &operator=(Mesh &&) noexcept = default;

        // ---- Data Management ----
        void SetData(const VertexData &vertexData, const Vector<uint32_t> &indices);

        void SetSubMeshes(const Vector<SubMesh> &submeshes);

        // ---- Accessors ONLY ----
        const VertexArray &GetVAO() const { return m_VAO; }
        size_t GetVertexCount() const { return m_VertexCount; }
        size_t GetIndexCount() const { return m_IndexCount; }
        VertexAttribute GetAttributes() const { return m_Attributes; }

        const Vector<SubMesh> &GetSubMeshes() const { return m_SubMeshes; }
        size_t GetSubMeshCount() const { return m_SubMeshes.size(); }
        bool HasSubMeshes() const { return !m_SubMeshes.empty(); }

        // ---- Bounding Volume ----
        const Vec3 &GetBoundsMin() const { return m_BoundsMin; }
        const Vec3 &GetBoundsMax() const { return m_BoundsMax; }
        Vec3 GetBoundsCenter() const { return (m_BoundsMin + m_BoundsMax) * 0.5f; }
        Vec3 GetBoundsExtents() const { return (m_BoundsMax - m_BoundsMin) * 0.5f; }

    private:
        VertexArray m_VAO;
        std::shared_ptr<VertexBuffer> m_VBO;
        std::shared_ptr<IndexBuffer> m_IBO;

        VertexAttribute m_Attributes = VertexAttribute::None;
        size_t m_VertexCount = 0;
        size_t m_IndexCount = 0;

        Vector<SubMesh> m_SubMeshes;

        Vec3 m_BoundsMin{0.0f};
        Vec3 m_BoundsMax{0.0f};

        static VertexBufferLayout CreateLayout(VertexAttribute attributes);

        void ComputeBounds(const VertexData &vertexData);
    };

    class MeshRenderer {
    public:
        // ---- Rendering Methods ----
        static void Draw(const Mesh &mesh);

        static void DrawSubMesh(const Mesh &mesh, size_t submeshIndex);

        static void DrawInstanced(const Mesh &mesh, uint32_t instanceCount);

        static void DrawSubMeshInstanced(const Mesh &mesh, size_t submeshIndex, uint32_t instanceCount);
    };

    /**
     * @brief Builder for creating mesh vertex data
     */
    class MeshBuilder {
    public:
        MeshBuilder() = default;

        /**
         * @brief Set which attributes to include
         */
        MeshBuilder &WithAttributes(VertexAttribute attrs);

        /**
         * @brief Add a vertex with all specified attributes
         */
        MeshBuilder &AddVertex(
            const Vec3 &position,
            const std::optional<Vec3> &normal = std::nullopt,
            const std::optional<Vec2> &texCoord = std::nullopt,
            const std::optional<Vec4> &color = std::nullopt,
            const std::optional<Vec3> &tangent = std::nullopt,
            const std::optional<Vec3> &bitangent = std::nullopt
        );

        /**
         * @brief Add triangle indices
         */
        MeshBuilder &AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2);

        /**
         * @brief Build the final vertex data
         */
        [[nodiscard]] VertexData BuildVertexData() const;

        /**
         * @brief Get indices
         */
        [[nodiscard]] const Vector<uint32_t> &GetIndices() const { return m_Indices; }

        /**
         * @brief Reset builder
         */
        void Clear();

    private:
        VertexAttribute m_Attributes = VertexAttribute::None;
        Vector<Vec3> m_Positions;
        Vector<Vec3> m_Normals;
        Vector<Vec2> m_TexCoords;
        Vector<Vec4> m_Colors;
        Vector<Vec3> m_Tangents;
        Vector<Vec3> m_Bitangents;
        Vector<uint32_t> m_Indices;

        size_t CalculateStride() const;
    };

    /**
     * @brief Predefined mesh generators
     */
    namespace MeshPrimitives {
        // Basic primitives
        Mesh CreateCube(float size = 1.0f);

        Mesh CreateSphere(float radius = 1.0f, uint32_t rings = 32, uint32_t segments = 64);

        Mesh CreatePlane(float width = 1.0f, float height = 1.0f, uint32_t subdivisionsX = 1,
                         uint32_t subdivisionsZ = 1);

        Mesh CreateQuad(float width = 1.0f, float height = 1.0f);

        // Cylinder and capsule
        Mesh CreateCylinder(float radius = 0.5f, float height = 2.0f, uint32_t sides = 32, uint32_t heightSegments = 1);

        Mesh CreateCone(float radius = 0.5f, float height = 1.0f, uint32_t sides = 32);

        Mesh CreateCapsule(float radius = 0.5f, float height = 2.0f, uint32_t radialSegments = 32, uint32_t rings = 8);

        // Torus
        Mesh CreateTorus(float innerRadius = 0.5f, float outerRadius = 1.0f, uint32_t rings = 32, uint32_t sides = 32);

        // Prisms
        Mesh CreatePrism(uint32_t sides = 3, float radius = 1.0f, float height = 2.0f);

        // Advanced primitives
        Mesh CreateIcosphere(float radius = 1.0f, uint32_t subdivisions = 2);

        Mesh CreateTetrahedron(float size = 1.0f);

        Mesh CreateOctahedron(float size = 1.0f);

        Mesh CreateDodecahedron(float size = 1.0f);

        Mesh CreateIcosahedron(float size = 1.0f);

        // Procedural
        Mesh CreateHeightmap(const Vector<float> &heights, uint32_t width, uint32_t depth, float heightScale = 1.0f);

        Mesh CreateParametric(
            std::function < Vec3(float, float) > func,
            uint32_t uSegments = 32,
            uint32_t vSegments = 32,
            
        float uMin = 0.0f, 
        float uMax = 1.0f,
            
        float vMin = 0.0f, 
        float vMax = 1.0f
        );

        // Text/Debug
        Mesh CreateArrow(float length = 1.0f, float headSize = 0.2f);

        Mesh CreateGrid(uint32_t size = 10, float spacing = 1.0f);

        Mesh CreateWireCube(float size = 1.0f);

        Mesh CreateWireSphere(float radius = 1.0f, uint32_t segments = 16);
    }
}

#endif // ASHEN_MESH_H