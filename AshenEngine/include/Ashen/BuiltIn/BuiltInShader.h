#ifndef ASHEN_BUILTINSHADERS_H
#define ASHEN_BUILTINSHADERS_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Ashen/GraphicsAPI/Shader.h"

namespace ash {

    /**
     * @brief Built-in shader sources embedded in the engine
     */
    class BuiltInShaders {
    public:
        enum class Type {
            Unlit,              // Simple unlit shader (color/texture)
            UnlitColor,         // Unlit with solid color
            UnlitTexture,       // Unlit with texture
            BlinnPhong,         // Classic Blinn-Phong lighting
            PBR,                // Physically Based Rendering
            Skybox,             // Skybox rendering
            PostProcess,        // Post-processing quad
            Debug,              // Debug visualization (normals, UVs, etc.)
            DebugNormals,       // Visualize normals
            DebugUVs,           // Visualize UVs
            Wireframe           // Wireframe rendering
        };

        /**
         * @brief Get shader source code for a built-in shader
         */
        static std::pair<std::string, std::string> GetSource(Type type);

        /**
         * @brief Create a shader program from built-in type
         */
        static ShaderProgram Create(Type type);

        /**
         * @brief Get shader type name as string
         */
        static std::string GetTypeName(Type type);

        /**
         * @brief Check if a shader type exists
         */
        static bool IsValid(Type type);

    private:
        // Vertex shaders
        static std::string GetUnlitVertexShader();
        static std::string GetBlinnPhongVertexShader();
        static std::string GetPBRVertexShader();
        static std::string GetSkyboxVertexShader();
        static std::string GetPostProcessVertexShader();
        static std::string GetDebugVertexShader();
        static std::string GetWireframeVertexShader();

        // Fragment shaders
        static std::string GetUnlitColorFragmentShader();
        static std::string GetUnlitTextureFragmentShader();
        static std::string GetBlinnPhongFragmentShader();
        static std::string GetPBRFragmentShader();
        static std::string GetSkyboxFragmentShader();
        static std::string GetPostProcessFragmentShader();
        static std::string GetDebugNormalsFragmentShader();
        static std::string GetDebugUVsFragmentShader();
        static std::string GetWireframeFragmentShader();
    };

    /**
     * @brief Manager for built-in shaders with caching
     */
    class BuiltInShaderManager {
    public:
        static BuiltInShaderManager& Instance() {
            static BuiltInShaderManager instance;
            return instance;
        }

        /**
         * @brief Get or create a built-in shader
         */
        std::shared_ptr<ShaderProgram> Get(BuiltInShaders::Type type);

        /**
         * @brief Clear all cached shaders
         */
        void Clear();

        /**
         * @brief Preload all built-in shaders
         */
        void PreloadAll();

    private:
        BuiltInShaderManager() = default;
        std::unordered_map<BuiltInShaders::Type, std::shared_ptr<ShaderProgram>> m_Shaders;
    };

}

#endif // ASHEN_BUILTINSHADERS_H