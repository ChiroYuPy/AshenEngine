#include "Ashen/Graphics/Objects/Material.h"

namespace ash {
    // ========== Material ==========

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
                    // Assign texture unit if not already assigned
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

    // ========== PBRMaterial ==========

    PBRMaterial::PBRMaterial(std::shared_ptr<ShaderProgram> shader)
        : Material(std::move(shader)) {
        // Set default PBR values
        SetAlbedo(Vec3(1.0f));
        SetMetallic(0.0f);
        SetRoughness(0.5f);
    }

    void PBRMaterial::SetAlbedo(const Vec3& color) {
        SetVec3("material.albedo", color);
    }

    void PBRMaterial::SetAlbedoMap(std::shared_ptr<Texture2D> texture) {
        SetTexture("material.albedoMap", std::move(texture));
        SetBool("material.hasAlbedoMap", true);
    }

    void PBRMaterial::SetMetallic(const float value) {
        SetFloat("material.metallic", value);
    }

    void PBRMaterial::SetRoughness(const float value) {
        SetFloat("material.roughness", value);
    }

    void PBRMaterial::SetMetallicRoughnessMap(std::shared_ptr<Texture2D> texture) {
        SetTexture("material.metallicRoughnessMap", std::move(texture));
        SetBool("material.hasMetallicRoughnessMap", true);
    }

    void PBRMaterial::SetNormalMap(std::shared_ptr<Texture2D> texture) {
        SetTexture("material.normalMap", std::move(texture));
        SetBool("material.hasNormalMap", true);
    }

    void PBRMaterial::SetAOMap(std::shared_ptr<Texture2D> texture) {
        SetTexture("material.aoMap", std::move(texture));
        SetBool("material.hasAOMap", true);
    }

    void PBRMaterial::SetEmissive(const Vec3& color) {
        SetVec3("material.emissive", color);
    }

    void PBRMaterial::SetEmissiveMap(std::shared_ptr<Texture2D> texture) {
        SetTexture("material.emissiveMap", std::move(texture));
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
}