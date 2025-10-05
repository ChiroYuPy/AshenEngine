#include "Ashen/resources/ResourceManager.h"

#include <iostream>

#include "Ashen/core/Logger.h"

namespace ash {
    // ========== ResourcePaths ==========

    void ResourcePaths::SetWorkingDirectory(const fs::path &dir) {
        m_Root = FileSystem::GetAbsolutePath(dir);
        Logger::info() << "Working directory set to: " + m_Root.string();
    }

    fs::path ResourcePaths::GetPath(const std::string &filename) const {
        return m_Root / filename;
    }

    std::vector<fs::path> ResourcePaths::Scan(const std::vector<std::string> &extensions) const {
        return FileSystem::ScanDirectory(m_Root, extensions, true);
    }

    // ========== ShaderManager ==========

    std::shared_ptr<ShaderProgram> ShaderManager::Load(const std::string &id) {
        if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
        const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

        if (!FileSystem::Exists(vertPath))
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        if (!FileSystem::Exists(fragPath))
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());

        auto shader = LoadShaderFromFiles(vertPath, fragPath);
        m_Resources[id] = shader;

        Logger::info() << "Loaded shader: " + id;
        return shader;
    }

    std::shared_ptr<ShaderProgram> ShaderManager::LoadShaderFromFiles(
        const fs::path &vertPath,
        const fs::path &fragPath
    ) {
        const std::string vertSource = FileSystem::ReadFileAsString(vertPath);
        const std::string fragSource = FileSystem::ReadFileAsString(fragPath);

        const ShaderUnit vertex(ShaderType::Vertex, vertSource);
        const ShaderUnit fragment(ShaderType::Fragment, fragSource);

        auto shader = std::make_shared<ShaderProgram>();
        shader->AttachShader(vertex);
        shader->AttachShader(fragment);
        shader->Link();

        return shader;
    }

    std::vector<std::string> ShaderManager::GetAvailableShaders() {
        std::vector<std::string> shaders;
        const auto files = FileSystem::ScanDirectory(
            ResourcePaths::Instance().Root(),
            {".vert"},
            true
        );

        for (const auto &file: files) {
            std::string name = file.stem().string();
            auto fragPath = file.parent_path() / (name + ".frag");
            if (FileSystem::Exists(fragPath)) {
                shaders.push_back(name);
            }
        }

        return shaders;
    }

    // ========== TextureManager ==========

    std::shared_ptr<Texture2D> TextureManager::Load(const std::string &id) {
        if (auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        fs::path texPath = FindTexturePath(id);
        if (texPath.empty())
            throw std::runtime_error("Texture not found: " + id);

        const ImageData imageData = ImageLoader::Load(texPath, true);
        auto texture = CreateTextureFromImage(imageData);

        m_Resources[id] = texture;
        Logger::info() << "Loaded texture: " + id;

        return texture;
    }

    fs::path TextureManager::FindTexturePath(const std::string &id) {
        fs::path texPath = ResourcePaths::Instance().GetPath(id);

        // Check if the exact path exists
        if (FileSystem::Exists(texPath))
            return texPath;

        // Try with supported extensions
        for (const auto &ext: ImageLoader::GetSupportedExtensions()) {
            texPath = ResourcePaths::Instance().GetPath(id + ext);
            if (FileSystem::Exists(texPath))
                return texPath;
        }

        return {};
    }

    std::shared_ptr<Texture2D> TextureManager::CreateTextureFromImage(const ImageData &imageData) {
        auto texture = std::make_shared<Texture2D>();

        const GLenum format = (imageData.channels == 3) ? GL_RGB : GL_RGBA;

        texture->SetData(
            0,
            static_cast<GLint>(format),
            imageData.width,
            imageData.height,
            format,
            GL_UNSIGNED_BYTE,
            imageData.pixels.data()
        );

        texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        texture->SetWrap(GL_REPEAT, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);

        return texture;
    }

    std::vector<std::string> TextureManager::GetAvailableTextures() {
        std::vector<std::string> textures;
        const auto files = FileSystem::ScanDirectory(
            ResourcePaths::Instance().Root(),
            ImageLoader::GetSupportedExtensions(),
            true
        );

        for (const auto &file: files) {
            textures.push_back(file.filename().string());
        }

        return textures;
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
        std::cout << "  Found " << shaders.size() << " shader(s): ";
        for (const auto &s: shaders) std::cout << s << " ";
        std::cout << "\n";

        const auto textures = TextureManager::GetAvailableTextures();
        std::cout << "  Found " << textures.size() << " texture(s): ";
        for (const auto &t: textures) std::cout << t << " ";
        std::cout << "\n";
    }

    void AssetLibrary::ClearAll() {
        Shaders().Clear();
        Textures().Clear();
        Logger::info("All resources cleared");
    }
}
