#include "Ashen/Resources/ResourceManager.h"

#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Resources/Loader/MeshLoader.h"
#include "Ashen/Resources/Loader/ShaderLoader.h"
#include "Ashen/Resources/Loader/TextureLoader.h"

namespace ash {

    // ========== ResourcePaths ==========

    void ResourcePaths::SetWorkingDirectory(const fs::path& dir) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Root = FileSystem::GetAbsolutePath(dir);
        Logger::Info() << "Working directory set to: " << m_Root.string();
    }

    fs::path ResourcePaths::GetPath(const std::string& filename) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Root / filename;
    }

    Vector<fs::path> ResourcePaths::Scan(const Vector<std::string>& extensions) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return FileSystem::ScanDirectory(m_Root, extensions, true);
    }

    // ========== ShaderManager ==========

    std::shared_ptr<ShaderProgram> ShaderManager::GetBuiltIn(const BuiltInShaders::Type type) {
        const std::string id = "__builtin__" + BuiltInShaders::GetTypeName(type);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        auto shader = BuiltInShaderManager::Instance().Get(type);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = shader;
        }

        return shader;
    }

    std::shared_ptr<ShaderProgram> ShaderManager::Load(const std::string& id) {
        const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
        const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

        return LoadFromPaths(id, vertPath, fragPath);
    }

    std::shared_ptr<ShaderProgram> ShaderManager::LoadFromPaths(
        const std::string& id,
        const fs::path& vertPath,
        const fs::path& fragPath
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        auto shader = std::make_shared<ShaderProgram>(
            ShaderLoader::Load(vertPath, fragPath)
        );

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = shader;
        }

        Logger::Info() << "Loaded shader: " << id;
        return shader;
    }

    Vector<std::string> ShaderManager::GetAvailableShaders() {
        return ShaderLoader::ScanForShaders(ResourcePaths::Instance().Root());
    }

    // ========== TextureManager ==========

    std::shared_ptr<Texture2D> TextureManager::Load(const std::string& id) {
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
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        auto texture = std::make_shared<Texture2D>(
            TextureLoader::Load2D(path, config)
        );

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = texture;
        }

        Logger::Info() << "Loaded texture: " << id;

        return texture;
    }

    Vector<std::string> TextureManager::GetAvailableTextures() {
        return TextureLoader::ScanForTextures(ResourcePaths::Instance().Root());
    }

    // ========== MeshManager ==========

    std::shared_ptr<Mesh> MeshManager::Load(const std::string& id) {
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
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        auto mesh = std::make_shared<Mesh>(
            MeshLoader::LoadSingle(path, flipUVs)
        );

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = mesh;
        }

        Logger::Info() << "Loaded mesh: " << id;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetCube() {
        const std::string id = "__primitive__cube";

        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreateCube());
        m_Resources[id] = mesh;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetSphere() {
        const std::string id = "__primitive__sphere";

        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreateSphere());
        m_Resources[id] = mesh;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetPlane() {
        const std::string id = "__primitive__plane";

        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreatePlane());
        m_Resources[id] = mesh;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetQuad() {
        const std::string id = "__primitive__quad";

        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreateQuad());
        m_Resources[id] = mesh;

        return mesh;
    }

    Vector<std::string> MeshManager::GetAvailableMeshes() {
        return MeshLoader::ScanForMeshes(ResourcePaths::Instance().Root());
    }

    // ========== MaterialManager ==========

    std::shared_ptr<CanvasItemMaterial> MaterialManager::CreateCanvasItem(
        const std::string& id,
        const Vec4& albedo
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_CanvasItemMaterials.find(id);
            if (it != m_CanvasItemMaterials.end()) {
                return it->second;
            }
        }

        auto material = MaterialFactory::CreateCanvasItem(albedo);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_CanvasItemMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created CanvasItem material: " << id;
        return material;
    }

    std::shared_ptr<CanvasItemMaterial> MaterialManager::CreateCanvasItemTextured(
        const std::string& id,
        const std::string& textureName
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_CanvasItemMaterials.find(id);
            if (it != m_CanvasItemMaterials.end()) {
                return it->second;
            }
        }

        const auto texture = TextureManager::Instance().Get(textureName);
        auto material = MaterialFactory::CreateCanvasItemTextured(texture);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_CanvasItemMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created CanvasItem textured material: " << id;
        return material;
    }

    std::shared_ptr<SpatialMaterial> MaterialManager::CreateSpatial(
        const std::string& id,
        const Vec4& albedo,
        const float metallic,
        const float roughness,
        const float specular
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_SpatialMaterials.find(id);
            if (it != m_SpatialMaterials.end()) {
                return it->second;
            }
        }

        auto material = MaterialFactory::CreateSpatial(albedo, metallic, roughness, specular);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_SpatialMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created Spatial material: " << id;
        return material;
    }

    std::shared_ptr<SpatialMaterial> MaterialManager::CreateSpatialUnlit(
        const std::string& id,
        const Vec4& albedo
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_SpatialMaterials.find(id);
            if (it != m_SpatialMaterials.end()) {
                return it->second;
            }
        }

        auto material = MaterialFactory::CreateSpatialUnlit(albedo);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_SpatialMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created Spatial Unlit material: " << id;
        return material;
    }

    std::shared_ptr<ToonMaterial> MaterialManager::CreateToon(
        const std::string& id,
        const Vec4& albedo,
        const int toonLevels,
        const float rimAmount
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_ToonMaterials.find(id);
            if (it != m_ToonMaterials.end()) {
                return it->second;
            }
        }

        auto material = MaterialFactory::CreateToon(albedo, toonLevels, rimAmount);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_ToonMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created Toon material: " << id;
        return material;
    }

    std::shared_ptr<SkyMaterial> MaterialManager::CreateSky(
        const std::string& id,
        const Vec4& color
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_SkyMaterials.find(id);
            if (it != m_SkyMaterials.end()) {
                return it->second;
            }
        }

        auto material = MaterialFactory::CreateSky(color);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_SkyMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::Info() << "Created Sky material: " << id;
        return material;
    }

    std::shared_ptr<Material> MaterialManager::CreateCustom(
        const std::string& id,
        const std::string& shaderName
    ) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            const auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        auto shader = ShaderManager::Instance().Get(shaderName);
        auto material = std::make_shared<Material>(shader);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = material;
        }

        Logger::Info() << "Created custom material: " << id;
        return material;
    }

    std::shared_ptr<CanvasItemMaterial> MaterialManager::GetCanvasItem(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_CanvasItemMaterials.find(id);
        return (it != m_CanvasItemMaterials.end()) ? it->second : nullptr;
    }

    std::shared_ptr<SpatialMaterial> MaterialManager::GetSpatial(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_SpatialMaterials.find(id);
        return (it != m_SpatialMaterials.end()) ? it->second : nullptr;
    }

    std::shared_ptr<ToonMaterial> MaterialManager::GetToon(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_ToonMaterials.find(id);
        return (it != m_ToonMaterials.end()) ? it->second : nullptr;
    }

    std::shared_ptr<SkyMaterial> MaterialManager::GetSky(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        const auto it = m_SkyMaterials.find(id);
        return (it != m_SkyMaterials.end()) ? it->second : nullptr;
    }

    // ========== AssetLibrary ==========

    void AssetLibrary::Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            Logger::Info() << "Initializing AssetLibrary...";

            // Preload built-in shaders
            BuiltInShaderManager::Instance().PreloadAll();

            LogAvailableResources();
        });
    }

    void AssetLibrary::PreloadCommon() {
        Logger::Info() << "Preloading common assets...";

        // Preload primitive meshes
        Meshes().GetCube();
        Meshes().GetSphere();
        Meshes().GetPlane();
        Meshes().GetQuad();

        Logger::Info() << "Common assets preloaded";
    }

    void AssetLibrary::LogAvailableResources() {
        const auto shaders = ShaderManager::GetAvailableShaders();
        Logger::Info() << "Found " << shaders.size() << " custom shader(s)";

        const auto textures = TextureManager::GetAvailableTextures();
        Logger::Info() << "Found " << textures.size() << " texture(s)";

        const auto meshes = MeshManager::GetAvailableMeshes();
        Logger::Info() << "Found " << meshes.size() << " mesh(es)";
    }

    void AssetLibrary::ClearAll() {
        Shaders().Clear();
        Textures().Clear();
        Meshes().Clear();
        Materials().Clear();
        BuiltInShaderManager::Instance().Clear();

        Logger::Info() << "All resources cleared";
    }

    size_t AssetLibrary::GetTotalResourceCount() {
        return Shaders().Count() +
               Textures().Count() +
               Meshes().Count() +
               Materials().Count();
    }

}