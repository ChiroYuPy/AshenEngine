#ifndef ASHEN_MESHLOADER_H
#define ASHEN_MESHLOADER_H

#include <memory>
#include <filesystem>
#include <vector>

#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Math/Math.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Result of loading a model file
     */
    struct ModelData {
        std::vector<Mesh> meshes;
        std::unordered_map<std::string, std::shared_ptr<Material>> materials;
        bool hasMultipleMeshes = false;

        [[nodiscard]] bool IsValid() const { return !meshes.empty(); }
    };

    /**
     * @brief Mesh loader supporting multiple formats
     */
    class MeshLoader {
    public:
        /**
         * @brief Load a mesh from file
         * @param path Path to model file
         * @param flipUVs Whether to flip texture coordinates vertically
         * @return ModelData containing meshes and materials
         */
        static ModelData Load(const fs::path& path, bool flipUVs = true);

        /**
         * @brief Load a single mesh from file (uses first mesh if multiple)
         */
        static Mesh LoadSingle(const fs::path& path, bool flipUVs = true);

        /**
         * @brief Check if file format is supported
         */
        static bool IsSupported(const std::string& extension);

        /**
         * @brief Get list of supported formats
         */
        static std::vector<std::string> GetSupportedFormats();

        /**
         * @brief Scan directory for mesh files
         */
        static std::vector<std::string> ScanForMeshes(const fs::path& directory);
    };
}

#endif // ASHEN_MESHLOADER_H
