#include "Ashen/Resources/Loader/MeshLoader.h"

#include <unordered_map>
#include <tiny_obj_loader.h>

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {

    // Helper for vertex deduplication
    struct VertexKey {
        int posIdx = -1;
        int normalIdx = -1;
        int texCoordIdx = -1;

        bool operator==(const VertexKey& other) const {
            return posIdx == other.posIdx &&
                   normalIdx == other.normalIdx &&
                   texCoordIdx == other.texCoordIdx;
        }
    };
}

// Hash specialization for VertexKey
namespace std {
    template<>
    struct hash<ash::VertexKey> {
        size_t operator()(const ash::VertexKey& key) const noexcept {
            const size_t h1 = hash<int>{}(key.posIdx);
            const size_t h2 = hash<int>{}(key.normalIdx);
            const size_t h3 = hash<int>{}(key.texCoordIdx);
            return h1 ^ h2 << 1 ^ h3 << 2;
        }
    };
}

namespace ash {

    Mesh MeshLoader::Load(const fs::path& path, bool flipUVs) {
        if (!FileSystem::Exists(path)) {
            throw std::runtime_error("Mesh file not found: " + path.string());
        }

        // Parse OBJ file using tinyobjloader
        tinyobj::attrib_t attrib;
        Vector<tinyobj::shape_t> shapes;
        Vector<tinyobj::material_t> materials;
        std::string warn, err;

        const std::string mtlBasedir = path.parent_path().string() + "/";
        const bool success = tinyobj::LoadObj(
            &attrib, &shapes, &materials,
            &warn, &err,
            path.string().c_str(),
            mtlBasedir.c_str(),
            true // triangulate
        );

        if (!warn.empty()) {
            Logger::Warn() << "OBJ Warning: " << warn;
        }
        if (!err.empty()) {
            Logger::Error() << "OBJ Error: " << err;
        }
        if (!success) {
            throw std::runtime_error("Failed to load OBJ: " + path.string());
        }
        if (shapes.empty()) {
            throw std::runtime_error("No shapes found in OBJ: " + path.string());
        }

        // Use first shape only (multi-mesh not supported in this simplified version)
        const auto& shape = shapes[0];

        // Determine available attributes
        VertexAttribute attributes = VertexAttribute::Position;
        if (!attrib.normals.empty()) {
            attributes = attributes | VertexAttribute::Normal;
        }
        if (!attrib.texcoords.empty()) {
            attributes = attributes | VertexAttribute::TexCoord;
        }

        // Build mesh data with deduplication
        MeshBuilder builder;
        builder.WithAttributes(attributes);

        Vector<uint32_t> indices;
        std::unordered_map<VertexKey, uint32_t> vertexMap;
        uint32_t vertexCount = 0;

        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            const size_t faceVertCount = shape.mesh.num_face_vertices[f];

            // Should be triangulated already
            if (faceVertCount != 3) {
                Logger::Warn() << "Non-triangular face found, skipping";
                indexOffset += faceVertCount;
                continue;
            }

            // Process triangle
            for (size_t v = 0; v < 3; ++v) {
                const tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];

                // Create vertex key for deduplication
                VertexKey key;
                key.posIdx = idx.vertex_index;
                key.normalIdx = idx.normal_index;
                key.texCoordIdx = idx.texcoord_index;

                // Check if vertex already exists
                auto it = vertexMap.find(key);
                if (it != vertexMap.end()) {
                    // Reuse existing vertex
                    indices.push_back(it->second);
                } else {
                    // Create new vertex
                    Vec3 position{
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                    };

                    std::optional<Vec3> normal;
                    if (idx.normal_index >= 0 && !attrib.normals.empty()) {
                        normal = Vec3{
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]
                        };
                    }

                    std::optional<Vec2> texCoord;
                    if (idx.texcoord_index >= 0 && !attrib.texcoords.empty()) {
                        float u = attrib.texcoords[2 * idx.texcoord_index + 0];
                        float v = attrib.texcoords[2 * idx.texcoord_index + 1];

                        if (flipUVs)
                            v = 1.0f - v;

                        texCoord = Vec2{u, v};
                    }

                    builder.AddVertex(position, normal, texCoord);

                    vertexMap[key] = vertexCount;
                    indices.push_back(vertexCount);
                    vertexCount++;
                }
            }

            indexOffset += faceVertCount;
        }

        // Create mesh
        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), indices);

        Logger::Info() << "Loaded mesh from " << path.filename().string()
                       << " (" << vertexCount << " vertices, "
                       << indices.size() / 3 << " triangles)";

        return mesh;
    }

    bool MeshLoader::IsSupported(const std::string& extension) {
        return extension == ".obj" || extension == ".OBJ";
    }
}