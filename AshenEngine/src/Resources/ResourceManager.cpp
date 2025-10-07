#include "Ashen/Resources/ResourceManager.h"


#include "Ashen/Core/Logger.h"
#include "Ashen/Resources/Loader/MeshLoader.h"
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

    // ========== MeshManager ==========

    std::shared_ptr<Mesh> MeshManager::Load(const std::string& id) {
        if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        // Try to find mesh file
        const fs::path basePath = ResourcePaths::Instance().Root();

        for (const auto& ext : MeshLoader::GetSupportedFormats()) {
            fs::path meshPath = basePath / (id + ext);
            if (FileSystem::Exists(meshPath)) {
                return LoadFromPath(id, meshPath);
            }
        }

        throw std::runtime_error("Mesh not found: " + id);
    }

    std::shared_ptr<Mesh> MeshManager::LoadFromPath(
        const std::string& id,
        const fs::path& path,
        const bool flipUVs
    ) {
        auto mesh = std::make_shared<Mesh>(
            MeshLoader::LoadSingle(path, flipUVs)
        );

        m_Resources[id] = mesh;

        return mesh;
    }

    std::vector<std::string> MeshManager::GetAvailableMeshes() {
        return MeshLoader::ScanForMeshes(ResourcePaths::Instance().Root());
    }

    // ========== MaterialManager ==========

    std::shared_ptr<Material> MaterialManager::Create(
        const std::string& id,
        const std::string& shaderName
    ) {
        if (const auto existing = m_Resources.find(id); existing != m_Resources.end())
            return existing->second;

        auto shader = ShaderManager::Instance().Get(shaderName);
        auto material = std::make_shared<Material>(shader);

        m_Resources[id] = material;
        Logger::info() << "Created material: " + id;

        return material;
    }

    std::shared_ptr<PBRMaterial> MaterialManager::CreatePBR(
        const std::string& id,
        const std::string& shaderName
    ) {
        // Check PBR cache first
        if (const auto existing = m_PBRMaterials.find(id); existing != m_PBRMaterials.end()) {
            return existing->second;
        }

        auto shader = ShaderManager::Instance().Get(shaderName);
        auto material = std::make_shared<PBRMaterial>(shader);

        // Store in both caches
        m_PBRMaterials[id] = material;
        m_Resources[id] = material;  // Also store as base Material

        Logger::info() << "Created PBR material: " + id;

        return material;
    }

    std::shared_ptr<PBRMaterial> MaterialManager::GetPBR(const std::string& id) {
        auto it = m_PBRMaterials.find(id);
        if (it != m_PBRMaterials.end()) {
            return it->second;
        }
        return nullptr;
    }

    // ========== AssetLibrary Updates ==========

    void AssetLibrary::Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            Logger::info("Scanning resources...");
            LogAvailableResources();
        });
    }

    void AssetLibrary::LogAvailableResources() {
        const auto shaders = ShaderManager::GetAvailableShaders();
        Logger::info() << "Found " << shaders.size() << " shader(s)";

        const auto textures = TextureManager::GetAvailableTextures();
        Logger::info() << "Found " << textures.size() << " texture(s)";

        const auto meshes = MeshManager::GetAvailableMeshes();
        Logger::info() << "Found " << meshes.size() << " mesh(es)";
    }

    void AssetLibrary::ClearAll() {
        Shaders().Clear();
        Textures().Clear();
        Meshes().Clear();
        Materials().Clear();
        Logger::info("All resources cleared");
    }
}
