#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <memory>
#include <unordered_map>
#include <filesystem>

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
    };

    /**
     * @brief Base template for resource managers (cache layer only)
     */
    template<typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        /**
         * @brief Get resource from cache or load it
         */
        std::shared_ptr<T> Get(const std::string& id) {
            auto it = m_Resources.find(id);
            if (it != m_Resources.end())
                return it->second;
            return Load(id);
        }

        /**
         * @brief Add resource to cache manually
         */
        void Add(const std::string& id, std::shared_ptr<T> resource) {
            m_Resources[id] = std::move(resource);
        }

        /**
         * @brief Check if resource is in cache
         */
        [[nodiscard]] bool Has(const std::string& id) const {
            return m_Resources.contains(id);
        }

        /**
         * @brief Remove resource from cache
         */
        void Unload(const std::string& id) {
            m_Resources.erase(id);
        }

        /**
         * @brief Clear all cached resources
         */
        void Clear() {
            m_Resources.clear();
        }

        /**
         * @brief Get list of cached resource IDs
         */
        [[nodiscard]] std::vector<std::string> GetLoadedResources() const {
            std::vector<std::string> ids;
            ids.reserve(m_Resources.size());
            for (const auto& [id, _] : m_Resources)
                ids.push_back(id);
            return ids;
        }

        /**
         * @brief Get number of cached resources
         */
        [[nodiscard]] size_t Count() const {
            return m_Resources.size();
        }

    protected:
        /**
         * @brief Load resource (to be implemented by derived classes)
         */
        virtual std::shared_ptr<T> Load(const std::string& id) = 0;

        std::unordered_map<std::string, std::shared_ptr<T>> m_Resources;
    };

    /**
     * @brief Manages shader program resources
     * Delegates loading to ShaderLoader
     */
    class ShaderManager final : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager& Instance() {
            static ShaderManager instance;
            return instance;
        }

        /**
         * @brief Load shader by name (looks for name.vert and name.frag)
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

        /**
         * @brief Get list of available shaders in resource directory
         */
        static std::vector<std::string> GetAvailableShaders();

    private:
        ShaderManager() = default;
    };

    /**
     * @brief Manages texture resources
     * Delegates loading to TextureLoader
     */
    class TextureManager final : public ResourceManager<Texture2D> {
    public:
        static TextureManager& Instance() {
            static TextureManager instance;
            return instance;
        }

        /**
         * @brief Load texture by name (finds file with supported extension)
         */
        std::shared_ptr<Texture2D> Load(const std::string& id) override;

        /**
         * @brief Load texture with specific configuration
         */
        std::shared_ptr<Texture2D> LoadWithConfig(
            const std::string& id,
            const TextureConfig& config
        );

        /**
         * @brief Load texture from explicit path
         */
        std::shared_ptr<Texture2D> LoadFromPath(
            const std::string& id,
            const fs::path& path,
            const TextureConfig& config = TextureConfig::Default()
        );

        /**
         * @brief Get list of available textures in resource directory
         */
        static std::vector<std::string> GetAvailableTextures();

    private:
        TextureManager() = default;
    };

    /**
     * @brief Central access point for all asset managers
     */
    class AssetLibrary {
    public:
        static ShaderManager& Shaders() { return ShaderManager::Instance(); }
        static TextureManager& Textures() { return TextureManager::Instance(); }

        static void Initialize();
        static void ClearAll();

    private:
        AssetLibrary() = default;
        static void LogAvailableResources();
    };
}

#endif // ASHEN_RESOURCEMANAGER_H