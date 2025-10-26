#ifndef ASHEN_MESHLOADER_H
#define ASHEN_MESHLOADER_H

#include <filesystem>

namespace ash {
    namespace fs = std::filesystem;

    // Forward declaration
    class Mesh;

    /**
     * @brief Static mesh loader - all methods are static
     * Loads 3D models from various file formats
     */
    class MeshLoader final {
    public:
        // No instantiation
        MeshLoader() = delete;

        /**
         * @brief Load mesh from file
         * Supported formats: .obj
         * @param path Path to mesh file
         * @param flipUVs Whether to flip V coordinate (default: true for OpenGL)
         * @return Loaded mesh (moved)
         * @throws std::runtime_error if file not found or loading fails
         */
        [[nodiscard]] static Mesh Load(
            const fs::path& path,
            bool flipUVs = true
        );

        /**
         * @brief Check if file extension is supported
         */
        [[nodiscard]] static bool IsSupported(const std::string& extension);
    };
}

#endif // ASHEN_MESHLOADER_H