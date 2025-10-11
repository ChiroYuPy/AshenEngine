#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Core/Logger.h"

namespace ash {

    // ========== 2D Shaders ==========

    std::string BuiltInShaders::GetCanvasItemVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetCanvasItemColorFragmentShader() {
        return R"(
#version 410 core

uniform vec4 u_Albedo = vec4(1.0);

out vec4 FragColor;

void main() {
    FragColor = u_Albedo;
}
)";
    }

    std::string BuiltInShaders::GetCanvasItemTexturedFragmentShader() {
        return R"(
#version 410 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Albedo = vec4(1.0);

out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord);
    FragColor = texColor * u_Albedo;
}
)";
    }

    // ========== 3D Shaders ==========

    std::string BuiltInShaders::GetSpatialVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);

    v_TexCoord = a_TexCoord;

    gl_Position = u_Proj * u_View * worldPos;
}
)";
    }

    std::string BuiltInShaders::GetSpatialFragmentShader() {
        return R"(
#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

// Material properties
uniform vec4 u_Albedo = vec4(1.0);
uniform sampler2D u_AlbedoTexture;
uniform bool u_UseAlbedoTexture = false;

uniform float u_Metallic = 0.0;
uniform float u_Roughness = 0.5;
uniform float u_Specular = 0.5;

// Lighting
uniform vec3 u_CameraPos;
uniform vec3 u_AmbientLight = vec3(0.03);

// Directional light
uniform vec3 u_LightDirection = vec3(-0.5, -1.0, -0.3);
uniform vec3 u_LightColor = vec3(1.0);
uniform float u_LightEnergy = 1.0;

// Point lights (max 4)
uniform int u_PointLightCount = 0;
uniform vec3 u_PointLightPositions[4];
uniform vec3 u_PointLightColors[4];
uniform float u_PointLightEnergies[4];
uniform float u_PointLightRanges[4];

out vec4 FragColor;

vec3 CalculateDirectionalLight(vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(-u_LightDirection);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor * u_LightEnergy;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0 * (1.0 - u_Roughness));
    vec3 specular = spec * u_Specular * u_LightColor * u_LightEnergy;

    return (diffuse + specular) * albedo;
}

vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(u_PointLightPositions[index] - v_FragPos);
    float distance = length(u_PointLightPositions[index] - v_FragPos);

    // Attenuation
    float range = u_PointLightRanges[index];
    float attenuation = clamp(1.0 - (distance / range), 0.0, 1.0);
    attenuation = attenuation * attenuation;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_PointLightColors[index] * u_PointLightEnergies[index];

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0 * (1.0 - u_Roughness));
    vec3 specular = spec * u_Specular * u_PointLightColors[index] * u_PointLightEnergies[index];

    return (diffuse + specular) * albedo * attenuation;
}

void main() {
    // Get albedo
    vec3 albedo = u_Albedo.rgb;
    if (u_UseAlbedoTexture) {
        vec4 texColor = texture(u_AlbedoTexture, v_TexCoord);
        albedo *= texColor.rgb;
    }

    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    // Ambient
    vec3 ambient = u_AmbientLight * albedo;

    // Directional light
    vec3 lighting = CalculateDirectionalLight(normal, viewDir, albedo);

    // Point lights
    for (int i = 0; i < u_PointLightCount && i < 4; i++) {
        lighting += CalculatePointLight(i, normal, viewDir, albedo);
    }

    vec3 result = ambient + lighting;

    // Simple tone mapping
    result = result / (result + vec3(1.0));

    FragColor = vec4(result, u_Albedo.a);
}
)";
    }

    std::string BuiltInShaders::GetSpatialUnlitVertexShader() {
        return GetCanvasItemVertexShader();
    }

    std::string BuiltInShaders::GetSpatialUnlitFragmentShader() {
        return R"(
#version 410 core

in vec2 v_TexCoord;

uniform vec4 u_Albedo = vec4(1.0);
uniform sampler2D u_AlbedoTexture;
uniform bool u_UseAlbedoTexture = false;

out vec4 FragColor;

void main() {
    vec4 color = u_Albedo;

    if (u_UseAlbedoTexture) {
        color *= texture(u_AlbedoTexture, v_TexCoord);
    }

    FragColor = color;
}
)";
    }

    // ========== Toon/Cell-Shaded Shader ==========

    std::string BuiltInShaders::GetToonVertexShader() {
        return GetSpatialVertexShader(); // Same as spatial
    }

    std::string BuiltInShaders::GetToonFragmentShader() {
        return R"(
#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

// Material
uniform vec4 u_Albedo = vec4(1.0);
uniform sampler2D u_AlbedoTexture;
uniform bool u_UseAlbedoTexture = false;

// Toon parameters
uniform int u_ToonLevels = 3;          // Number of discrete shading levels
uniform float u_OutlineThickness = 0.03;
uniform vec3 u_OutlineColor = vec3(0.0, 0.0, 0.0);
uniform float u_SpecularGlossiness = 32.0;
uniform float u_RimAmount = 0.716;
uniform float u_RimThreshold = 0.1;

// Lighting
uniform vec3 u_CameraPos;
uniform vec3 u_AmbientLight = vec3(0.1);
uniform vec3 u_LightDirection = vec3(-0.5, -1.0, -0.3);
uniform vec3 u_LightColor = vec3(1.0);
uniform float u_LightEnergy = 1.0;

out vec4 FragColor;

float ToonShade(float intensity) {
    float level = floor(intensity * float(u_ToonLevels));
    return level / float(u_ToonLevels);
}

void main() {
    vec3 albedo = u_Albedo.rgb;
    if (u_UseAlbedoTexture) {
        albedo *= texture(u_AlbedoTexture, v_TexCoord).rgb;
    }

    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDirection);
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    // Diffuse lighting with toon shading
    float NdotL = max(dot(normal, lightDir), 0.0);
    float toonDiffuse = ToonShade(NdotL);
    vec3 diffuse = toonDiffuse * u_LightColor * u_LightEnergy;

    // Specular highlight (toon style)
    vec3 halfVector = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfVector), 0.0);
    float specularIntensity = pow(NdotH, u_SpecularGlossiness);
    float toonSpecular = smoothstep(0.005, 0.01, specularIntensity);
    vec3 specular = toonSpecular * u_LightColor * u_LightEnergy;

    // Rim lighting
    float rimDot = 1.0 - dot(viewDir, normal);
    float rimIntensity = rimDot * NdotL;
    rimIntensity = smoothstep(u_RimAmount - 0.01, u_RimAmount + 0.01, rimIntensity);
    vec3 rim = rimIntensity * u_LightColor * u_LightEnergy;

    // Combine lighting
    vec3 lighting = u_AmbientLight + diffuse + specular + rim;
    vec3 result = albedo * lighting;

    FragColor = vec4(result, u_Albedo.a);
}
)";
    }

    // ========== Environment Shaders ==========

    std::string BuiltInShaders::GetSkyVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 v_TexCoord;

void main() {
    v_TexCoord = a_Position;
    vec4 pos = u_Proj * mat4(mat3(u_View)) * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}
)";
    }

    std::string BuiltInShaders::GetSkyFragmentShader() {
        return R"(
#version 410 core

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;
uniform vec4 u_SkyColor = vec4(0.5, 0.7, 1.0, 1.0);
uniform bool u_UseSkybox = false;

out vec4 FragColor;

void main() {
    if (u_UseSkybox) {
        FragColor = texture(u_Skybox, v_TexCoord);
    } else {
        float t = v_TexCoord.y * 0.5 + 0.5;
        FragColor = mix(vec4(1.0), u_SkyColor, t);
    }
}
)";
    }

    // ========== Public API ==========

    std::pair<std::string, std::string> BuiltInShaders::GetSource(Type type) {
        switch (type) {
            case Type::CanvasItem:
                return {GetCanvasItemVertexShader(), GetCanvasItemColorFragmentShader()};
            case Type::CanvasItemTextured:
                return {GetCanvasItemVertexShader(), GetCanvasItemTexturedFragmentShader()};
            case Type::Spatial:
                return {GetSpatialVertexShader(), GetSpatialFragmentShader()};
            case Type::SpatialUnlit:
                return {GetSpatialUnlitVertexShader(), GetSpatialUnlitFragmentShader()};
            case Type::Toon:
                return {GetToonVertexShader(), GetToonFragmentShader()};
            case Type::Sky:
                return {GetSkyVertexShader(), GetSkyFragmentShader()};
            default:
                Logger::Error() << "Unknown built-in shader type";
                return {GetCanvasItemVertexShader(), GetCanvasItemColorFragmentShader()};
        }
    }

    ShaderProgram BuiltInShaders::Create(Type type) {
        auto [vertSource, fragSource] = GetSource(type);
        return ShaderProgram::FromSources(vertSource, fragSource);
    }

    std::string BuiltInShaders::GetTypeName(Type type) {
        switch (type) {
            case Type::CanvasItem: return "CanvasItem";
            case Type::CanvasItemTextured: return "CanvasItemTextured";
            case Type::Spatial: return "Spatial";
            case Type::SpatialUnlit: return "SpatialUnlit";
            case Type::Toon: return "Toon";
            case Type::Sky: return "Sky";
            default: return "Unknown";
        }
    }

    bool BuiltInShaders::IsValid(Type type) {
        return static_cast<int>(type) >= 0 &&
               static_cast<int>(type) < static_cast<int>(Type::MAX_TYPES);
    }

    // ========== Shader Manager ==========

    std::shared_ptr<ShaderProgram> BuiltInShaderManager::Get(BuiltInShaders::Type type) {
        const auto it = m_Shaders.find(type);
        if (it != m_Shaders.end())
            return it->second;

        auto shader = std::make_shared<ShaderProgram>(BuiltInShaders::Create(type));
        m_Shaders[type] = shader;

        Logger::Info() << "Created built-in shader: " << BuiltInShaders::GetTypeName(type);

        return shader;
    }

    void BuiltInShaderManager::Clear() {
        m_Shaders.clear();
    }

    void BuiltInShaderManager::PreloadAll() {
        Get(BuiltInShaders::Type::CanvasItem);
        Get(BuiltInShaders::Type::CanvasItemTextured);
        Get(BuiltInShaders::Type::Spatial);
        Get(BuiltInShaders::Type::SpatialUnlit);
        Get(BuiltInShaders::Type::Toon);
        Get(BuiltInShaders::Type::Sky);
        
        Logger::Info() << "Preloaded all built-in shaders";
    }

}