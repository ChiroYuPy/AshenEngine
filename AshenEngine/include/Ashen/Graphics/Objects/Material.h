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
     * @brief Base material class
     */
    class Material {
    public:
        explicit Material(std::shared_ptr<ShaderProgram> shader);
        Material() = default;
        virtual ~Material() = default;

        void SetShader(std::shared_ptr<ShaderProgram> shader);
        [[nodiscard]] const std::shared_ptr<ShaderProgram>& GetShader() const { return m_Shader; }

        // Property setters
        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);
        void SetBool(const std::string& name, bool value);
        void SetVec2(const std::string& name, const Vec2& value);
        void SetVec3(const std::string& name, const Vec3& value);
        void SetVec4(const std::string& name, const Vec4& value);
        void SetMat3(const std::string& name, const Mat3& value);
        void SetMat4(const std::string& name, const Mat4& value);
        void SetTexture(const std::string& name, std::shared_ptr<Texture2D> texture);

        // Property getters
        [[nodiscard]] std::optional<float> GetFloat(const std::string& name) const;
        [[nodiscard]] std::optional<int> GetInt(const std::string& name) const;
        [[nodiscard]] std::optional<Vec3> GetVec3(const std::string& name) const;
        [[nodiscard]] std::optional<Vec4> GetVec4(const std::string& name) const;
        [[nodiscard]] bool HasProperty(const std::string& name) const;

        virtual void Apply() const;
        virtual void Bind() const;
        virtual void Unbind() const;

    protected:
        std::shared_ptr<ShaderProgram> m_Shader;
        std::unordered_map<std::string, MaterialValue> m_Properties;
        mutable std::unordered_map<std::string, uint32_t> m_TextureUnits;
        mutable uint32_t m_NextTextureUnit = 0;

        void ApplyProperty(const std::string& name, const MaterialValue& value) const;
    };

    /**
     * @brief Unlit material (no lighting)
     */
    class UnlitMaterial : public Material {
    public:
        UnlitMaterial();
        explicit UnlitMaterial(std::shared_ptr<ShaderProgram> customShader);

        void SetColor(const Vec4& color);
        void SetTexture(std::shared_ptr<Texture2D> texture);

        [[nodiscard]] Vec4 GetColor() const;
    };

    /**
     * @brief Blinn-Phong material
     */
    class BlinnPhongMaterial : public Material {
    public:
        BlinnPhongMaterial();
        explicit BlinnPhongMaterial(std::shared_ptr<ShaderProgram> customShader);

        void SetDiffuse(const Vec3& color);
        void SetSpecular(const Vec3& color);
        void SetShininess(float value);
        void SetDiffuseMap(std::shared_ptr<Texture2D> texture);

        [[nodiscard]] Vec3 GetDiffuse() const;
        [[nodiscard]] Vec3 GetSpecular() const;
        [[nodiscard]] float GetShininess() const;
    };

    /**
     * @brief PBR Material
     */
    class PBRMaterial : public Material {
    public:
        PBRMaterial();
        explicit PBRMaterial(std::shared_ptr<ShaderProgram> customShader);

        // Base properties
        void SetAlbedo(const Vec3& color);
        void SetMetallic(float value);
        void SetRoughness(float value);
        void SetEmissive(const Vec3& color);

        // Texture maps
        void SetAlbedoMap(std::shared_ptr<Texture2D> texture);
        void SetMetallicRoughnessMap(std::shared_ptr<Texture2D> texture);
        void SetNormalMap(std::shared_ptr<Texture2D> texture);
        void SetAOMap(std::shared_ptr<Texture2D> texture);
        void SetEmissiveMap(std::shared_ptr<Texture2D> texture);

        // Getters
        [[nodiscard]] Vec3 GetAlbedo() const;
        [[nodiscard]] float GetMetallic() const;
        [[nodiscard]] float GetRoughness() const;
        [[nodiscard]] Vec3 GetEmissive() const;
    };

    /**
     * @brief Skybox material
     */
    class SkyboxMaterial : public Material {
    public:
        SkyboxMaterial();
        explicit SkyboxMaterial(std::shared_ptr<ShaderProgram> customShader);

        void SetCubemap(const std::shared_ptr<TextureCubeMap> &cubemap) const;
    };

    /**
     * @brief Factory for creating materials with built-in shaders
     */
    class MaterialFactory {
    public:
        static std::shared_ptr<UnlitMaterial> CreateUnlit(const Vec4& color = Vec4(1.0f));
        static std::shared_ptr<UnlitMaterial> CreateUnlitTextured(std::shared_ptr<Texture2D> texture);

        static std::shared_ptr<BlinnPhongMaterial> CreateBlinnPhong(
            const Vec3& diffuse = Vec3(0.8f),
            const Vec3& specular = Vec3(0.5f),
            float shininess = 32.0f
        );

        static std::shared_ptr<PBRMaterial> CreatePBR(
            const Vec3& albedo = Vec3(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f
        );

        static std::shared_ptr<SkyboxMaterial> CreateSkybox(std::shared_ptr<TextureCubeMap> cubemap);
    };

}

#endif // ASHEN_MATERIAL_H