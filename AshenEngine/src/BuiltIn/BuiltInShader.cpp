#include "Ashen/BuiltIn/BuiltInShader.h"

#include "Ashen/Core/Logger.h"

namespace ash {

    // ========== Vertex Shaders ==========

    std::string BuiltInShaders::GetUnlitVertexShader() {
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

    std::string BuiltInShaders::GetBlinnPhongVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat3 u_NormalMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = normalize(u_NormalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    
    gl_Position = u_Proj * u_View * worldPos;
}
)";
    }

    std::string BuiltInShaders::GetPBRVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec4 a_Tangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat3 u_NormalMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out mat3 v_TBN;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = normalize(u_NormalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    
    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(u_NormalMatrix * a_Tangent.xyz);
    vec3 N = v_Normal;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * a_Tangent.w;
    v_TBN = mat3(T, B, N);
    
    gl_Position = u_Proj * u_View * worldPos;
}
)";
    }

    std::string BuiltInShaders::GetSkyboxVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 v_TexCoord;

void main() {
    v_TexCoord = a_Position;
    vec4 pos = u_Proj * mat4(mat3(u_View)) * vec4(a_Position, 1.0);
    gl_Position = pos.xyww; // Depth will be 1.0
}
)";
    }

    std::string BuiltInShaders::GetPostProcessVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetDebugVertexShader() {
        return GetBlinnPhongVertexShader();
    }

    std::string BuiltInShaders::GetWireframeVertexShader() {
        return GetUnlitVertexShader();
    }

    // ========== Fragment Shaders ==========

    std::string BuiltInShaders::GetUnlitColorFragmentShader() {
        return R"(
#version 410 core

uniform vec4 u_Color = vec4(1.0);

out vec4 FragColor;

void main() {
    FragColor = u_Color;
}
)";
    }

    std::string BuiltInShaders::GetUnlitTextureFragmentShader() {
        return R"(
#version 410 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Color = vec4(1.0);

out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord);
    FragColor = texColor * u_Color;
}
)";
    }

    std::string BuiltInShaders::GetBlinnPhongFragmentShader() {
        return R"(
#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool u_HasTexture;
    sampler2D diffuseMap;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material u_Material;
uniform Light u_Light;
uniform vec3 u_ViewPos;

out vec4 FragColor;

void main() {
    vec3 baseColor = u_Material.diffuse;
    if (u_Material.u_HasTexture) {
        baseColor = texture(u_Material.diffuseMap, v_TexCoord).rgb;
    }

    // Ambient
    vec3 ambient = u_Light.ambient * baseColor;

    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_Light.position - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * diff * baseColor;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), u_Material.shininess);
    vec3 specular = u_Light.specular * spec * u_Material.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetPBRFragmentShader() {
        return R"(
#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in mat3 v_TBN;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    vec3 emissive;

    bool u_HasAlbedoMap;
    bool u_HasMetallicRoughnessMap;
    bool u_HasNormalMap;
    bool u_HasAOMap;
    bool u_HasEmissiveMap;

    sampler2D albedoMap;
    sampler2D metallicRoughnessMap;
    sampler2D normalMap;
    sampler2D aoMap;
    sampler2D emissiveMap;
};

uniform Material material;
uniform vec3 u_ViewPos;
uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];

out vec4 FragColor;

const float PI = 3.14159265359;

// PBR functions
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main() {
    // Sample material properties
    vec3 albedo = material.albedo;
    if (material.u_HasAlbedoMap) {
        albedo = pow(texture(material.albedoMap, v_TexCoord).rgb, vec3(2.2));
    }

    float metallic = material.metallic;
    float roughness = material.roughness;
    if (material.u_HasMetallicRoughnessMap) {
        vec3 mr = texture(material.metallicRoughnessMap, v_TexCoord).rgb;
        metallic = mr.b;
        roughness = mr.g;
    }

    float ao = 1.0;
    if (material.u_HasAOMap) {
        ao = texture(material.aoMap, v_TexCoord).r;
    }

    vec3 normal = v_Normal;
    if (material.u_HasNormalMap) {
        normal = texture(material.normalMap, v_TexCoord).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(v_TBN * normal);
    }

    vec3 N = normalize(normal);
    vec3 V = normalize(u_ViewPos - v_FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(u_LightPositions[i] - v_FragPos);
        vec3 H = normalize(V + L);
        float distance = length(u_LightPositions[i] - v_FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_LightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // Add emissive
    if (material.u_HasEmissiveMap) {
        color += texture(material.emissiveMap, v_TexCoord).rgb;
    } else {
        color += material.emissive;
    }
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetSkyboxFragmentShader() {
        return R"(
#version 410 core

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

out vec4 FragColor;

void main() {
    FragColor = texture(u_Skybox, v_TexCoord);
}
)";
    }

    std::string BuiltInShaders::GetPostProcessFragmentShader() {
        return R"(
#version 410 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main() {
    FragColor = texture(u_Texture, v_TexCoord);
}
)";
    }

    std::string BuiltInShaders::GetDebugNormalsFragmentShader() {
        return R"(
#version 410 core

in vec3 v_Normal;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(v_Normal);
    FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetDebugUVsFragmentShader() {
        return R"(
#version 410 core

in vec2 v_TexCoord;

out vec4 FragColor;

void main() {
    FragColor = vec4(v_TexCoord, 0.0, 1.0);
}
)";
    }

    std::string BuiltInShaders::GetWireframeFragmentShader() {
        return R"(
#version 410 core

uniform vec4 u_Color = vec4(0.0, 1.0, 0.0, 1.0);

out vec4 FragColor;

void main() {
    FragColor = u_Color;
}
)";
    }

    // ========== Public API ==========

    std::pair<std::string, std::string> BuiltInShaders::GetSource(const Type type) {
        switch (type) {
            case Type::UnlitColor:
                return {GetUnlitVertexShader(), GetUnlitColorFragmentShader()};
            case Type::UnlitTexture:
                return {GetUnlitVertexShader(), GetUnlitTextureFragmentShader()};
            case Type::Unlit:
                return {GetUnlitVertexShader(), GetUnlitTextureFragmentShader()};
            case Type::BlinnPhong:
                return {GetBlinnPhongVertexShader(), GetBlinnPhongFragmentShader()};
            case Type::PBR:
                return {GetPBRVertexShader(), GetPBRFragmentShader()};
            case Type::Skybox:
                return {GetSkyboxVertexShader(), GetSkyboxFragmentShader()};
            case Type::PostProcess:
                return {GetPostProcessVertexShader(), GetPostProcessFragmentShader()};
            case Type::DebugNormals:
                return {GetDebugVertexShader(), GetDebugNormalsFragmentShader()};
            case Type::DebugUVs:
                return {GetDebugVertexShader(), GetDebugUVsFragmentShader()};
            case Type::Debug:
                return {GetDebugVertexShader(), GetDebugNormalsFragmentShader()};
            case Type::Wireframe:
                return {GetWireframeVertexShader(), GetWireframeFragmentShader()};
            default:
                Logger::error() << "Unknown built-in shader type";
                return {GetUnlitVertexShader(), GetUnlitColorFragmentShader()};
        }
    }

    ShaderProgram BuiltInShaders::Create(const Type type) {
        auto [vertSource, fragSource] = GetSource(type);
        return ShaderProgram::FromSources(vertSource, fragSource);
    }

    std::string BuiltInShaders::GetTypeName(const Type type) {
        switch (type) {
            case Type::Unlit: return "Unlit";
            case Type::UnlitColor: return "UnlitColor";
            case Type::UnlitTexture: return "UnlitTexture";
            case Type::BlinnPhong: return "BlinnPhong";
            case Type::PBR: return "PBR";
            case Type::Skybox: return "Skybox";
            case Type::PostProcess: return "PostProcess";
            case Type::Debug: return "Debug";
            case Type::DebugNormals: return "DebugNormals";
            case Type::DebugUVs: return "DebugUVs";
            case Type::Wireframe: return "Wireframe";
            default: return "Unknown";
        }
    }

    bool BuiltInShaders::IsValid(Type type) {
        return static_cast<int>(type) >= 0 && 
               static_cast<int>(type) <= static_cast<int>(Type::Wireframe);
    }

    // ========== BuiltInShaderManager ==========

    std::shared_ptr<ShaderProgram> BuiltInShaderManager::Get(const BuiltInShaders::Type type) {
        const auto it = m_Shaders.find(type);
        if (it != m_Shaders.end())
            return it->second;

        auto shader = std::make_shared<ShaderProgram>(BuiltInShaders::Create(type));
        m_Shaders[type] = shader;
        
        Logger::info() << "Created built-in shader: " << BuiltInShaders::GetTypeName(type);
        
        return shader;
    }

    void BuiltInShaderManager::Clear() {
        m_Shaders.clear();
    }

    void BuiltInShaderManager::PreloadAll() {
        Get(BuiltInShaders::Type::Unlit);
        Get(BuiltInShaders::Type::UnlitColor);
        Get(BuiltInShaders::Type::UnlitTexture);
        Get(BuiltInShaders::Type::BlinnPhong);
        Get(BuiltInShaders::Type::PBR);
        Get(BuiltInShaders::Type::Skybox);
        Get(BuiltInShaders::Type::PostProcess);
        Get(BuiltInShaders::Type::DebugNormals);
        Get(BuiltInShaders::Type::DebugUVs);
        Get(BuiltInShaders::Type::Wireframe);
        
        Logger::info() << "Preloaded all built-in shaders";
    }

}