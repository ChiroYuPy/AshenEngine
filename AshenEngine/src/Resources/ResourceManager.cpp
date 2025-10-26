#include "Ashen/Resources/ResourceManager.h"

#include "Ashen/BuiltIn/BuiltInShader.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Resources/Loader/MeshLoader.h"
#include "Ashen/Resources/Loader/ShaderLoader.h"
#include "Ashen/Resources/Loader/TextureLoader.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {

    // ========== ResourcePaths ==========

    ResourcePaths& ResourcePaths::Instance() {
        static ResourcePaths instance;
        return instance;
    }

    void ResourcePaths::SetWorkingDirectory(const fs::path& dir) {
        std::lock_guard lock(m_Mutex);
        m_Root = FileSystem::GetAbsolutePath(dir);
        Logger::Debug() << "Resource root set to: " << m_Root.string();
    }

    fs::path ResourcePaths::GetPath(const std::string& filename) const {
        std::lock_guard lock(m_Mutex);
        return m_Root / filename;
    }

    const fs::path& ResourcePaths::Root() const {
        std::lock_guard lock(m_Mutex);
        return m_Root;
    }

    // ========== ShaderManager ==========

    ShaderManager& ShaderManager::Instance() {
        static ShaderManager instance;
        return instance;
    }

    Ref<ShaderProgram> ShaderManager::GetBuiltIn(BuiltIn type) {
        // Map to BuiltInShaders enum
        BuiltInShaders::Type builtInType;
        switch (type) {
            case BuiltIn::CanvasItem: builtInType = BuiltInShaders::Type::CanvasItem; break;
            case BuiltIn::CanvasItemTextured: builtInType = BuiltInShaders::Type::CanvasItemTextured; break;
            case BuiltIn::Spatial: builtInType = BuiltInShaders::Type::Spatial; break;
            case BuiltIn::SpatialUnlit: builtInType = BuiltInShaders::Type::SpatialUnlit; break;
            case BuiltIn::Toon: builtInType = BuiltInShaders::Type::Toon; break;
            case BuiltIn::Sky: builtInType = BuiltInShaders::Type::Sky; break;
            default:
                throw std::invalid_argument("Invalid built-in shader type");
        }

        const std::string id = "__builtin_" + BuiltInShaders::GetTypeName(builtInType);

        // Check cache
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Resources.find(id);
            if (it != m_Resources.end()) {
                return it->second;
            }
        }

        // Create built-in shader
        auto shader = BuiltInShaderManager::Instance().Get(builtInType);
        Cache(id, shader);

        return shader;
    }

    Ref<ShaderProgram> ShaderManager::Load(const std::string& id) {
        const auto vertPath = ResourcePaths::Instance().GetPath(id + ".vert");
        const auto fragPath = ResourcePaths::Instance().GetPath(id + ".frag");

        if (!FileSystem::Exists(vertPath)) {
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        }
        if (!FileSystem::Exists(fragPath)) {
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());
        }

        auto shader = MakeRef<ShaderProgram>(
            ShaderLoader::Load(vertPath, fragPath)
        );
        Cache(id, shader);

        Logger::Info() << "Loaded shader: " << id;
        return shader;
    }

    void ShaderManager::Clear() {
        ResourceManager::Clear();
        BuiltInShaderManager::Instance().Clear();
        Logger::Debug("Cleared all shaders");
    }

    // ========== TextureManager ==========

    TextureManager& TextureManager::Instance() {
        static TextureManager instance;
        return instance;
    }

    Ref<Texture2D> TextureManager::Load(const std::string& id) {
        const fs::path basePath = ResourcePaths::Instance().Root();
        const fs::path texPath = TextureLoader::FindTexture(basePath, id);

        if (texPath.empty()) {
            throw std::runtime_error("Texture not found: " + id);
        }

        auto texture = MakeRef<Texture2D>(
            TextureLoader::Load2D(texPath, TextureConfig::Default())
        );
        Cache(id, texture);

        Logger::Info() << "Loaded texture: " << id;
        return texture;
    }

    // ========== MeshManager ==========

    MeshManager& MeshManager::Instance() {
        static MeshManager instance;
        return instance;
    }

    Ref<Mesh> MeshManager::Load(const std::string& id) {
        const fs::path basePath = ResourcePaths::Instance().Root();

        // Try .obj extension
        fs::path meshPath = basePath / (id + ".obj");
        if (!FileSystem::Exists(meshPath)) {
            throw std::runtime_error("Mesh not found: " + id);
        }

        auto mesh = MakeRef<Mesh>(
            MeshLoader::Load(meshPath)
        );
        Cache(id, mesh);

        Logger::Info() << "Loaded mesh: " << id;
        return mesh;
    }

    Ref<Mesh> MeshManager::GetCube() {
        const std::string id = "__primitive_cube";

        std::lock_guard lock(m_Mutex);
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = MakeRef<Mesh>(MeshPrimitives::CreateCube());
        m_Resources[id] = mesh;
        return mesh;
    }

    Ref<Mesh> MeshManager::GetSphere() {
        const std::string id = "__primitive_sphere";

        std::lock_guard lock(m_Mutex);
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = MakeRef<Mesh>(MeshPrimitives::CreateSphere());
        m_Resources[id] = mesh;
        return mesh;
    }

    Ref<Mesh> MeshManager::GetPlane() {
        const std::string id = "__primitive_plane";

        std::lock_guard lock(m_Mutex);
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = MakeRef<Mesh>(MeshPrimitives::CreatePlane());
        m_Resources[id] = mesh;
        return mesh;
    }

    Ref<Mesh> MeshManager::GetQuad() {
        const std::string id = "__primitive_quad";

        std::lock_guard lock(m_Mutex);
        auto it = m_Resources.find(id);
        if (it != m_Resources.end()) {
            return it->second;
        }

        auto mesh = MakeRef<Mesh>(MeshPrimitives::CreateQuad());
        m_Resources[id] = mesh;
        return mesh;
    }

    // ========== MaterialManager ==========

    MaterialManager& MaterialManager::Instance() {
        static MaterialManager instance;
        return instance;
    }

    Ref<CanvasItemMaterial> MaterialManager::CreateCanvasItem(
        const std::string& id,
        const Vec4& albedo
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<CanvasItemMaterial>(it->second);
            }
        }

        auto material = MaterialFactory::CreateCanvasItem(albedo);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created CanvasItem material: " << id;
        return material;
    }

    Ref<CanvasItemMaterial> MaterialManager::CreateCanvasItemTextured(
        const std::string& id,
        const std::string& textureName
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<CanvasItemMaterial>(it->second);
            }
        }

        auto texture = TextureManager::Instance().Get(textureName);
        auto material = MaterialFactory::CreateCanvasItemTextured(texture);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created textured CanvasItem material: " << id;
        return material;
    }

    Ref<SpatialMaterial> MaterialManager::CreateSpatial(
        const std::string& id,
        const Vec4& albedo,
        float metallic,
        float roughness,
        float specular
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<SpatialMaterial>(it->second);
            }
        }

        auto material = MaterialFactory::CreateSpatial(albedo, metallic, roughness, specular);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created Spatial material: " << id;
        return material;
    }

    Ref<SpatialMaterial> MaterialManager::CreateSpatialUnlit(
        const std::string& id,
        const Vec4& albedo
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<SpatialMaterial>(it->second);
            }
        }

        auto material = MaterialFactory::CreateSpatialUnlit(albedo);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created Spatial Unlit material: " << id;
        return material;
    }

    Ref<ToonMaterial> MaterialManager::CreateToon(
        const std::string& id,
        const Vec4& albedo,
        int toonLevels,
        float rimAmount
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<ToonMaterial>(it->second);
            }
        }

        auto material = MaterialFactory::CreateToon(albedo, toonLevels, rimAmount);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created Toon material: " << id;
        return material;
    }

    Ref<SkyMaterial> MaterialManager::CreateSky(
        const std::string& id,
        const Vec4& color
    ) {
        {
            std::lock_guard lock(m_Mutex);
            auto it = m_Materials.find(id);
            if (it != m_Materials.end()) {
                Logger::Warn() << "Material already exists: " << id;
                return std::dynamic_pointer_cast<SkyMaterial>(it->second);
            }
        }

        auto material = MaterialFactory::CreateSky(color);

        {
            std::lock_guard lock(m_Mutex);
            m_Materials[id] = material;
        }

        Logger::Trace() << "Created Sky material: " << id;
        return material;
    }

    Ref<Material> MaterialManager::Get(const std::string& id) const {
        std::lock_guard lock(m_Mutex);
        auto it = m_Materials.find(id);
        if (it != m_Materials.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool MaterialManager::Has(const std::string& id) const {
        std::lock_guard lock(m_Mutex);
        return m_Materials.contains(id);
    }

    size_t MaterialManager::Count() const {
        std::lock_guard lock(m_Mutex);
        return m_Materials.size();
    }

    void MaterialManager::Clear() {
        std::lock_guard lock(m_Mutex);
        m_Materials.clear();
        Logger::Debug("Cleared all materials");
    }

    // ========== AssetLibrary ==========

    void AssetLibrary::Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            Logger::Info("Initializing AssetLibrary...");
            
            // Preload built-in shaders
            BuiltInShaderManager::Instance().PreloadAll();
            
            Logger::Info("AssetLibrary initialized");
        });
    }

    void AssetLibrary::PreloadCommon() {
        Logger::Info("Preloading common assets...");

        // Preload primitive meshes
        Meshes().GetCube();
        Meshes().GetSphere();
        Meshes().GetPlane();
        Meshes().GetQuad();

        Logger::Info("Common assets preloaded");
    }

    void AssetLibrary::ClearAll() {
        Shaders().Clear();
        Textures().Clear();
        Meshes().Clear();
        Materials().Clear();
        
        Logger::Info("All assets cleared");
    }
}