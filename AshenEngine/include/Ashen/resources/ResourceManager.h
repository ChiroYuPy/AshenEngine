#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <memory>
#include <unordered_map>
#include <filesystem>

#include "Ashen/renderer/gfx/Shader.h"
#include "Ashen/renderer/gfx/Texture.h"
#include "Ashen/utils/FileSystem.h"
#include "Ashen/utils/ImageLoader.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Manages resource paths and working directory
     */
    class ResourcePaths {
    public:
        static ResourcePaths &Instance() {
            static ResourcePaths instance;
            return instance;
        }

        void SetWorkingDirectory(const fs::path &dir);

        [[nodiscard]] fs::path GetPath(const std::string &filename) const;

        [[nodiscard]] std::vector<fs::path> Scan(const std::vector<std::string> &extensions = {}) const;

        [[nodiscard]] const fs::path &Root() const { return m_Root; }

    private:
        ResourcePaths() = default;

        fs::path m_Root;
    };

    /**
     * @brief Base template for resource managers
     */
    template<typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        std::shared_ptr<T> Get(const std::string &id) {
            auto it = m_Resources.find(id);
            if (it != m_Resources.end())
                return it->second;
            return Load(id);
        }

        [[nodiscard]] bool Has(const std::string &id) const {
            return m_Resources.contains(id);
        }

        void Unload(const std::string &id) {
            m_Resources.erase(id);
        }

        void Clear() {
            m_Resources.clear();
        }

        [[nodiscard]] std::vector<std::string> GetLoadedResources() const {
            std::vector<std::string> ids;
            ids.reserve(m_Resources.size());
            for (const auto &[id, _]: m_Resources)
                ids.push_back(id);
            return ids;
        }

        [[nodiscard]] size_t Count() const {
            return m_Resources.size();
        }

    protected:
        virtual std::shared_ptr<T> Load(const std::string &id) = 0;

        std::unordered_map<std::string, std::shared_ptr<T> > m_Resources;
    };

    /**
     * @brief Manages shader program resources
     */
    class ShaderManager final : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager &Instance() {
            static ShaderManager instance;
            return instance;
        }

        std::shared_ptr<ShaderProgram> Load(const std::string &id) override;

        static std::vector<std::string> GetAvailableShaders();

    private:
        ShaderManager() = default;

        static std::shared_ptr<ShaderProgram> LoadShaderFromFiles(
            const fs::path &vertPath,
            const fs::path &fragPath
        );
    };

    /**
     * @brief Manages texture resources
     */
    class TextureManager final : public ResourceManager<Texture2D> {
    public:
        static TextureManager &Instance() {
            static TextureManager instance;
            return instance;
        }

        std::shared_ptr<Texture2D> Load(const std::string &id) override;

        static std::vector<std::string> GetAvailableTextures();

    private:
        TextureManager() = default;

        static fs::path FindTexturePath(const std::string &id);

        static std::shared_ptr<Texture2D> CreateTextureFromImage(const ImageData &imageData);
    };

    /**
     * @brief Central access point for all asset managers
     */
    class AssetLibrary {
    public:
        static ShaderManager &Shaders() { return ShaderManager::Instance(); }
        static TextureManager &Textures() { return TextureManager::Instance(); }

        static void Initialize();

        static void ClearAll();

    private:
        AssetLibrary() = default;

        static void LogAvailableResources();
    };
}

#endif //ASHEN_RESOURCEMANAGER_H
