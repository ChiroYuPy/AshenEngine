#ifndef ASHEN_MATERIAL_H
#define ASHEN_MATERIAL_H

#include <string>
#include <unordered_map>
#include <variant>
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Math/Math.h"

namespace ash {
    using MaterialValue = std::variant<float, int, bool, Vec2, Vec3, Vec4, Mat3, Mat4, std::shared_ptr<Texture2D> >;

    /**
     * @brief Base material class
     */
    class Material {
    public:
        explicit Material(std::shared_ptr<ShaderProgram> shader);

        Material() = default;

        virtual ~Material() = default;

        void SetShader(std::shared_ptr<ShaderProgram> shader);

        [[nodiscard]] const std::shared_ptr<ShaderProgram> &GetShader() const { return m_Shader; }

        // Property setters
        void SetFloat(const std::string &name, float value);

        void SetInt(const std::string &name, int value);

        void SetBool(const std::string &name, bool value);

        void SetVec2(const std::string &name, const Vec2 &value);

        void SetVec3(const std::string &name, const Vec3 &value);

        void SetVec4(const std::string &name, const Vec4 &value);

        void SetMat3(const std::string &name, const Mat3 &value);

        void SetMat4(const std::string &name, const Mat4 &value);

        void SetTexture(const std::string &name, std::shared_ptr<Texture2D> texture);

        // Property getters
        [[nodiscard]] std::optional<float> GetFloat(const std::string &name) const;

        [[nodiscard]] std::optional<int> GetInt(const std::string &name) const;

        [[nodiscard]] std::optional<Vec3> GetVec3(const std::string &name) const;

        [[nodiscard]] std::optional<Vec4> GetVec4(const std::string &name) const;

        [[nodiscard]] bool HasProperty(const std::string &name) const;

        virtual void Apply() const;

        virtual void Bind() const;

        virtual void Unbind() const;

    protected:
        std::shared_ptr<ShaderProgram> m_Shader;
        std::unordered_map<std::string, MaterialValue> m_Properties;

        // CORRECTION: Les unités de texture doivent être gérées par matériau
        // mais réinitialisées à chaque Apply() pour éviter les conflits
        mutable std::unordered_map<std::string, uint32_t> m_TextureUnits;

        void ApplyProperty(const std::string &name, const MaterialValue &value) const;

        void ResetTextureUnits() const; // NOUVEAU: Méthode pour réinitialiser les unités
    };

    /**
     * @brief CanvasItemMaterial - For 2D rendering
     */
    class CanvasItemMaterial : public Material {
    public:
        CanvasItemMaterial();

        explicit CanvasItemMaterial(std::shared_ptr<ShaderProgram> customShader);

        void SetAlbedo(const Vec4 &color);

        void SetTexture(std::shared_ptr<Texture2D> texture);

        [[nodiscard]] Vec4 GetAlbedo() const;
    };

    /**
     * @brief SpatialMaterial - For 3D rendering with lighting
     */
    class SpatialMaterial : public Material {
    public:
        SpatialMaterial();

        explicit SpatialMaterial(std::shared_ptr<ShaderProgram> customShader);

        // Albedo
        void SetAlbedo(const Vec4 &color);

        void SetAlbedoTexture(std::shared_ptr<Texture2D> texture);

        // Surface properties
        void SetMetallic(float value);

        void SetRoughness(float value);

        void SetSpecular(float value);

        // Flags
        void SetUnshaded(bool unshaded);

        // Getters
        [[nodiscard]] Vec4 GetAlbedo() const;

        [[nodiscard]] float GetMetallic() const;

        [[nodiscard]] float GetRoughness() const;

        [[nodiscard]] float GetSpecular() const;
    };

    /**
     * @brief ToonMaterial - For cell-shaded/toon rendering
     */
    class ToonMaterial : public Material {
    public:
        ToonMaterial();

        explicit ToonMaterial(std::shared_ptr<ShaderProgram> customShader);

        // Albedo
        void SetAlbedo(const Vec4 &color);

        void SetAlbedoTexture(std::shared_ptr<Texture2D> texture);

        // Toon properties
        void SetToonLevels(int levels);

        void SetOutlineThickness(float thickness);

        void SetOutlineColor(const Vec3 &color);

        void SetSpecularGlossiness(float glossiness);

        void SetRimAmount(float amount);

        void SetRimThreshold(float threshold);

        // Getters
        [[nodiscard]] Vec4 GetAlbedo() const;

        [[nodiscard]] int GetToonLevels() const;
    };

    /**
     * @brief SkyMaterial - For skybox rendering
     */
    class SkyMaterial : public Material {
    public:
        SkyMaterial();

        explicit SkyMaterial(std::shared_ptr<ShaderProgram> customShader);

        void SetSkyColor(const Vec4 &color);

        void SetCubemap(const std::shared_ptr<TextureCubeMap> &cubemap);

        [[nodiscard]] Vec4 GetSkyColor() const;
    };

    /**
     * @brief Material factory for creating materials
     */
    class MaterialFactory {
    public:
        // 2D Materials
        static std::shared_ptr<CanvasItemMaterial> CreateCanvasItem(const Vec4 &albedo = Vec4(1.0f));

        static std::shared_ptr<CanvasItemMaterial> CreateCanvasItemTextured(std::shared_ptr<Texture2D> texture);

        // 3D Materials
        static std::shared_ptr<SpatialMaterial> CreateSpatial(
            const Vec4 &albedo = Vec4(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f,
            float specular = 0.5f
        );

        static std::shared_ptr<SpatialMaterial> CreateSpatialUnlit(const Vec4 &albedo = Vec4(1.0f));

        // Toon/Cell-Shaded Materials
        static std::shared_ptr<ToonMaterial> CreateToon(
            const Vec4 &albedo = Vec4(1.0f),
            int toonLevels = 3,
            float rimAmount = 0.716f
        );

        // Environment
        static std::shared_ptr<SkyMaterial> CreateSky(const Vec4 &color = Vec4(0.5f, 0.7f, 1.0f, 1.0f));

        static std::shared_ptr<SkyMaterial> CreateSkyCubemap(std::shared_ptr<TextureCubeMap> cubemap);
    };
}

#endif // ASHEN_MATERIAL_H