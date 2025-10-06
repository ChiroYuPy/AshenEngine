#ifndef ASHEN_SHADERLOADER_H
#define ASHEN_SHADERLOADER_H

#include <memory>
#include <filesystem>
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Loads shaders from files and creates ShaderProgram objects
     */
    class ShaderLoader {
    public:
        /**
         * @brief Load a shader program from vertex and fragment shader files
         */
        static ShaderProgram Load(const fs::path& vertPath, const fs::path& fragPath);

        /**
         * @brief Load a shader program with geometry shader
         */
        static ShaderProgram LoadWithGeometry(
            const fs::path& vertPath,
            const fs::path& fragPath,
            const fs::path& geomPath
        );

        /**
         * @brief Create shader program from source strings
         */
        static ShaderProgram FromSources(
            const std::string& vertSource,
            const std::string& fragSource
        );

        /**
         * @brief Check if both .vert and .frag files exist for a shader name
         */
        static bool ShaderExists(const fs::path& basePath, const std::string& shaderName);

        /**
         * @brief Scan directory for available shader pairs
         * @return List of shader names (without extensions)
         */
        static std::vector<std::string> ScanForShaders(const fs::path& directory);
    };
}

#endif // ASHEN_SHADERLOADER_H