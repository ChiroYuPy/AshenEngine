#ifndef ASHEN_SHADERLOADER_H
#define ASHEN_SHADERLOADER_H

#include <filesystem>
#include <string>

namespace ash {
    namespace fs = std::filesystem;

    // Forward declaration
    class ShaderProgram;

    /**
     * @brief Static shader loader - all methods are static
     * Compiles and links GLSL shaders from source files
     */
    class ShaderLoader final {
    public:
        // No instantiation
        ShaderLoader() = delete;

        /**
         * @brief Load vertex + fragment shader pair
         * @param vertPath Path to .vert file
         * @param fragPath Path to .frag file
         * @return Compiled and linked shader program (moved)
         */
        [[nodiscard]] static ShaderProgram Load(
            const fs::path &vertPath,
            const fs::path &fragPath
        );

        /**
         * @brief Load shader with geometry stage
         * @param vertPath Path to .vert file
         * @param fragPath Path to .frag file
         * @param geomPath Path to .geom file
         * @return Compiled and linked shader program (moved)
         */
        [[nodiscard]] static ShaderProgram LoadWithGeometry(
            const fs::path &vertPath,
            const fs::path &fragPath,
            const fs::path &geomPath
        );

        /**
         * @brief Create shader from source strings
         * @param vertSource Vertex shader GLSL code
         * @param fragSource Fragment shader GLSL code
         * @return Compiled and linked shader program (moved)
         */
        [[nodiscard]] static ShaderProgram FromSources(
            const std::string &vertSource,
            const std::string &fragSource
        );
    };
}

#endif // ASHEN_SHADERLOADER_H