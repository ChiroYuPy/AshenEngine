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
        Logger::info() << "Working directory set to: " << m_Root.string();
    }

    fs::path ResourcePaths::GetPath(const std::string& filename) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Root / filename;
    }

    std::vector<fs::path> ResourcePaths::Scan(const std::vector<std::string>& extensions) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return FileSystem::ScanDirectory(m_Root, extensions, true);
    }

    // ========== ShaderManager ==========

    std::shared_ptr<ShaderProgram> ShaderManager::GetBuiltIn(const BuiltInShaders::Type type) {
        const std::string id = "__builtin__" + BuiltInShaders::GetTypeName(type);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        } // 🔒 lock auto-libéré ici

        // Crée le shader sans verrou
        auto shader = BuiltInShaderManager::Instance().Get(type);

        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = shader;
        }

        return shader;
    }

    std::shared_ptr<ShaderProgram> ShaderManager::Load(const std::string& id) {
        // Get paths without holding lock
        const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
        const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

        return LoadFromPaths(id, vertPath, fragPath);
    }

    std::shared_ptr<ShaderProgram> ShaderManager::LoadFromPaths(
    const std::string& id,
    const fs::path& vertPath,
    const fs::path& fragPath
) {
        // Ne PAS acquérir le lock ici - déjà acquis par Get()
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        // Charger le shader
        auto shader = std::make_shared<ShaderProgram>(
            ShaderLoader::Load(vertPath, fragPath)
        );

        m_Resources[id] = shader;
        Logger::info() << "Loaded shader: " << id;
        return shader;
    }

    std::vector<std::string> ShaderManager::GetAvailableShaders() {
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
        // Check if already loaded
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        // Load without holding lock
        auto texture = std::make_shared<Texture2D>(
            TextureLoader::Load2D(path, config)
        );

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = texture;
        }

        Logger::info() << "Loaded texture: " << id;

        return texture;
    }

    std::vector<std::string> TextureManager::GetAvailableTextures() {
        return TextureLoader::ScanForTextures(ResourcePaths::Instance().Root());
    }

    // ========== MeshManager ==========

    std::shared_ptr<Mesh> MeshManager::Load(const std::string& id) {
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
        // Check if already loaded
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        // Load without holding lock
        auto mesh = std::make_shared<Mesh>(
            MeshLoader::LoadSingle(path, flipUVs)
        );

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = mesh;
        }

        Logger::info() << "Loaded mesh: " << id;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetCube() {
        const std::string id = "__primitive__cube";

        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        // Create without releasing lock (primitives are fast)
        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreateCube());
        m_Resources[id] = mesh;

        return mesh;
    }

    std::shared_ptr<Mesh> MeshManager::GetSphere() {
        const std::string id = "__primitive__sphere";

        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Resources.find(id);
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
        auto it = m_Resources.find(id);
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
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>(MeshPrimitives::CreateQuad());
        m_Resources[id] = mesh;

        return mesh;
    }

    std::vector<std::string> MeshManager::GetAvailableMeshes() {
        return MeshLoader::ScanForMeshes(ResourcePaths::Instance().Root());
    }

    // ========== MaterialManager ==========

    std::shared_ptr<UnlitMaterial> MaterialManager::CreateUnlit(
        const std::string& id,
        const Vec4& color
    ) {
        // Check if already exists
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_UnlitMaterials.find(id);
            if (it != m_UnlitMaterials.end()) {
                return it->second;
            }
        }

        // Create without holding lock
        auto material = MaterialFactory::CreateUnlit(color);

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_UnlitMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::info() << "Created unlit material: " << id;

        return material;
    }

    std::shared_ptr<UnlitMaterial> MaterialManager::CreateUnlitTextured(
        const std::string& id,
        const std::string& textureName
    ) {
        // Check if already exists
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_UnlitMaterials.find(id);
            if (it != m_UnlitMaterials.end()) {
                return it->second;
            }
        }

        // Load texture (doesn't need our lock)
        auto texture = TextureManager::Instance().Get(textureName);
        auto material = MaterialFactory::CreateUnlitTextured(texture);

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_UnlitMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::info() << "Created unlit textured material: " << id;

        return material;
    }

    std::shared_ptr<BlinnPhongMaterial> MaterialManager::CreateBlinnPhong(
        const std::string& id,
        const Vec3& diffuse,
        const Vec3& specular,
        const float shininess
    ) {
        // Check if already exists
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_BlinnPhongMaterials.find(id);
            if (it != m_BlinnPhongMaterials.end()) {
                return it->second;
            }
        }

        // Create without holding lock
        auto material = MaterialFactory::CreateBlinnPhong(diffuse, specular, shininess);

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_BlinnPhongMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::info() << "Created Blinn-Phong material: " << id;

        return material;
    }

    std::shared_ptr<PBRMaterial> MaterialManager::CreatePBR(
        const std::string& id,
        const Vec3& albedo,
        const float metallic,
        const float roughness
    ) {
        // Check if already exists
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_PBRMaterials.find(id);
            if (it != m_PBRMaterials.end()) {
                return it->second;
            }
        }

        // Create without holding lock
        auto material = MaterialFactory::CreatePBR(albedo, metallic, roughness);

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_PBRMaterials[id] = material;
            m_Resources[id] = material;
        }

        Logger::info() << "Created PBR material: " << id;

        return material;
    }

    std::shared_ptr<Material> MaterialManager::CreateCustom(
        const std::string& id,
        const std::string& shaderName
    ) {
        // Check if already exists
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        // Load shader (doesn't need our lock)
        auto shader = ShaderManager::Instance().Get(shaderName);
        auto material = std::make_shared<Material>(shader);

        // Store result
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Resources[id] = material;
        }

        Logger::info() << "Created custom material: " << id;

        return material;
    }

    std::shared_ptr<UnlitMaterial> MaterialManager::GetUnlit(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_UnlitMaterials.find(id);
        return (it != m_UnlitMaterials.end()) ? it->second : nullptr;
    }

    std::shared_ptr<BlinnPhongMaterial> MaterialManager::GetBlinnPhong(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_BlinnPhongMaterials.find(id);
        return (it != m_BlinnPhongMaterials.end()) ? it->second : nullptr;
    }

    std::shared_ptr<PBRMaterial> MaterialManager::GetPBR(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_PBRMaterials.find(id);
        return (it != m_PBRMaterials.end()) ? it->second : nullptr;
    }

    // ========== AssetLibrary ==========

    void AssetLibrary::Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            Logger::info() << "Initializing AssetLibrary...";

            // Preload built-in shaders
            BuiltInShaderManager::Instance().PreloadAll();

            LogAvailableResources();
        });
    }

    void AssetLibrary::PreloadCommon() {
        Logger::info() << "Preloading common assets...";

        // Preload primitive meshes
        Meshes().GetCube();
        Meshes().GetSphere();
        Meshes().GetPlane();
        Meshes().GetQuad();

        Logger::info() << "Common assets preloaded";
    }

    void AssetLibrary::LogAvailableResources() {
        const auto shaders = ShaderManager::GetAvailableShaders();
        Logger::info() << "Found " << shaders.size() << " custom shader(s)";

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
        BuiltInShaderManager::Instance().Clear();

        Logger::info() << "All resources cleared";
    }

    size_t AssetLibrary::GetTotalResourceCount() {
        return Shaders().Count() +
               Textures().Count() +
               Meshes().Count() +
               Materials().Count();
    }

} // namespace ash