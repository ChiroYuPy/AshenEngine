#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/BuiltIn/BuiltInShader.h"

namespace ash {
    Material::Material(Ref<ShaderProgram> shader)
        : m_Shader(std::move(shader)) {
    }

    void Material::SetShader(Ref<ShaderProgram> shader) {
        m_Shader = std::move(shader);
    }

    void Material::SetFloat(const String &name, float value) {
        m_Properties[name] = value;
    }

    void Material::SetInt(const String &name, int value) {
        m_Properties[name] = value;
    }

    void Material::SetBool(const String &name, bool value) {
        m_Properties[name] = value;
    }

    void Material::SetVec2(const String &name, const Vec2 &value) {
        m_Properties[name] = value;
    }

    void Material::SetVec3(const String &name, const Vec3 &value) {
        m_Properties[name] = value;
    }

    void Material::SetVec4(const String &name, const Vec4 &value) {
        m_Properties[name] = value;
    }

    void Material::SetMat3(const String &name, const Mat3 &value) {
        m_Properties[name] = value;
    }

    void Material::SetMat4(const String &name, const Mat4 &value) {
        m_Properties[name] = value;
    }

    void Material::SetTexture(const String &name, Ref<Texture2D> texture) {
        m_Properties[name] = std::move(texture);
    }

    Optional<float> Material::GetFloat(const String &name) const {
        const auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<float>(it->second))
            return std::get<float>(it->second);

        return std::nullopt;
    }

    Optional<int> Material::GetInt(const String &name) const {
        const auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<int>(it->second))
            return std::get<int>(it->second);

        return std::nullopt;
    }

    Optional<Vec3> Material::GetVec3(const String &name) const {
        const auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<Vec3>(it->second))
            return std::get<Vec3>(it->second);

        return std::nullopt;
    }

    Optional<Vec4> Material::GetVec4(const String &name) const {
        const auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<Vec4>(it->second))
            return std::get<Vec4>(it->second);

        return std::nullopt;
    }

    bool Material::HasProperty(const String &name) const {
        return m_Properties.contains(name);
    }

    void Material::ResetTextureUnits() const {
        m_TextureUnits.clear();
    }

    void Material::ApplyProperty(const String &name, const MaterialValue &value) const {
        if (!m_Shader) return;

        if (!m_Shader->HasUniform(name))
            return;

        std::visit([this, &name]<typename T0>(T0 &&arg) {
            using T = std::decay_t<T0>;

            if constexpr (std::is_same_v<T, float>)
                m_Shader->SetFloat(name, arg);
            else if constexpr (std::is_same_v<T, int>)
                m_Shader->SetInt(name, arg);
            else if constexpr (std::is_same_v<T, bool>)
                m_Shader->SetBool(name, arg);
            else if constexpr (std::is_same_v<T, Vec2>)
                m_Shader->SetVec2(name, arg);
            else if constexpr (std::is_same_v<T, Vec3>)
                m_Shader->SetVec3(name, arg);
            else if constexpr (std::is_same_v<T, Vec4>)
                m_Shader->SetVec4(name, arg);
            else if constexpr (std::is_same_v<T, Mat3>)
                m_Shader->SetMat3(name, arg);
            else if constexpr (std::is_same_v<T, Mat4>)
                m_Shader->SetMat4(name, arg);
            else if constexpr (std::is_same_v<T, Ref<Texture2D> >) {
                if (arg) {
                    uint32_t unit = static_cast<uint32_t>(m_TextureUnits.size());

                    if (unit >= 16) {
                        Logger::Error() << "Too many textures in material: " << name;
                        return;
                    }

                    m_TextureUnits[name] = unit;
                    arg->BindToUnit(unit);
                    m_Shader->SetInt(name, static_cast<int>(unit));
                }
            }
        }, value);
    }

    void Material::Apply() const {
        if (!m_Shader) return;

        ResetTextureUnits();

        for (const auto &[name, value]: m_Properties)
            ApplyProperty(name, value);
    }

    void Material::Bind() const {
        if (m_Shader) {
            m_Shader->Bind();
            Apply();
        }
    }

    void Material::Unbind() const {
        if (m_Shader)
            m_Shader->Unbind();
    }

    // ========== CanvasItemMaterial ==========

    CanvasItemMaterial::CanvasItemMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::CanvasItemTextured)) {
        SetAlbedo(Vec4(1.0f));
    }

    CanvasItemMaterial::CanvasItemMaterial(Ref<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec4(1.0f));
    }

    void CanvasItemMaterial::SetAlbedo(const Vec4 &color) {
        SetVec4("u_Albedo", color);
    }

    void CanvasItemMaterial::SetTexture(Ref<Texture2D> texture) {
        Material::SetTexture("u_Texture", std::move(texture));
    }

    Vec4 CanvasItemMaterial::GetAlbedo() const {
        return GetVec4("u_Albedo").value_or(Vec4(1.0f));
    }

    SpatialMaterial::SpatialMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Spatial)) {
        SetAlbedo(Vec4(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetSpecular(0.5f);
        SetBool("u_UseAlbedoTexture", false);
    }

    SpatialMaterial::SpatialMaterial(Ref<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec4(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetSpecular(0.5f);
        SetBool("u_UseAlbedoTexture", false);
    }

    void SpatialMaterial::SetAlbedo(const Vec4 &color) {
        SetVec4("u_Albedo", color);
    }

    void SpatialMaterial::SetAlbedoTexture(Ref<Texture2D> texture) {
        SetTexture("u_AlbedoTexture", std::move(texture));
        SetBool("u_UseAlbedoTexture", true);
    }

    void SpatialMaterial::SetMetallic(const float value) {
        SetFloat("u_Metallic", value);
    }

    void SpatialMaterial::SetRoughness(const float value) {
        SetFloat("u_Roughness", value);
    }

    void SpatialMaterial::SetSpecular(const float value) {
        SetFloat("u_Specular", value);
    }

    void SpatialMaterial::SetUnshaded(const bool unshaded) {
        if (unshaded)
            SetShader(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::SpatialUnlit));
        else
            SetShader(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Spatial));
    }

    Vec4 SpatialMaterial::GetAlbedo() const {
        return GetVec4("u_Albedo").value_or(Vec4(1.0f));
    }

    float SpatialMaterial::GetMetallic() const {
        return GetFloat("u_Metallic").value_or(0.0f);
    }

    float SpatialMaterial::GetRoughness() const {
        return GetFloat("u_Roughness").value_or(0.5f);
    }

    float SpatialMaterial::GetSpecular() const {
        return GetFloat("u_Specular").value_or(0.5f);
    }

    ToonMaterial::ToonMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Toon)) {
        SetAlbedo(Vec4(1.0f));
        SetToonLevels(3);
        SetOutlineThickness(0.03f);
        SetOutlineColor(Vec3(0.0f));
        SetSpecularGlossiness(32.0f);
        SetRimAmount(0.716f);
        SetRimThreshold(0.1f);
        SetBool("u_UseAlbedoTexture", false);
    }

    ToonMaterial::ToonMaterial(Ref<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec4(1.0f));
        SetToonLevels(3);
        SetBool("u_UseAlbedoTexture", false);
    }

    void ToonMaterial::SetAlbedo(const Vec4 &color) {
        SetVec4("u_Albedo", color);
    }

    void ToonMaterial::SetAlbedoTexture(Ref<Texture2D> texture) {
        SetTexture("u_AlbedoTexture", std::move(texture));
        SetBool("u_UseAlbedoTexture", true);
    }

    void ToonMaterial::SetToonLevels(const int levels) {
        SetInt("u_ToonLevels", levels);
    }

    void ToonMaterial::SetOutlineThickness(const float thickness) {
        SetFloat("u_OutlineThickness", thickness);
    }

    void ToonMaterial::SetOutlineColor(const Vec3 &color) {
        SetVec3("u_OutlineColor", color);
    }

    void ToonMaterial::SetSpecularGlossiness(const float glossiness) {
        SetFloat("u_SpecularGlossiness", glossiness);
    }

    void ToonMaterial::SetRimAmount(const float amount) {
        SetFloat("u_RimAmount", amount);
    }

    void ToonMaterial::SetRimThreshold(const float threshold) {
        SetFloat("u_RimThreshold", threshold);
    }

    Vec4 ToonMaterial::GetAlbedo() const {
        return GetVec4("u_Albedo").value_or(Vec4(1.0f));
    }

    int ToonMaterial::GetToonLevels() const {
        return GetInt("u_ToonLevels").value_or(3);
    }

    SkyMaterial::SkyMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Sky)) {
        SetSkyColor(Vec4(0.5f, 0.7f, 1.0f, 1.0f));
        SetBool("u_UseSkybox", false);
    }

    SkyMaterial::SkyMaterial(Ref<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetSkyColor(Vec4(0.5f, 0.7f, 1.0f, 1.0f));
        SetBool("u_UseSkybox", false);
    }

    void SkyMaterial::SetSkyColor(const Vec4 &color) {
        SetVec4("u_SkyColor", color);
    }

    void SkyMaterial::SetCubemap(const Ref<TextureCubeMap> &cubemap) {
        if (cubemap && m_Shader) {
            m_Shader->Bind();
            cubemap->BindToUnit(0);
            m_Shader->SetInt("u_Skybox", 0);
            SetBool("u_UseSkybox", true);
        }
    }

    Vec4 SkyMaterial::GetSkyColor() const {
        return GetVec4("u_SkyColor").value_or(Vec4(0.5f, 0.7f, 1.0f, 1.0f));
    }

    Ref<CanvasItemMaterial> MaterialFactory::CreateCanvasItem(const Vec4 &albedo) {
        auto material = MakeRef<CanvasItemMaterial>();
        material->SetAlbedo(albedo);
        return material;
    }

    Ref<CanvasItemMaterial> MaterialFactory::CreateCanvasItemTextured(Ref<Texture2D> texture) {
        auto material = MakeRef<CanvasItemMaterial>();
        material->SetTexture(std::move(texture));
        return material;
    }

    Ref<SpatialMaterial> MaterialFactory::CreateSpatial(
        const Vec4 &albedo,
        const float metallic,
        const float roughness,
        const float specular
    ) {
        auto material = MakeRef<SpatialMaterial>();
        material->SetAlbedo(albedo);
        material->SetMetallic(metallic);
        material->SetRoughness(roughness);
        material->SetSpecular(specular);
        return material;
    }

    Ref<SpatialMaterial> MaterialFactory::CreateSpatialUnlit(const Vec4 &albedo) {
        auto material = MakeRef<SpatialMaterial>(
            BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::SpatialUnlit)
        );
        material->SetAlbedo(albedo);
        return material;
    }

    Ref<ToonMaterial> MaterialFactory::CreateToon(
        const Vec4 &albedo,
        const int toonLevels,
        const float rimAmount
    ) {
        auto material = MakeRef<ToonMaterial>();
        material->SetAlbedo(albedo);
        material->SetToonLevels(toonLevels);
        material->SetRimAmount(rimAmount);
        return material;
    }

    Ref<SkyMaterial> MaterialFactory::CreateSky(const Vec4 &color) {
        auto material = MakeRef<SkyMaterial>();
        material->SetSkyColor(color);
        return material;
    }

    Ref<SkyMaterial> MaterialFactory::CreateSkyCubemap(Ref<TextureCubeMap> cubemap) {
        auto material = MakeRef<SkyMaterial>();
        material->SetCubemap(std::move(cubemap));
        return material;
    }
}
