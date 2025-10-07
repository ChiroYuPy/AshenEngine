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
            const size_t h1 = hash<int>{}(vk.posIdx);
            const size_t h2 = hash<int>{}(vk.normalIdx);
            const size_t h3 = hash<int>{}(vk.texCoordIdx);
            // Correction: parenthèses pour la priorité des opérateurs
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

namespace ash {

    Mesh MeshLoader::LoadSingle(const fs::path& path, const bool flipUVs) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        // Correction: Le 5ème paramètre est mtl_basedir, pas flipUVs
        // Pour charger les matériaux, spécifier le chemin du dossier
        std::string mtl_basedir = path.parent_path().string();
        if (!mtl_basedir.empty()) {
            mtl_basedir += "/";
        }

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                    path.string().c_str(),
                                    mtl_basedir.c_str());

        if (!warn.empty()) Logger::warn() << warn;
        if (!err.empty()) Logger::error() << err;
        if (!ret) throw std::runtime_error("Failed to load OBJ: " + path.string());

        // Vérifier qu'il y a des shapes
        if (shapes.empty()) {
            throw std::runtime_error("No shapes found in OBJ file: " + path.string());
        }

        MeshBuilder builder;

        // Détecter les attributs disponibles
        VertexAttribute attributes = VertexAttribute::Position;
        if (!attrib.normals.empty()) {
            attributes = attributes | VertexAttribute::Normal;
        }
        if (!attrib.texcoords.empty()) {
            attributes = attributes | VertexAttribute::TexCoord;
        }
        builder.WithAttributes(attributes);

        std::vector<uint32_t> indices;
        std::unordered_map<VertexKey, uint32_t> vertexMap; // Pour déduplication
        uint32_t vertexCount = 0;

        // Parcourir tous les shapes
        for (const auto& shape : shapes) {
            size_t index_offset = 0;

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
                size_t fv = shape.mesh.num_face_vertices[f];

                // Vérifier que c'est un triangle (tinyobjloader devrait trianguler automatiquement)
                if (fv != 3) {
                    Logger::warn() << "Non-triangular face detected with " << fv
                                   << " vertices. Consider enabling triangulation.";
                }

                for (size_t v = 0; v < fv; ++v) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    // Créer une clé pour la déduplication
                    VertexKey key;
                    key.posIdx = idx.vertex_index;
                    key.normalIdx = idx.normal_index;
                    key.texCoordIdx = idx.texcoord_index;

                    // Vérifier si ce vertex existe déjà
                    auto it = vertexMap.find(key);
                    if (it != vertexMap.end()) {
                        // Vertex existe déjà, réutiliser l'index
                        indices.push_back(it->second);
                    } else {
                        // Nouveau vertex
                        Vec3 pos{
                            attrib.vertices[3 * idx.vertex_index + 0],
                            attrib.vertices[3 * idx.vertex_index + 1],
                            attrib.vertices[3 * idx.vertex_index + 2]
                        };

                        std::optional<Vec3> normal = std::nullopt;
                        if (idx.normal_index >= 0 && !attrib.normals.empty()) {
                            normal = Vec3{
                                attrib.normals[3 * idx.normal_index + 0],
                                attrib.normals[3 * idx.normal_index + 1],
                                attrib.normals[3 * idx.normal_index + 2]
                            };
                        }

                        std::optional<Vec2> uv = std::nullopt;
                        if (idx.texcoord_index >= 0 && !attrib.texcoords.empty()) {
                            float u = attrib.texcoords[2 * idx.texcoord_index + 0];
                            float v = attrib.texcoords[2 * idx.texcoord_index + 1];

                            // Correction: flipper les UVs si demandé
                            if (flipUVs) {
                                v = 1.0f - v;
                            }

                            uv = Vec2{u, v};
                        }

                        builder.AddVertex(pos, normal, uv);

                        // Stocker l'index du nouveau vertex
                        vertexMap[key] = vertexCount;
                        indices.push_back(vertexCount);
                        vertexCount++;
                    }
                }
                index_offset += fv;
            }
        }

        Mesh mesh;
        mesh.SetData(builder.BuildVertexData(), indices);

        // Correction: utiliser vertexCount au lieu de builder.GetIndices().size()
        Logger::info() << "Loaded OBJ mesh: " << path.filename().string()
                       << " (" << vertexCount << " unique vertices, "
                       << indices.size() / 3 << " triangles)";

        return mesh;
    }

    bool MeshLoader::IsSupported(const std::string& extension) {
        static const std::vector<std::string> supported = {".obj"};
        return std::ranges::find(supported, extension) != supported.end();
    }

    std::vector<std::string> MeshLoader::GetSupportedFormats() {
        return {".obj"};
    }

    std::vector<std::string> MeshLoader::ScanForMeshes(const fs::path& directory) {
        std::vector<std::string> meshes;
        const auto files = FileSystem::ScanDirectory(directory, GetSupportedFormats(), true);

        for (const auto& file : files)
            meshes.push_back(file.filename().string());

        return meshes;
    }
}
