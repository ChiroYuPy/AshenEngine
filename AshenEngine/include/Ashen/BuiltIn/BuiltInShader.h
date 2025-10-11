#ifndef ASHEN_BUILTINSHADERS_H
#define ASHEN_BUILTINSHADERS_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Ashen/GraphicsAPI/Shader.h"

namespace ash {

    /**
     * @brief Built-in shader types
     */
    class BuiltInShaders {
    public:
        enum class Type {
            // 2D Shaders
            CanvasItem,         // Basic 2D shader for sprites/UI
            CanvasItemTextured, // Textured 2D shader

            // 3D Shaders
            Spatial,            // Basic 3D shader with Blinn-Phong lighting
            SpatialUnlit,       // Unlit 3D shader
            Toon,               // Cell-shaded/Toon shader

            // Environment
            Sky,                // Skybox shader

            MAX_TYPES
        };

        /**
         * @brief Get vertex and fragment shader source for a type
         */
        static std::pair<std::string, std::string> GetSource(Type type);

        /**
         * @brief Create a shader program from built-in type
         */
        static ShaderProgram Create(Type type);

        /**
         * @brief Get human-readable shader name
         */
        static std::string GetTypeName(Type type);

        /**
         * @brief Check if type is valid
         */
        static bool IsValid(Type type);

    private:
        // 2D Vertex Shaders
        static std::string GetCanvasItemVertexShader();

        // 2D Fragment Shaders
        static std::string GetCanvasItemColorFragmentShader();
        static std::string GetCanvasItemTexturedFragmentShader();

        // 3D Vertex Shaders
        static std::string GetSpatialVertexShader();
        static std::string GetSpatialUnlitVertexShader();
        static std::string GetToonVertexShader();

        // 3D Fragment Shaders
        static std::string GetSpatialFragmentShader();
        static std::string GetSpatialUnlitFragmentShader();
        static std::string GetToonFragmentShader();

        // Environment Shaders
        static std::string GetSkyVertexShader();
        static std::string GetSkyFragmentShader();
    };

    /**
     * @brief Shader manager with caching
     */
    class BuiltInShaderManager {
    public:
        static BuiltInShaderManager& Instance() {
            static BuiltInShaderManager instance;
            return instance;
        }

        std::shared_ptr<ShaderProgram> Get(BuiltInShaders::Type type);
        void Clear();
        void PreloadAll();

    private:
        BuiltInShaderManager() = default;
        std::unordered_map<BuiltInShaders::Type, std::shared_ptr<ShaderProgram>> m_Shaders;
    };

}

#endif // ASHEN_BUILTINSHADERS_H