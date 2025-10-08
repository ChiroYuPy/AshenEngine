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

    // ========== UnlitMaterial ==========

    UnlitMaterial::UnlitMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::UnlitTexture)) {
        SetColor(Vec4(1.0f));
    }

    UnlitMaterial::UnlitMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetColor(Vec4(1.0f));
    }

    void UnlitMaterial::SetColor(const Vec4& color) {
        SetVec4("u_Color", color);
    }

    void UnlitMaterial::SetTexture(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("u_Texture", std::move(texture));
    }

    Vec4 UnlitMaterial::GetColor() const {
        return GetVec4("u_Color").value_or(Vec4(1.0f));
    }

    // ========== BlinnPhongMaterial ==========

    BlinnPhongMaterial::BlinnPhongMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::BlinnPhong)) {
        SetDiffuse(Vec3(0.8f));
        SetSpecular(Vec3(0.5f));
        SetShininess(32.0f);
        SetBool("u_Material.hasTexture", false);
    }

    BlinnPhongMaterial::BlinnPhongMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetDiffuse(Vec3(0.8f));
        SetSpecular(Vec3(0.5f));
        SetShininess(32.0f);
        SetBool("u_Material.hasTexture", false);
    }

    void BlinnPhongMaterial::SetDiffuse(const Vec3& color) {
        SetVec3("u_Material.diffuse", color);
    }

    void BlinnPhongMaterial::SetSpecular(const Vec3& color) {
        SetVec3("u_Material.specular", color);
    }

    void BlinnPhongMaterial::SetShininess(const float value) {
        SetFloat("u_Material.shininess", value);
    }

    void BlinnPhongMaterial::SetDiffuseMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("u_Material.diffuseMap", std::move(texture));
        SetBool("u_Material.hasTexture", true);
    }

    Vec3 BlinnPhongMaterial::GetDiffuse() const {
        return GetVec3("u_Material.diffuse").value_or(Vec3(0.8f));
    }

    Vec3 BlinnPhongMaterial::GetSpecular() const {
        return GetVec3("u_Material.specular").value_or(Vec3(0.5f));
    }

    float BlinnPhongMaterial::GetShininess() const {
        return GetFloat("u_Material.shininess").value_or(32.0f);
    }

    // ========== PBRMaterial ==========

    PBRMaterial::PBRMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::PBR)) {
        SetAlbedo(Vec3(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetEmissive(Vec3(0.0f));

        // Initialize texture flags
        SetBool("material.hasAlbedoMap", false);
        SetBool("material.hasMetallicRoughnessMap", false);
        SetBool("material.hasNormalMap", false);
        SetBool("material.hasAOMap", false);
        SetBool("material.hasEmissiveMap", false);
    }

    PBRMaterial::PBRMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
        SetAlbedo(Vec3(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
        SetEmissive(Vec3(0.0f));

        SetBool("material.hasAlbedoMap", false);
        SetBool("material.hasMetallicRoughnessMap", false);
        SetBool("material.hasNormalMap", false);
        SetBool("material.hasAOMap", false);
        SetBool("material.hasEmissiveMap", false);
    }

    void PBRMaterial::SetAlbedo(const Vec3& color) {
        SetVec3("material.albedo", color);
    }

    void PBRMaterial::SetMetallic(const float value) {
        SetFloat("material.metallic", value);
    }

    void PBRMaterial::SetRoughness(const float value) {
        SetFloat("material.roughness", value);
    }

    void PBRMaterial::SetEmissive(const Vec3& color) {
        SetVec3("material.emissive", color);
    }

    void PBRMaterial::SetAlbedoMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("material.albedoMap", std::move(texture));
        SetBool("material.hasAlbedoMap", true);
    }

    void PBRMaterial::SetMetallicRoughnessMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("material.metallicRoughnessMap", std::move(texture));
        SetBool("material.hasMetallicRoughnessMap", true);
    }

    void PBRMaterial::SetNormalMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("material.normalMap", std::move(texture));
        SetBool("material.hasNormalMap", true);
    }

    void PBRMaterial::SetAOMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("material.aoMap", std::move(texture));
        SetBool("material.hasAOMap", true);
    }

    void PBRMaterial::SetEmissiveMap(std::shared_ptr<Texture2D> texture) {
        Material::SetTexture("material.emissiveMap", std::move(texture));
        SetBool("material.hasEmissiveMap", true);
    }

    Vec3 PBRMaterial::GetAlbedo() const {
        return GetVec3("material.albedo").value_or(Vec3(1.0f));
    }

    float PBRMaterial::GetMetallic() const {
        return GetFloat("material.metallic").value_or(0.0f);
    }

    float PBRMaterial::GetRoughness() const {
        return GetFloat("material.roughness").value_or(0.5f);
    }

    Vec3 PBRMaterial::GetEmissive() const {
        return GetVec3("material.emissive").value_or(Vec3(0.0f));
    }

    // ========== SkyboxMaterial ==========

    SkyboxMaterial::SkyboxMaterial()
        : Material(BuiltInShaderManager::Instance().Get(BuiltInShaders::Type::Skybox)) {
    }

    SkyboxMaterial::SkyboxMaterial(std::shared_ptr<ShaderProgram> customShader)
        : Material(std::move(customShader)) {
    }

    void SkyboxMaterial::SetCubemap(const std::shared_ptr<TextureCubeMap> &cubemap) const {
        // Note: This needs special handling since cubemap is not Texture2D
        // We'll need to extend MaterialValue to support cubemaps or handle it differently
        if (cubemap && m_Shader) {
            m_Shader->Bind();
            cubemap->BindToUnit(0);
            m_Shader->SetInt("u_Skybox", 0);
        }
    }

    // ========== MaterialFactory ==========

    std::shared_ptr<UnlitMaterial> MaterialFactory::CreateUnlit(const Vec4& color) {
        auto material = std::make_shared<UnlitMaterial>();
        material->SetColor(color);
        return material;
    }

    std::shared_ptr<UnlitMaterial> MaterialFactory::CreateUnlitTextured(std::shared_ptr<Texture2D> texture) {
        auto material = std::make_shared<UnlitMaterial>();
        material->SetTexture(std::move(texture));
        return material;
    }

    std::shared_ptr<BlinnPhongMaterial> MaterialFactory::CreateBlinnPhong(
        const Vec3& diffuse,
        const Vec3& specular,
        const float shininess
    ) {
        auto material = std::make_shared<BlinnPhongMaterial>();
        material->SetDiffuse(diffuse);
        material->SetSpecular(specular);
        material->SetShininess(shininess);
        return material;
    }

    std::shared_ptr<PBRMaterial> MaterialFactory::CreatePBR(
        const Vec3& albedo,
        const float metallic,
        const float roughness
    ) {
        auto material = std::make_shared<PBRMaterial>();
        material->SetAlbedo(albedo);
        material->SetMetallic(metallic);
        material->SetRoughness(roughness);
        return material;
    }

    std::shared_ptr<SkyboxMaterial> MaterialFactory::CreateSkybox(std::shared_ptr<TextureCubeMap> cubemap) {
        auto material = std::make_shared<SkyboxMaterial>();
        material->SetCubemap(cubemap);
        return material;
    }

}