#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Manages resource paths and working directory
     */
    class ResourcePaths {
    public:
        static ResourcePaths& Instance() {
            static ResourcePaths instance;
            return instance;
        }

        void SetWorkingDirectory(const fs::path& dir);
        [[nodiscard]] fs::path GetPath(const std::string& filename) const;
        [[nodiscard]] const fs::path& Root() const { return m_Root; }
        [[nodiscard]] Vector<fs::path> Scan(const Vector<std::string>& extensions) const;

    private:
        ResourcePaths() = default;
        fs::path m_Root;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Base resource manager template
     */
    template<typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        virtual std::shared_ptr<T> Get(const std::string& id) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
            return Load(id);
        }

        virtual std::shared_ptr<T> Load(const std::string& id) = 0;

        virtual void Clear() {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources.clear();
        }

        [[nodiscard]] size_t Count() const {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Resources.size();
        }

        [[nodiscard]] bool Has(const std::string& id) const {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Resources.contains(id);
        }

    protected:
        std::unordered_map<std::string, std::shared_ptr<T>> m_Resources;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Shader manager
     */
    class ShaderManager : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager& Instance() {
            static ShaderManager instance;
            return instance;
        }

        std::shared_ptr<ShaderProgram> GetBuiltIn(BuiltInShaders::Type type);
        std::shared_ptr<ShaderProgram> Load(const std::string& id) override;
        std::shared_ptr<ShaderProgram> LoadFromPaths(
            const std::string& id,
            const fs::path& vertPath,
            const fs::path& fragPath
        );

        static Vector<std::string> GetAvailableShaders();

    private:
        ShaderManager() = default;
    };

    /**
     * @brief Texture manager
     */
    class TextureManager : public ResourceManager<Texture2D> {
    public:
        static TextureManager& Instance() {
            static TextureManager instance;
            return instance;
        }

        std::shared_ptr<Texture2D> Load(const std::string& id) override;
        std::shared_ptr<Texture2D> LoadWithConfig(
            const std::string& id,
            const TextureConfig& config
        );
        std::shared_ptr<Texture2D> LoadFromPath(
            const std::string& id,
            const fs::path& path,
            const TextureConfig& config = TextureConfig::Default()
        );

        static Vector<std::string> GetAvailableTextures();

    private:
        TextureManager() = default;
    };

    /**
     * @brief Mesh manager
     */
    class MeshManager : public ResourceManager<Mesh> {
    public:
        static MeshManager& Instance() {
            static MeshManager instance;
            return instance;
        }

        std::shared_ptr<Mesh> Load(const std::string& id) override;
        std::shared_ptr<Mesh> LoadFromPath(
            const std::string& id,
            const fs::path& path,
            bool flipUVs = false
        );

        // Primitive meshes (cached)
        std::shared_ptr<Mesh> GetCube();
        std::shared_ptr<Mesh> GetSphere();
        std::shared_ptr<Mesh> GetPlane();
        std::shared_ptr<Mesh> GetQuad();

        static Vector<std::string> GetAvailableMeshes();

    private:
        MeshManager() = default;
    };

    /**
     * @brief Material manager
     */
    class MaterialManager : public ResourceManager<Material> {
    public:
        static MaterialManager& Instance() {
            static MaterialManager instance;
            return instance;
        }

        // CanvasItem materials (2D)
        std::shared_ptr<CanvasItemMaterial> CreateCanvasItem(
            const std::string& id,
            const Vec4& albedo = Vec4(1.0f)
        );

        std::shared_ptr<CanvasItemMaterial> CreateCanvasItemTextured(
            const std::string& id,
            const std::string& textureName
        );

        // Spatial materials (3D)
        std::shared_ptr<SpatialMaterial> CreateSpatial(
            const std::string& id,
            const Vec4& albedo = Vec4(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f,
            float specular = 0.5f
        );

        std::shared_ptr<SpatialMaterial> CreateSpatialUnlit(
            const std::string& id,
            const Vec4& albedo = Vec4(1.0f)
        );

        // Toon materials
        std::shared_ptr<ToonMaterial> CreateToon(
            const std::string& id,
            const Vec4& albedo = Vec4(1.0f),
            int toonLevels = 3,
            float rimAmount = 0.716f
        );

        // Sky materials
        std::shared_ptr<SkyMaterial> CreateSky(
            const std::string& id,
            const Vec4& color = Vec4(0.5f, 0.7f, 1.0f, 1.0f)
        );

        // Custom materials
        std::shared_ptr<Material> CreateCustom(
            const std::string& id,
            const std::string& shaderName
        );

        std::shared_ptr<Material> Load(const std::string& id) override {
            // Materials are created, not loaded from files
            return Get(id);
        }

        // Getters
        std::shared_ptr<CanvasItemMaterial> GetCanvasItem(const std::string& id);
        std::shared_ptr<SpatialMaterial> GetSpatial(const std::string& id);
        std::shared_ptr<ToonMaterial> GetToon(const std::string& id);
        std::shared_ptr<SkyMaterial> GetSky(const std::string& id);

        void Clear() override {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources.clear();
            m_CanvasItemMaterials.clear();
            m_SpatialMaterials.clear();
            m_ToonMaterials.clear();
            m_SkyMaterials.clear();
        }

    private:
        MaterialManager() = default;

        std::unordered_map<std::string, std::shared_ptr<CanvasItemMaterial>> m_CanvasItemMaterials;
        std::unordered_map<std::string, std::shared_ptr<SpatialMaterial>> m_SpatialMaterials;
        std::unordered_map<std::string, std::shared_ptr<ToonMaterial>> m_ToonMaterials;
        std::unordered_map<std::string, std::shared_ptr<SkyMaterial>> m_SkyMaterials;
    };

    /**
     * @brief Unified asset library interface
     */
    class AssetLibrary {
    public:
        static void Initialize();
        static void PreloadCommon();
        static void ClearAll();
        static void LogAvailableResources();
        static size_t GetTotalResourceCount();

        // Managers
        static ShaderManager& Shaders() { return ShaderManager::Instance(); }
        static TextureManager& Textures() { return TextureManager::Instance(); }
        static MeshManager& Meshes() { return MeshManager::Instance(); }
        static MaterialManager& Materials() { return MaterialManager::Instance(); }

    private:
        AssetLibrary() = delete;
    };

}

#endif // ASHEN_RESOURCEMANAGER_H