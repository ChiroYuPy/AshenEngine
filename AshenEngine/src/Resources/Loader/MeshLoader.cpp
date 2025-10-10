#include "Ashen/Resources/Loader/MeshLoader.h"

#include <algorithm>
#include <unordered_map>
#include <tiny_obj_loader.h>

#include "Ashen/Core/Logger.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {

    // Helper structure for vertex hashing (deduplication)
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

// Hash function for VertexKey
namespace std {
    template<>
    struct hash<ash::VertexKey> {
        size_t operator()(const ash::VertexKey& vk) const {
            size_t h1 = hash<int>{}(vk.posIdx);
            size_t h2 = hash<int>{}(vk.normalIdx);
            size_t h3 = hash<int>{}(vk.texCoordIdx);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

namespace ash {

    ModelData MeshLoader::Load(const fs::path& path, bool flipUVs) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string mtl_basedir = path.parent_path().string();
        if (!mtl_basedir.empty()) {
            mtl_basedir += "/";
        }

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                    path.string().c_str(),
                                    mtl_basedir.c_str(),
                                    true); // triangulate

        if (!warn.empty()) Logger::warn() << "OBJ Warning: " << warn;
        if (!err.empty()) Logger::error() << "OBJ Error: " << err;
        if (!ret) {
            throw std::runtime_error("Failed to load OBJ: " + path.string());
        }

        if (shapes.empty()) {
            throw std::runtime_error("No shapes found in OBJ file: " + path.string());
        }

        ModelData modelData;
        modelData.hasMultipleMeshes = shapes.size() > 1;

        // Process each shape as a separate mesh
        for (const auto& shape : shapes) {
            MeshBuilder builder;

            // Detect available attributes
            VertexAttribute attributes = VertexAttribute::Position;
            if (!attrib.normals.empty()) {
                attributes = attributes | VertexAttribute::Normal;
            }
            if (!attrib.texcoords.empty()) {
                attributes = attributes | VertexAttribute::TexCoord;
            }
            builder.WithAttributes(attributes);

            std::vector<uint32_t> indices;
            std::unordered_map<VertexKey, uint32_t> vertexMap;
            uint32_t vertexCount = 0;

            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
                size_t fv = shape.mesh.num_face_vertices[f];

                // Should already be triangulated
                if (fv != 3) {
                    Logger::warn() << "Non-triangular face with " << fv << " vertices found";
                    index_offset += fv;
                    continue;
                }

                for (size_t v = 0; v < 3; ++v) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    VertexKey key;
                    key.posIdx = idx.vertex_index;
                    key.normalIdx = idx.normal_index;
                    key.texCoordIdx = idx.texcoord_index;

                    auto it = vertexMap.find(key);
                    if (it != vertexMap.end()) {
                        indices.push_back(it->second);
                    } else {
                        // Position (always present)
                        Vec3 pos{
                            attrib.vertices[3 * idx.vertex_index + 0],
                            attrib.vertices[3 * idx.vertex_index + 1],
                            attrib.vertices[3 * idx.vertex_index + 2]
                        };

                        // Normal (optional)
                        std::optional<Vec3> normal = std::nullopt;
                        if (idx.normal_index >= 0 && !attrib.normals.empty()) {
                            normal = Vec3{
                                attrib.normals[3 * idx.normal_index + 0],
                                attrib.normals[3 * idx.normal_index + 1],
                                attrib.normals[3 * idx.normal_index + 2]
                            };
                        }

                        // UV (optional)
                        std::optional<Vec2> uv = std::nullopt;
                        if (idx.texcoord_index >= 0 && !attrib.texcoords.empty()) {
                            float u = attrib.texcoords[2 * idx.texcoord_index + 0];
                            float v = attrib.texcoords[2 * idx.texcoord_index + 1];

                            if (flipUVs) {
                                v = 1.0f - v;
                            }

                            uv = Vec2{u, v};
                        }

                        builder.AddVertex(pos, normal, uv);

                        vertexMap[key] = vertexCount;
                        indices.push_back(vertexCount);
                        vertexCount++;
                    }
                }
                index_offset += fv;
            }

            // Create mesh
            Mesh mesh;
            mesh.SetData(builder.BuildVertexData(), indices);
            modelData.meshes.push_back(std::move(mesh));

            Logger::info() << "Loaded mesh: " << shape.name
                          << " (" << vertexCount << " vertices, "
                          << indices.size() / 3 << " triangles)";
        }

        return modelData;
    }

    Mesh MeshLoader::LoadSingle(const fs::path& path, bool flipUVs) {
        ModelData data = Load(path, flipUVs);

        if (data.meshes.empty()) {
            throw std::runtime_error("No meshes loaded from: " + path.string());
        }

        if (data.hasMultipleMeshes) {
            Logger::warn() << "Multiple meshes found in " << path.filename().string()
                          << ", using first mesh only";
        }

        return std::move(data.meshes[0]);
    }

    bool MeshLoader::IsSupported(const std::string& extension) {
        static const std::vector<std::string> supported = {".obj", ".OBJ"};
        return std::ranges::find(supported, extension) != supported.end();
    }

    std::vector<std::string> MeshLoader::GetSupportedFormats() {
        return {".obj"};
    }

    std::vector<std::string> MeshLoader::ScanForMeshes(const fs::path& directory) {
        std::vector<std::string> meshes;
        auto files = FileSystem::ScanDirectory(directory, GetSupportedFormats(), true);

        for (const auto& file : files) {
            meshes.push_back(file.stem().string());
        }

        return meshes;
    }
}