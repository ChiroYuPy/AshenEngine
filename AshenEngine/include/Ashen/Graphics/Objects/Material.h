#ifndef ASHEN_MATERIAL_H
#define ASHEN_MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Math/Math.h"

namespace ash {
    /**
     * @brief Material property value types
     */
    using MaterialValue = std::variant<float, int, bool, Vec2, Vec3, Vec4, Mat3, Mat4, std::shared_ptr<Texture2D>>;

    /**
     * @brief Material class managing shader and properties
     */
    class Material {
    public:
        explicit Material(std::shared_ptr<ShaderProgram> shader);
        Material() = default;
        virtual ~Material() = default;  // Virtual destructor for polymorphism

        /**
         * @brief Set the shader program
         */
        void SetShader(std::shared_ptr<ShaderProgram> shader);

        /**
         * @brief Get the shader program
         */
        [[nodiscard]] const std::shared_ptr<ShaderProgram>& GetShader() const { return m_Shader; }

        /**
         * @brief Set property values
         */
        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);
        void SetBool(const std::string& name, bool value);
        void SetVec2(const std::string& name, const Vec2& value);
        void SetVec3(const std::string& name, const Vec3& value);
        void SetVec4(const std::string& name, const Vec4& value);
        void SetMat3(const std::string& name, const Mat3& value);
        void SetMat4(const std::string& name, const Mat4& value);
        void SetTexture(const std::string& name, std::shared_ptr<Texture2D> texture);

        /**
         * @brief Get property values
         */
        [[nodiscard]] std::optional<float> GetFloat(const std::string& name) const;
        [[nodiscard]] std::optional<int> GetInt(const std::string& name) const;
        [[nodiscard]] std::optional<Vec3> GetVec3(const std::string& name) const;
        [[nodiscard]] std::optional<Vec4> GetVec4(const std::string& name) const;

        /**
         * @brief Check if property exists
         */
        [[nodiscard]] bool HasProperty(const std::string& name) const;

        /**
         * @brief Apply all material properties to shader
         */
        virtual void Apply() const;

        /**
         * @brief Bind the material (binds shader and applies properties)
         */
        virtual void Bind() const;

        /**
         * @brief Unbind the material
         */
        virtual void Unbind() const;

    protected:  // Changed from private to allow derived class access
        std::shared_ptr<ShaderProgram> m_Shader;
        std::unordered_map<std::string, MaterialValue> m_Properties;
        mutable std::unordered_map<std::string, uint32_t> m_TextureUnits;
        mutable uint32_t m_NextTextureUnit = 0;

        void ApplyProperty(const std::string& name, const MaterialValue& value) const;
    };

    /**
     * @brief PBR Material with standard properties
     */
    class PBRMaterial : public Material {
    public:
        explicit PBRMaterial(std::shared_ptr<ShaderProgram> shader);

        // Albedo/Base Color
        void SetAlbedo(const Vec3& color);
        void SetAlbedoMap(std::shared_ptr<Texture2D> texture);

        // Metallic and Roughness
        void SetMetallic(float value);
        void SetRoughness(float value);
        void SetMetallicRoughnessMap(std::shared_ptr<Texture2D> texture);

        // Normal mapping
        void SetNormalMap(std::shared_ptr<Texture2D> texture);

        // Ambient Occlusion
        void SetAOMap(std::shared_ptr<Texture2D> texture);

        // Emissive
        void SetEmissive(const Vec3& color);
        void SetEmissiveMap(std::shared_ptr<Texture2D> texture);

        [[nodiscard]] Vec3 GetAlbedo() const;
        [[nodiscard]] float GetMetallic() const;
        [[nodiscard]] float GetRoughness() const;
    };
}

#endif // ASHEN_MATERIAL_H