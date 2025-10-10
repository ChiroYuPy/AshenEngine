#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/BuiltIn/BuiltInShader.h"

namespace ash {

    // ========== Base Material ==========

    Material::Material(std::shared_ptr<ShaderProgram> shader)
        : m_Shader(std::move(shader)) {
    }

    void Material::SetShader(std::shared_ptr<ShaderProgram> shader) {
        m_Shader = std::move(shader);
    }

    void Material::SetFloat(const std::string& name, float value) {
        m_Properties[name] = value;
    }

    void Material::SetInt(const std::string& name, int value) {
        m_Properties[name] = value;
    }

    void Material::SetBool(const std::string& name, bool value) {
        m_Properties[name] = value;
    }

    void Material::SetVec2(const std::string& name, const Vec2& value) {
        m_Properties[name] = value;
    }

    void Material::SetVec3(const std::string& name, const Vec3& value) {
        m_Properties[name] = value;
    }

    void Material::SetVec4(const std::string& name, const Vec4& value) {
        m_Properties[name] = value;
    }

    void Material::SetMat3(const std::string& name, const Mat3& value) {
        m_Properties[name] = value;
    }

    void Material::SetMat4(const std::string& name, const Mat4& value) {
        m_Properties[name] = value;
    }

    void Material::SetTexture(const std::string& name, std::shared_ptr<Texture2D> texture) {
        m_Properties[name] = std::move(texture);
    }

    std::optional<float> Material::GetFloat(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<float>(it->second)) {
            return std::get<float>(it->second);
        }
        return std::nullopt;
    }

    std::optional<int> Material::GetInt(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<int>(it->second)) {
            return std::get<int>(it->second);
        }
        return std::nullopt;
    }

    std::optional<Vec3> Material::GetVec3(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<Vec3>(it->second)) {
            return std::get<Vec3>(it->second);
        }
        return std::nullopt;
    }

    std::optional<Vec4> Material::GetVec4(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it != m_Properties.end() && std::holds_alternative<Vec4>(it->second)) {
            return std::get<Vec4>(it->second);
        }
        return std::nullopt;
    }

    bool Material::HasProperty(const std::string& name) const {
        return m_Properties.contains(name);
    }

    void Material::ApplyProperty(const std::string& name, const MaterialValue& value) const {
        if (!m_Shader) return;

        std::visit([this, &name](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, float>) {
                m_Shader->SetFloat(name, arg);
            }
            else if constexpr (std::is_same_v<T, int>) {
                m_Shader->SetInt(name, arg);
            }
            else if constexpr (std::is_same_v<T, bool>) {
                m_Shader->SetBool(name, arg);
            }
            else if constexpr (std::is_same_v<T, Vec2>) {
                m_Shader->SetVec2(name, arg);
            }
            else if constexpr (std::is_same_v<T, Vec3>) {
                m_Shader->SetVec3(name, arg);
            }
            else if constexpr (std::is_same_v<T, Vec4>) {
                m_Shader->SetVec4(name, arg);
            }
            else if constexpr (std::is_same_v<T, Mat3>) {
                m_Shader->SetMat3(name, arg);
            }
            else if constexpr (std::is_same_v<T, Mat4>) {
                m_Shader->SetMat4(name, arg);
            }
            else if constexpr (std::is_same_v<T, std::shared_ptr<Texture2D>>) {
                if (arg) {
                    if (!m_TextureUnits.contains(name)) {
                        m_TextureUnits[name] = m_NextTextureUnit++;
                    }
                    uint32_t unit = m_TextureUnits[name];
                    arg->BindToUnit(unit);
                    m_Shader->SetInt(name, static_cast<int>(unit));
                }
            }
        }, value);
    }

    void Material::Apply() const {
        if (!m_Shader) return;

        for (const auto& [name, value] : m_Properties) {
            ApplyProperty(name, value);
        }
    }

    void Material::Bind() const {
        if (m_Shader) {
            m_Shader->Bind();
            Apply();
        }
    }

    void Material::Unbind() const {
        if (m_Shader) {
            m_Shader->Unbind();
        }
    }

    // ========== CanvasItemMaterial ==========

    CanvasItemMaterial::CanvasItemMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::CanvasItemTextured)) {
        SetAlbedo(Vec4(1.0f));
    }

    CanvasItemMaterial::CanvasItemMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec4(1.0f));
    }

    void CanvasItemMaterial::SetAlbedo(const Vec4& color) {
        SetVec4("u_Albedo", color);
    }

    void CanvasItemMaterial::SetTexture(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("u_Texture", std::move(texture));
    }

    Vec4 CanvasItemMaterial::GetAlbedo() const {
        return GetVec4("u_Albedo").value_or(Vec4(1.0f));
    }

    // ========== SpatialMaterial ==========

    SpatialMaterial::SpatialMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Spatial)) {
        SetAlbedo(Vec4(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetSpecular(0.5f);
        SetBool("u_UseAlbedoTexture", false);
    }

    SpatialMaterial::SpatialMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec4(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetSpecular(0.5f);
        SetBool("u_UseAlbedoTexture", false);
    }

    void SpatialMaterial::SetAlbedo(const Vec4& color) {
        SetVec4("u_Albedo", color);
    }

    void SpatialMaterial::SetAlbedoTexture(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("u_AlbedoTexture", std::move(texture));
        SetBool("u_UseAlbedoTexture", true);
    }

    void SpatialMaterial::SetMetallic(float value) {
        SetFloat("u_Metallic", value);
    }

    void SpatialMaterial::SetRoughness(float value) {
        SetFloat("u_Roughness", value);
    }

    void SpatialMaterial::SetSpecular(float value) {
        SetFloat("u_Specular", value);
    }

    void SpatialMaterial::SetUnshaded(bool unshaded) {
        if (unshaded) {
            SetShader(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::SpatialUnlit));
        } else {
            SetShader(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Spatial));
        }
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

    // ========== SkyMaterial ==========

    SkyMaterial::SkyMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Sky)) {
        SetSkyColor(Vec4(0.5f, 0.7f, 1.0f, 1.0f));
        SetBool("u_UseSkybox", false);
    }

    SkyMaterial::SkyMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetSkyColor(Vec4(0.5f, 0.7f, 1.0f, 1.0f));
        SetBool("u_UseSkybox", false);
    }

    void SkyMaterial::SetSkyColor(const Vec4& color) {
        SetVec4("u_SkyColor", color);
    }

    void SkyMaterial::SetCubemap(const std::shared_ptr<TextureCubeMap>& cubemap) {
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

    // ========== MaterialFactory ==========

    std::shared_ptr<CanvasItemMaterial> MaterialFactory::CreateCanvasItem(const Vec4& albedo) {
        auto material = std::make_shared<CanvasItemMaterial>();
        material->SetAlbedo(albedo);
        return material;
    }

    std::shared_ptr<CanvasItemMaterial> MaterialFactory::CreateCanvasItemTextured(std::shared_ptr<Texture2D> texture) {
        auto material = std::make_shared<CanvasItemMaterial>();
        material->SetTexture(std::move(texture));
        return material;
    }

    std::shared_ptr<SpatialMaterial> MaterialFactory::CreateSpatial(
        const Vec4& albedo,
        float metallic,
        float roughness,
        float specular
    ) {
        auto material = std::make_shared<SpatialMaterial>();
        material->SetAlbedo(albedo);
        material->SetMetallic(metallic);
        material->SetRoughness(roughness);
        material->SetSpecular(specular);
        return material;
    }

    std::shared_ptr<SpatialMaterial> MaterialFactory::CreateSpatialUnlit(const Vec4& albedo) {
        auto material = std::make_shared<SpatialMaterial>(
            BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::SpatialUnlit)
        );
        material->SetAlbedo(albedo);
        return material;
    }

    std::shared_ptr<SkyMaterial> MaterialFactory::CreateSky(const Vec4& color) {
        auto material = std::make_shared<SkyMaterial>();
        material->SetSkyColor(color);
        return material;
    }

    std::shared_ptr<SkyMaterial> MaterialFactory::CreateSkyCubemap(std::shared_ptr<TextureCubeMap> cubemap) {
        auto material = std::make_shared<SkyMaterial>();
        material->SetCubemap(std::move(cubemap));
        return material;
    }

}