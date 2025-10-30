#ifndef ASHEN_MATERIAL_H
#define ASHEN_MATERIAL_H

#include <string>
#include <unordered_map>
#include <variant>
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Math/Math.h"

namespace ash {
    using MaterialValue = Variant<float, int, bool, Vec2, Vec3, Vec4, Mat3, Mat4, Ref<Texture2D> >;

    class Material {
    public:
        explicit Material(Ref<ShaderProgram> shader);

        Material() = default;

        virtual ~Material() = default;

        void SetShader(Ref<ShaderProgram> shader);

        [[nodiscard]] const Ref<ShaderProgram> &GetShader() const { return m_Shader; }

        void SetFloat(const String &name, float value);

        void SetInt(const String &name, int value);

        void SetBool(const String &name, bool value);

        void SetVec2(const String &name, const Vec2 &value);

        void SetVec3(const String &name, const Vec3 &value);

        void SetVec4(const String &name, const Vec4 &value);

        void SetMat3(const String &name, const Mat3 &value);

        void SetMat4(const String &name, const Mat4 &value);

        void SetTexture(const String &name, Ref<Texture2D> texture);

        [[nodiscard]] Optional<float> GetFloat(const String &name) const;

        [[nodiscard]] Optional<int> GetInt(const String &name) const;

        [[nodiscard]] Optional<Vec3> GetVec3(const String &name) const;

        [[nodiscard]] Optional<Vec4> GetVec4(const String &name) const;

        [[nodiscard]] bool HasProperty(const String &name) const;

        virtual void Apply() const;

        virtual void Bind() const;

        virtual void Unbind() const;

    protected:
        Ref<ShaderProgram> m_Shader;
        HashMap<String, MaterialValue> m_Properties;

        mutable HashMap<String, uint32_t> m_TextureUnits;

        void ApplyProperty(const String &name, const MaterialValue &value) const;

        void ResetTextureUnits() const; // NOUVEAU: Méthode pour réinitialiser les unités
    };

    /**
     * @brief CanvasItemMaterial - For 2D rendering
     */
    class CanvasItemMaterial : public Material {
    public:
        CanvasItemMaterial();

        explicit CanvasItemMaterial(Ref<ShaderProgram> customShader);

        void SetAlbedo(const Vec4 &color);

        void SetTexture(Ref<Texture2D> texture);

        [[nodiscard]] Vec4 GetAlbedo() const;
    };

    /**
     * @brief SpatialMaterial - For 3D rendering with lighting
     */
    class SpatialMaterial : public Material {
    public:
        SpatialMaterial();

        explicit SpatialMaterial(Ref<ShaderProgram> customShader);

        // Albedo
        void SetAlbedo(const Vec4 &color);

        void SetAlbedoTexture(Ref<Texture2D> texture);

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

        explicit ToonMaterial(Ref<ShaderProgram> customShader);

        // Albedo
        void SetAlbedo(const Vec4 &color);

        void SetAlbedoTexture(Ref<Texture2D> texture);

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

        explicit SkyMaterial(Ref<ShaderProgram> customShader);

        void SetSkyColor(const Vec4 &color);

        void SetCubemap(const Ref<TextureCubeMap> &cubemap);

        [[nodiscard]] Vec4 GetSkyColor() const;
    };

    /**
     * @brief Material factory for creating materials
     */
    class MaterialFactory {
    public:
        // 2D Materials
        static Ref<CanvasItemMaterial> CreateCanvasItem(const Vec4 &albedo = Vec4(1.0f));

        static Ref<CanvasItemMaterial> CreateCanvasItemTextured(Ref<Texture2D> texture);

        // 3D Materials
        static Ref<SpatialMaterial> CreateSpatial(
            const Vec4 &albedo = Vec4(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f,
            float specular = 0.5f
        );

        static Ref<SpatialMaterial> CreateSpatialUnlit(const Vec4 &albedo = Vec4(1.0f));

        // Toon/Cell-Shaded Materials
        static Ref<ToonMaterial> CreateToon(
            const Vec4 &albedo = Vec4(1.0f),
            int toonLevels = 3,
            float rimAmount = 0.716f
        );

        // Environment
        static Ref<SkyMaterial> CreateSky(const Vec4 &color = Vec4(0.5f, 0.7f, 1.0f, 1.0f));

        static Ref<SkyMaterial> CreateSkyCubemap(Ref<TextureCubeMap> cubemap);
    };
}

#endif // ASHEN_MATERIAL_H
