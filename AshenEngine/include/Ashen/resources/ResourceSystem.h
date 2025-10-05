#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <unordered_map>
#include <filesystem>
#include <mutex>

#include "Ashen/renderer/gfx/Shader.h"
#include "Ashen/renderer/gfx/Texture.h"
#include "Ashen/utils/utils.h"

namespace pixl {

    namespace fs = std::filesystem;

       class ResourcePaths {
    public:
        static ResourcePaths& Instance() {
            static ResourcePaths instance;
            return instance;
        }

        void SetWorkingDirectory(const fs::path& dir) {
            root = fs::absolute(dir);
            std::cout << "[ResourcePaths] Working directory set to: " << root << "\n";
        }

        [[nodiscard]] fs::path GetPath(const std::string& filename) const {
            return root / filename;
        }

        [[nodiscard]] std::vector<fs::path> Scan(const std::vector<std::string>& extensions = {}) const {
            std::vector<fs::path> files;
            if (!fs::exists(root) || !fs::is_directory(root))
                return files;

            for (const auto& entry : fs::recursive_directory_iterator(root)) {
                if (!entry.is_regular_file()) continue;
                if (extensions.empty()) {
                    files.push_back(entry.path());
                } else {
                    auto ext = entry.path().extension().string();
                    for (const auto& e : extensions) {
                        if (ext == e) {
                            files.push_back(entry.path());
                            break;
                        }
                    }
                }
            }
            return files;
        }

        [[nodiscard]] const fs::path& Root() const { return root; }

    private:
        ResourcePaths() = default;
        fs::path root;
    };

    template <typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        std::shared_ptr<T> Get(const std::string& id) {
            auto it = m_Resources.find(id);
            if (it != m_Resources.end())
                return it->second;
            return Load(id);
        }

        [[nodiscard]] bool Has(const std::string& id) const { return m_Resources.contains(id); }

        void Unload(const std::string& id) { m_Resources.erase(id); }

        void Clear() { m_Resources.clear(); }

        [[nodiscard]] std::vector<std::string> GetLoadedResources() const {
            std::vector<std::string> ids;
            ids.reserve(m_Resources.size());
            for (const auto& [id, _] : m_Resources)
                ids.push_back(id);
            return ids;
        }

        [[nodiscard]] size_t Count() const { return m_Resources.size(); }

    protected:
        virtual std::shared_ptr<T> Load(const std::string& id) = 0;

        std::unordered_map<std::string, std::shared_ptr<T>> m_Resources;

        static std::vector<fs::path> ScanDirectory(const fs::path& dir, const std::vector<std::string>& extensions) {
            std::vector<fs::path> files;
            if (!fs::exists(dir) || !fs::is_directory(dir)) return files;

            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (!entry.is_regular_file()) continue;
                const auto ext = entry.path().extension().string();
                for (const auto& validExt : extensions) {
                    if (ext == validExt) {
                        files.push_back(entry.path());
                        break;
                    }
                }
            }
            return files;
        }
    };

    class ShaderManager final : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager& Instance() {
            static ShaderManager instance;
            return instance;
        }

        std::shared_ptr<ShaderProgram> Load(const std::string& id) override {
            if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
                return existing->second;

            const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
            const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

            if (!fs::exists(vertPath))
                throw std::runtime_error("Vertex shader not found: " + vertPath.string());
            if (!fs::exists(fragPath))
                throw std::runtime_error("Fragment shader not found: " + fragPath.string());

            const ShaderUnit vertex(ShaderType::Vertex, readFileAsString(vertPath.string()));
            const ShaderUnit fragment(ShaderType::Fragment, readFileAsString(fragPath.string()));

            auto shader = std::make_shared<ShaderProgram>();
            shader->AttachShader(vertex);
            shader->AttachShader(fragment);
            shader->Link();

            m_Resources[id] = shader;
            std::cout << "[ShaderManager] Loaded shader: " << id << "\n";
            return shader;
        }

        static std::vector<std::string> GetAvailableShaders() {
            std::vector<std::string> shaders;
            const auto files = ScanDirectory(ResourcePaths::Instance().Root(), {".vert"});
            for (const auto& file : files) {
                std::string name = file.stem().string();
                if (fs::exists(file.parent_path() / (name + ".frag")))
                    shaders.push_back(name);
            }
            return shaders;
        }

    private:
        ShaderManager() = default;
    };

    class TextureManager final : public ResourceManager<Texture2D> {
    public:
        static TextureManager& Instance() {
            static TextureManager instance;
            return instance;
        }

        std::shared_ptr<Texture2D> Load(const std::string& id) override {
            if (auto existing = m_Resources.find(id); existing != m_Resources.end())
                return existing->second;

            fs::path texPath = ResourcePaths::Instance().GetPath(id);
            if (!fs::exists(texPath)) {
                for (const auto& ext : {".png", ".jpg", ".jpeg", ".bmp", ".tga"}) {
                    texPath = ResourcePaths::Instance().GetPath(id + ext);
                    if (fs::exists(texPath)) break;
                }
            }

            if (!fs::exists(texPath))
                throw std::runtime_error("Texture not found: " + id);

            auto texture = std::make_shared<Texture2D>();
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(texPath.string().c_str(), &width, &height, &channels, 0);
            if (!data) throw std::runtime_error("Failed to load image data");

            const GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
            texture->SetData(0, static_cast<GLint>(format), width, height, format, GL_UNSIGNED_BYTE, data);
            texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            texture->SetWrap(GL_REPEAT, GL_REPEAT);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);

            m_Resources[id] = texture;
            std::cout << "[TextureManager] Loaded texture: " << id << "\n";
            return texture;
        }

        static std::vector<std::string> GetAvailableTextures() {
            std::vector<std::string> textures;
            const auto files = ScanDirectory(ResourcePaths::Instance().Root(),
                                            {".png", ".jpg", ".jpeg", ".bmp", ".tga"});
            for (const auto& file : files) textures.push_back(file.filename().string());
            return textures;
        }

    private:
        TextureManager() = default;
    };

    class AssetLibrary {
    public:
        static ShaderManager& Shaders() { return ShaderManager::Instance(); }
        static TextureManager& Textures() { return TextureManager::Instance(); }

        static void Initialize() {
            static std::once_flag flag;
            std::call_once(flag, []() {
                std::cout << "[AssetLibrary] Scanning resources...\n";

                const auto shaders = ShaderManager::GetAvailableShaders();
                std::cout << "  Found " << shaders.size() << " shader(s): ";
                for (const auto& s : shaders) std::cout << s << " ";
                std::cout << "\n";

                const auto textures = TextureManager::GetAvailableTextures();
                std::cout << "  Found " << textures.size() << " texture(s): ";
                for (const auto& t : textures) std::cout << t << " ";
                std::cout << "\n";
            });
        }

        static void ClearAll() {
            Shaders().Clear();
            Textures().Clear();
            std::cout << "[AssetLibrary] All resources cleared\n";
        }

    private:
        AssetLibrary() = default;
    };

}

#endif //ASHEN_RESOURCEMANAGER_H
