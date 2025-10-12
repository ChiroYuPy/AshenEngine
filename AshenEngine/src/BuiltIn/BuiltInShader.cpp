#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Core/Logger.h"

namespace ash {

    static ShaderConfig GetBuiltInShaderConfig() {
        ShaderConfig config;
        config.validateOnLink = true;
        config.detachAfterLink = true;
        config.throwOnWarning = false;
        config.cacheUniforms = true;
        config.warnOnMissingUniform = true;
        return config;
    }

    // ========== SHADERS 2D (Inchangés - Corrects) ==========

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

    // ========== SPATIAL VERTEX SHADER - CORRIGÉ ==========

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
    // ✅ Position en world space
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;

    // ✅ CORRECTION: Normal matrix correcte
    // Ne PAS normaliser ici car on perd l'échelle non-uniforme
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    v_Normal = normalMatrix * a_Normal;

    v_TexCoord = a_TexCoord;

    gl_Position = u_Proj * u_View * worldPos;
}
)";
    }

    // ========== SPATIAL FRAGMENT SHADER - CORRIGÉ ==========

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

// ✅ CORRECTION: Blinn-Phong amélioré
vec3 CalculateDirectionalLight(vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(-u_LightDirection);

    // Diffuse (Lambert)
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = NdotL * u_LightColor * u_LightEnergy;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);

    // ✅ CORRECTION: Utiliser roughness pour calculer shininess
    // Roughness 0 = shininess 256, Roughness 1 = shininess 1
    float shininess = mix(256.0, 1.0, u_Roughness);
    float spec = pow(NdotH, shininess);

    // ✅ Normalisation énergétique pour Blinn-Phong
    float normalization = (shininess + 8.0) / (8.0 * 3.14159265359);
    vec3 specular = spec * normalization * u_Specular * u_LightColor * u_LightEnergy;

    return (diffuse + specular) * albedo;
}

// ✅ CORRECTION: Atténuation physiquement plausible
vec3 CalculatePointLight(int index, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = u_PointLightPositions[index] - v_FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // ✅ CORRECTION: Atténuation inverse carrée avec smooth cutoff
    float range = u_PointLightRanges[index];

    // Atténuation physique (inverse square)
    float attenuation = 1.0 / (distance * distance + 1.0);

    // Smooth cutoff pour éviter les discontinuités
    float windowFactor = pow(max(1.0 - pow(distance / range, 4.0), 0.0), 2.0);
    attenuation *= windowFactor;

    // Diffuse
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = NdotL * u_PointLightColors[index] * u_PointLightEnergies[index];

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);

    float shininess = mix(256.0, 1.0, u_Roughness);
    float spec = pow(NdotH, shininess);

    float normalization = (shininess + 8.0) / (8.0 * 3.14159265359);
    vec3 specular = spec * normalization * u_Specular * u_PointLightColors[index] * u_PointLightEnergies[index];

    return (diffuse + specular) * albedo * attenuation;
}

void main() {
    // Get albedo
    vec3 albedo = u_Albedo.rgb;
    if (u_UseAlbedoTexture) {
        vec4 texColor = texture(u_AlbedoTexture, v_TexCoord);
        albedo *= texColor.rgb;
    }

    // ✅ CORRECTION: Normaliser la normale dans le fragment shader
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

    // ✅ CORRECTION: Tone mapping Reinhard amélioré
    // Préserve mieux les couleurs
    float luminance = dot(result, vec3(0.2126, 0.7152, 0.0722));
    float mappedLuminance = luminance / (1.0 + luminance);
    vec3 toneMapped = result * (mappedLuminance / max(luminance, 0.001));

    // ✅ Gamma correction (sRGB)
    toneMapped = pow(toneMapped, vec3(1.0 / 2.2));

    FragColor = vec4(toneMapped, u_Albedo.a);
}
)";
    }

    // ========== SPATIAL UNLIT - CORRIGÉ ==========

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

    // ✅ CORRECTION: Gamma correction pour cohérence avec les autres shaders
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));

    FragColor = color;
}
)";
    }

    // ========== TOON SHADER - CORRIGÉ ==========

    std::string BuiltInShaders::GetToonVertexShader() {
        return GetSpatialVertexShader();
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
uniform int u_ToonLevels = 3;
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

// ✅ CORRECTION: Quantification améliorée avec antialiasing
float ToonShade(float intensity) {
    // Quantifier en niveaux discrets
    float levels = float(u_ToonLevels);
    float level = floor(intensity * levels);
    float quantized = level / levels;

    // ✅ Ajouter un léger antialiasing pour éviter les artifacts
    float threshold = 0.5 / levels;
    float fract_part = fract(intensity * levels);
    float smoothed = smoothstep(0.5 - threshold, 0.5 + threshold, fract_part);

    return mix(quantized, (level + 1.0) / levels, smoothed * 0.1);
}

void main() {
    vec3 albedo = u_Albedo.rgb;
    if (u_UseAlbedoTexture) {
        albedo *= texture(u_AlbedoTexture, v_TexCoord).rgb;
    }

    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDirection);
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    // ✅ Diffuse lighting avec toon shading
    float NdotL = max(dot(normal, lightDir), 0.0);
    float toonDiffuse = ToonShade(NdotL);
    vec3 diffuse = toonDiffuse * u_LightColor * u_LightEnergy;

    // ✅ CORRECTION: Specular highlight style toon
    vec3 halfVector = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfVector), 0.0);
    float specularIntensity = pow(NdotH, u_SpecularGlossiness);

    // ✅ Seuil plus net pour l'effet toon
    float toonSpecular = step(0.5, specularIntensity);

    // ✅ Antialiasing sur le specular
    float specularEdge = 0.01;
    toonSpecular = smoothstep(0.5 - specularEdge, 0.5 + specularEdge, specularIntensity);

    vec3 specular = toonSpecular * u_LightColor * u_LightEnergy * 0.5;

    // ✅ CORRECTION: Rim lighting amélioré
    float rimDot = 1.0 - max(dot(viewDir, normal), 0.0);

    // ✅ Rim dépend de l'éclairage (n'apparaît que côté éclairé)
    float rimIntensity = rimDot * pow(NdotL, 0.5);

    // ✅ Quantification du rim pour style toon
    rimIntensity = smoothstep(u_RimAmount - u_RimThreshold, u_RimAmount + u_RimThreshold, rimIntensity);

    vec3 rim = rimIntensity * u_LightColor * u_LightEnergy * 0.5;

    // ✅ CORRECTION: Combinaison finale
    vec3 lighting = u_AmbientLight + diffuse + specular + rim;
    vec3 result = albedo * lighting;

    // ✅ Clamper pour éviter les valeurs > 1 (style toon)
    result = clamp(result, 0.0, 1.0);

    // ✅ Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, u_Albedo.a);
}
)";
    }

    // ========== SKY SHADER - CORRIGÉ ==========

    std::string BuiltInShaders::GetSkyVertexShader() {
        return R"(
#version 410 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 v_TexCoord;

void main() {
    v_TexCoord = a_Position;

    // ✅ CORRECTION: Enlever la translation de la vue
    mat4 viewNoTranslation = mat4(mat3(u_View));
    vec4 pos = u_Proj * viewNoTranslation * vec4(a_Position, 1.0);

    // ✅ Forcer z = w pour que le skybox soit toujours au fond
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
        // ✅ CORRECTION: Gamma correction pour la cubemap
        vec3 skyboxColor = texture(u_Skybox, v_TexCoord).rgb;
        skyboxColor = pow(skyboxColor, vec3(1.0 / 2.2));
        FragColor = vec4(skyboxColor, 1.0);
    } else {
        // ✅ CORRECTION: Gradient du ciel plus réaliste
        // Utiliser Y normalisé entre -1 et 1
        float t = v_TexCoord.y * 0.5 + 0.5;  // Map [-1,1] -> [0,1]
        t = pow(t, 0.7);  // Courbe non-linéaire pour plus de réalisme

        // Couleur horizon (plus clair) vers zenith (couleur du ciel)
        vec3 horizonColor = vec3(0.9, 0.95, 1.0);
        vec3 zenithColor = u_SkyColor.rgb;

        vec3 skyColor = mix(horizonColor, zenithColor, t);

        // ✅ Gamma correction
        skyColor = pow(skyColor, vec3(1.0 / 2.2));

        FragColor = vec4(skyColor, 1.0);
    }
}
)";
    }

    // ========== API Publique ==========

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
        return ShaderProgram::FromSources(vertSource, fragSource, GetBuiltInShaderConfig());
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