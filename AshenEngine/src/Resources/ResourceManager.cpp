#include "Ashen/Resources/ResourceManager.h"

#include <iostream>

#include "Ashen/Core/Logger.h"
#include "Ashen/Resources/Loader/ShaderLoader.h"
#include "Ashen/Resources/Loader/TextureLoader.h"

namespace ash {
    // ========== ResourcePaths ==========

    void ResourcePaths::SetWorkingDirectory(const fs::path& dir) {
        m_Root = FileSystem::GetAbsolutePath(dir);
        Logger::info() << "Working directory set to: " + m_Root.string();
    }

    fs::path ResourcePaths::GetPath(const std::string& filename) const {
        return m_Root / filename;
    }

    std::vector<fs::path> ResourcePaths::Scan(const std::vector<std::string>& extensions) const {
        return FileSystem::ScanDirectory(m_Root, extensions, true);
    }

    // ========== ShaderManager ==========

    std::shared_ptr<ShaderProgram> ShaderManager::Load(const std::string& id) {
        if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
        const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

        return LoadFromPaths(id, vertPath, fragPath);
    }

    std::shared_ptr<ShaderProgram> ShaderManager::LoadFromPaths(
        const std::string& id,
        const fs::path& vertPath,
        const fs::path& fragPath
    ) {
        auto shader = std::make_shared<ShaderProgram>(
            ShaderLoader::Load(vertPath, fragPath)
        );

        m_Resources[id] = shader;
        Logger::info() << "Loaded shader: " + id;

        return shader;
    }

    std::vector<std::string> ShaderManager::GetAvailableShaders() {
        return ShaderLoader::ScanForShaders(ResourcePaths::Instance().Root());
    }

    // ========== TextureManager ==========

    std::shared_ptr<Texture2D> TextureManager::Load(const std::string& id) {
        if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        return LoadWithConfig(id, TextureConfig::Default());
    }

    std::shared_ptr<Texture2D> TextureManager::LoadWithConfig(
        const std::string& id,
        const TextureConfig& config
    ) {
        const fs::path texPath = TextureLoader::FindTexture(
            ResourcePaths::Instance().Root(),
            id
        );

        if (texPath.empty())
            throw std::runtime_error("Texture not found: " + id);

        return LoadFromPath(id, texPath, config);
    }

    std::shared_ptr<Texture2D> TextureManager::LoadFromPath(
        const std::string& id,
        const fs::path& path,
        const TextureConfig& config
    ) {
        auto texture = std::make_shared<Texture2D>(
            TextureLoader::Load2D(path, config)
        );

        m_Resources[id] = texture;
        Logger::info() << "Loaded texture: " + id;

        return texture;
    }

    std::vector<std::string> TextureManager::GetAvailableTextures() {
        return TextureLoader::ScanForTextures(ResourcePaths::Instance().Root());
    }

    // ========== AssetLibrary ==========

    void AssetLibrary::Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            Logger::info("Scanning resources...");
            LogAvailableResources();
        });
    }

    void AssetLibrary::LogAvailableResources() {
        const auto shaders = ShaderManager::GetAvailableShaders();
        Logger::info() << "Found " << shaders.size() << " shader(s): ";

        const auto textures = TextureManager::GetAvailableTextures();
        Logger::info() << "Found " << textures.size() << " texture(s): ";
    }

    void AssetLibrary::ClearAll() {
        Shaders().Clear();
        Textures().Clear();
        Logger::info("All resources cleared");
    }
}
