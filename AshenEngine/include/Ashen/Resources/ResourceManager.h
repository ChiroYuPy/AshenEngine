#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <memory>
#include <unordered_map>
#include <filesystem>
#include <mutex>

#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Utils/FileSystem.h"

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
        [[nodiscard]] std::vector<fs::path> Scan(const std::vector<std::string>& extensions = {}) const;
        [[nodiscard]] const fs::path& Root() const { return m_Root; }

    private:
        ResourcePaths() = default;
        fs::path m_Root;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Base template for resource managers (cache layer)
     */
    template<typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        std::shared_ptr<T> Get(const std::string& id) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            auto it = m_Resources.find(id);
            if (it != m_Resources.end())
                return it->second;

            return Load(id);
        }

        void Add(const std::string& id, std::shared_ptr<T> resource) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = std::move(resource);
        }

        [[nodiscard]] bool Has(const std::string& id) const {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Resources.contains(id);
        }

        void Unload(const std::string& id) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources.erase(id);
        }

        void Clear() {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources.clear();
        }

        [[nodiscard]] std::vector<std::string> GetLoadedResources() const {
            std::lock_guard<std::mutex> lock(m_Mutex);
            std::vector<std::string> ids;
            ids.reserve(m_Resources.size());
            for (const auto& [id, _] : m_Resources)
                ids.push_back(id);
            return ids;
        }

        [[nodiscard]] size_t Count() const {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Resources.size();
        }

    protected:
        virtual std::shared_ptr<T> Load(const std::string& id) = 0;

        std::unordered_map<std::string, std::shared_ptr<T>> m_Resources;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Shader manager supporting both built-in and custom shaders
     */
    class ShaderManager final : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager& Instance() {
            static ShaderManager instance;
            return instance;
        }

        /**
         * @brief Get a built-in shader
         */
        std::shared_ptr<ShaderProgram> GetBuiltIn(BuiltInShaders::Type type);

        /**
         * @brief Load custom shader by name (looks for name.vert and name.frag)
         */
        std::shared_ptr<ShaderProgram> Load(const std::string& id) override;

        /**
         * @brief Load shader from explicit paths
         */
        std::shared_ptr<ShaderProgram> LoadFromPaths(
            const std::string& id,
            const fs::path& vertPath,
            const fs::path& fragPath
        );

        static std::vector<std::string> GetAvailableShaders();

    private:
        ShaderManager() = default;
    };

    /**
     * @brief Texture manager
     */
    class TextureManager final : public ResourceManager<Texture2D> {
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

        static std::vector<std::string> GetAvailableTextures();

    private:
        TextureManager() = default;
    };

    /**
     * @brief Mesh manager
     */
    class MeshManager final : public ResourceManager<Mesh> {
    public:
        static MeshManager& Instance() {
            static MeshManager instance;
            return instance;
        }

        std::shared_ptr<Mesh> Load(const std::string& id) override;

        std::shared_ptr<Mesh> LoadFromPath(
            const std::string& id,
            const fs::path& path,
            bool flipUVs = true
        );

        /**
         * @brief Get built-in primitive meshes
         */
        std::shared_ptr<Mesh> GetCube();
        std::shared_ptr<Mesh> GetSphere();
        std::shared_ptr<Mesh> GetPlane();
        std::shared_ptr<Mesh> GetQuad();

        static std::vector<std::string> GetAvailableMeshes();

    private:
        MeshManager() = default;
    };

    /**
     * @brief Material manager with built-in material support
     */
    class MaterialManager final : public ResourceManager<Material> {
    public:
        static MaterialManager& Instance() {
            static MaterialManager instance;
            return instance;
        }

        /**
         * @brief Create materials using built-in shaders
         */
        std::shared_ptr<UnlitMaterial> CreateUnlit(
            const std::string& id,
            const Vec4& color = Vec4(1.0f)
        );

        std::shared_ptr<UnlitMaterial> CreateUnlitTextured(
            const std::string& id,
            const std::string& textureName
        );

        std::shared_ptr<BlinnPhongMaterial> CreateBlinnPhong(
            const std::string& id,
            const Vec3& diffuse = Vec3(0.8f),
            const Vec3& specular = Vec3(0.5f),
            float shininess = 32.0f
        );

        std::shared_ptr<PBRMaterial> CreatePBR(
            const std::string& id,
            const Vec3& albedo = Vec3(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f
        );

        /**
         * @brief Create material with custom shader
         */
        std::shared_ptr<Material> CreateCustom(
            const std::string& id,
            const std::string& shaderName
        );

        /**
         * @brief Get typed materials
         */
        std::shared_ptr<UnlitMaterial> GetUnlit(const std::string& id);
        std::shared_ptr<BlinnPhongMaterial> GetBlinnPhong(const std::string& id);
        std::shared_ptr<PBRMaterial> GetPBR(const std::string& id);

        std::shared_ptr<Material> Load(const std::string& id) override {
            return nullptr; // Materials are created, not loaded
        }

    private:
        MaterialManager() = default;

        // Type-specific caches to avoid casting
        std::unordered_map<std::string, std::shared_ptr<UnlitMaterial>> m_UnlitMaterials;
        std::unordered_map<std::string, std::shared_ptr<BlinnPhongMaterial>> m_BlinnPhongMaterials;
        std::unordered_map<std::string, std::shared_ptr<PBRMaterial>> m_PBRMaterials;
    };

    /**
     * @brief Central access point for all asset managers
     */
    class AssetLibrary {
    public:
        static ShaderManager& Shaders() { return ShaderManager::Instance(); }
        static TextureManager& Textures() { return TextureManager::Instance(); }
        static MeshManager& Meshes() { return MeshManager::Instance(); }
        static MaterialManager& Materials() { return MaterialManager::Instance(); }

        /**
         * @brief Initialize the asset library and scan resources
         */
        static void Initialize();

        /**
         * @brief Preload commonly used assets
         */
        static void PreloadCommon();

        /**
         * @brief Clear all cached resources
         */
        static void ClearAll();

        /**
         * @brief Get memory usage statistics
         */
        static size_t GetTotalResourceCount();

    private:
        AssetLibrary() = default;
        static void LogAvailableResources();
    };

}

#endif // ASHEN_RESOURCEMANAGER_H