#include "Ashen/Resources/Loader/MeshLoader.h"

#include <tiny_obj_loader.h>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {

// Structure clé pour déduplication de vertex
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

} // namespace ash

namespace std {

template <>
struct hash<ash::VertexKey> {
    size_t operator()(const ash::VertexKey& key) const noexcept {
        size_t h1 = hash<int>{}(key.posIdx);
        size_t h2 = hash<int>{}(key.normalIdx);
        size_t h3 = hash<int>{}(key.texCoordIdx);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

} // namespace std

namespace ash {

Mesh MeshLoader::Load(const fs::path& path, bool flipUVs) {
    if (!FileSystem::Exists(path)) {
        throw std::runtime_error("Mesh file not found: " + path.string());
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    const std::string mtlBasedir = path.parent_path().string();

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                    path.string().c_str(),
                                    mtlBasedir.empty() ? nullptr : mtlBasedir.c_str(),
                                    true); // trianguler

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

    Logger::Info() << "Number of materials found: " << materials.size();
    std::unordered_set<std::string> materialNames;
    for (size_t i = 0; i < materials.size(); ++i) {
        Logger::Info() << "Material [" << i << "]: " << materials[i].name;
        materialNames.insert(materials[i].name);
    }

    const auto& shape = shapes[0];

    VertexAttribute attributes = VertexAttribute::Position;
    if (!attrib.normals.empty()) {
        attributes = attributes | VertexAttribute::Normal;
    }
    if (!attrib.texcoords.empty()) {
        attributes = attributes | VertexAttribute::TexCoord;
    }

    MeshBuilder builder;
    builder.WithAttributes(attributes);

    std::vector<uint32_t> indices;
    std::unordered_map<VertexKey, uint32_t> vertexMap;
    uint32_t vertexCount = 0;

    size_t indexOffset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
        const size_t faceVertCount = shape.mesh.num_face_vertices[f];
        if (faceVertCount != 3) {
            Logger::Warn() << "Non-triangular face found at index " << f << ", skipping";
            indexOffset += faceVertCount;
            continue;
        }

        int matId = shape.mesh.material_ids[f];
        if (matId < 0) {
            Logger::Warn() << "Face " << f << " uses invalid material ID: " << matId;
        } else if ((size_t)matId >= materials.size()) {
            Logger::Warn() << "Face " << f << " material ID out of range: " << matId;
        }

        for (size_t v = 0; v < 3; ++v) {
            const auto& idx = shape.mesh.indices[indexOffset + v];

            VertexKey key{idx.vertex_index, idx.normal_index, idx.texcoord_index};

            auto it = vertexMap.find(key);
            if (it != vertexMap.end()) {
                indices.push_back(it->second);
            } else {
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
                    if (flipUVs) {
                        v = 1.0f - v;
                    }
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

    Mesh mesh;
    mesh.SetData(builder.BuildVertexData(), indices);

    Logger::Info() << "Loaded mesh from " << path.filename().string()
                   << " (" << vertexCount << " vertices, "
                   << indices.size() / 3 << " triangles)";

    return mesh;
}

} // namespace ash