#ifndef ASHEN_BUILTINSHADERS_H
#define ASHEN_BUILTINSHADERS_H

#include "Ashen/GraphicsAPI/Shader.h"

namespace ash {
    class BuiltInShaders {
    public:
        enum class Type {
            CanvasItem,
            CanvasItemTextured,

            Spatial,
            SpatialUnlit,
            Toon,

            Sky,

            MAX_TYPES
        };

        static std::pair<String, String> GetSource(Type type);

        static ShaderProgram Create(Type type);

        static String GetTypeName(Type type);

        static bool IsValid(Type type);

    private:
        static String GetCanvasItemVertexShader();

        static String GetCanvasItemColorFragmentShader();

        static String GetCanvasItemTexturedFragmentShader();

        static String GetSpatialVertexShader();

        static String GetSpatialUnlitVertexShader();

        static String GetToonVertexShader();

        static String GetSpatialFragmentShader();

        static String GetSpatialUnlitFragmentShader();

        static String GetToonFragmentShader();

        static String GetSkyVertexShader();

        static String GetSkyFragmentShader();
    };

    class BuiltInShaderManager {
    public:
        static BuiltInShaderManager &Instance() {
            static BuiltInShaderManager instance;
            return instance;
        }

        Ref<ShaderProgram> Get(BuiltInShaders::Type type);

        void Clear();

        void PreloadAll();

    private:
        BuiltInShaderManager() = default;

        HashMap<BuiltInShaders::Type, Ref<ShaderProgram> > m_Shaders;
    };
}

#endif // ASHEN_BUILTINSHADERS_H