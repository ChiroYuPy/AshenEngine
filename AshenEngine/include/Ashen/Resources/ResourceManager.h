#ifndef ASHEN_RESOURCEMANAGER_H
#define ASHEN_RESOURCEMANAGER_H

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

// Forward declarations from other modules
namespace ash {
    namespace fs = std::filesystem;
    class ShaderProgram;
    class Texture2D;
    class Mesh;
    class Material;
    class CanvasItemMaterial;
    class SpatialMaterial;
    class ToonMaterial;
    class SkyMaterial;
    struct TextureConfig;

    /**
     * @brief Thread-safe resource paths manager
     */
    class ResourcePaths final {
    public:
        static ResourcePaths &Instance();

        // No copy/move
        ResourcePaths(const ResourcePaths &) = delete;

        ResourcePaths &operator=(const ResourcePaths &) = delete;

        ResourcePaths(ResourcePaths &&) = delete;

        ResourcePaths &operator=(ResourcePaths &&) = delete;

        void SetWorkingDirectory(const fs::path &dir);

        [[nodiscard]] fs::path GetPath(const std::string &filename) const;

        [[nodiscard]] const fs::path &Root() const;

    private:
        ResourcePaths() = default;

        fs::path m_Root;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Thread-safe base resource manager
     * Resources are cached and shared via shared_ptr
     */
    template<typename T>
    class ResourceManager {
    public:
        virtual ~ResourceManager() = default;

        // No copy/move
        ResourceManager(const ResourceManager &) = delete;

        ResourceManager &operator=(const ResourceManager &) = delete;

        ResourceManager(ResourceManager &&) = delete;

        ResourceManager &operator=(ResourceManager &&) = delete;

        /**
         * @brief Get or load a resource by ID
         * Thread-safe, returns cached resource if available
         */
        [[nodiscard]] Ref<T> Get(const std::string &id) { {
                std::lock_guard lock(m_Mutex);
                auto it = m_Resources.find(id);
                if (it != m_Resources.end()) {
                    return it->second;
                }
            }
            return Load(id);
        }

        /**
         * @brief Check if resource exists in cache
         */
        [[nodiscard]] bool Has(const std::string &id) const {
            std::lock_guard lock(m_Mutex);
            return m_Resources.contains(id);
        }

        /**
         * @brief Get number of cached resources
         */
        [[nodiscard]] size_t Count() const {
            std::lock_guard lock(m_Mutex);
            return m_Resources.size();
        }

        /**
         * @brief Clear all cached resources
         */
        virtual void Clear() {
            std::lock_guard lock(m_Mutex);
            m_Resources.clear();
        }

    protected:
        ResourceManager() = default;

        /**
         * @brief Load resource implementation (called when not in cache)
         * Must be implemented by derived classes
         */
        virtual Ref<T> Load(const std::string &id) = 0;

        /**
         * @brief Thread-safe cache insertion
         */
        void Cache(const std::string &id, Ref<T> resource) {
            std::lock_guard lock(m_Mutex);
            m_Resources[id] = std::move(resource);
        }

        std::unordered_map<std::string, Ref<T> > m_Resources;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Shader resource manager
     * Handles both built-in and custom shaders
     */
    class ShaderManager final : public ResourceManager<ShaderProgram> {
    public:
        static ShaderManager &Instance();

        // Built-in shaders (enum from BuiltInShader.h)
        enum class BuiltIn {
            CanvasItem,
            CanvasItemTextured,
            Spatial,
            SpatialUnlit,
            Toon,
            Sky
        };

        /**
         * @brief Get a built-in shader
         */
        [[nodiscard]] Ref<ShaderProgram> GetBuiltIn(BuiltIn type);

        /**
         * @brief Load custom shader from vertex/fragment files
         * Files should be named: id.vert and id.frag
         */
        Ref<ShaderProgram> Load(const std::string &id) override;

        void Clear() override;

    private:
        ShaderManager() = default;
    };

    /**
     * @brief Texture resource manager
     */
    class TextureManager final : public ResourceManager<Texture2D> {
    public:
        static TextureManager &Instance();

        /**
         * @brief Load texture with automatic format detection
         * Searches for file with supported extensions
         */
        Ref<Texture2D> Load(const std::string &id) override;

    private:
        TextureManager() = default;
    };

    /**
     * @brief Mesh resource manager
     */
    class MeshManager final : public ResourceManager<Mesh> {
    public:
        static MeshManager &Instance();

        /**
         * @brief Load mesh from file
         * Searches for file with supported extensions (.obj)
         */
        Ref<Mesh> Load(const std::string &id) override;

        /**
         * @brief Get primitive meshes (cached)
         */
        [[nodiscard]] Ref<Mesh> GetCube();

        [[nodiscard]] Ref<Mesh> GetSphere();

        [[nodiscard]] Ref<Mesh> GetPlane();

        [[nodiscard]] Ref<Mesh> GetQuad();

    private:
        MeshManager() = default;
    };

    /**
     * @brief Material factory and manager
     * Materials are created via factory methods, not loaded from files
     */
    class MaterialManager final {
    public:
        static MaterialManager &Instance();

        // No copy/move
        MaterialManager(const MaterialManager &) = delete;

        MaterialManager &operator=(const MaterialManager &) = delete;

        MaterialManager(MaterialManager &&) = delete;

        MaterialManager &operator=(MaterialManager &&) = delete;

        // ====== Factory Methods ======

        /**
         * @brief Create 2D colored material
         */
        [[nodiscard]] Ref<CanvasItemMaterial> CreateCanvasItem(
            const std::string &id,
            const Vec4 &albedo = Vec4(1.0f)
        );

        /**
         * @brief Create 2D textured material
         */
        [[nodiscard]] Ref<CanvasItemMaterial> CreateCanvasItemTextured(
            const std::string &id,
            const std::string &textureName
        );

        /**
         * @brief Create 3D PBR material
         */
        [[nodiscard]] Ref<SpatialMaterial> CreateSpatial(
            const std::string &id,
            const Vec4 &albedo = Vec4(1.0f),
            float metallic = 0.0f,
            float roughness = 0.5f,
            float specular = 0.5f
        );

        /**
         * @brief Create 3D unlit material
         */
        [[nodiscard]] Ref<SpatialMaterial> CreateSpatialUnlit(
            const std::string &id,
            const Vec4 &albedo = Vec4(1.0f)
        );

        /**
         * @brief Create toon/cel-shaded material
         */
        [[nodiscard]] Ref<ToonMaterial> CreateToon(
            const std::string &id,
            const Vec4 &albedo = Vec4(1.0f),
            int toonLevels = 3,
            float rimAmount = 0.716f
        );

        /**
         * @brief Create sky material
         */
        [[nodiscard]] Ref<SkyMaterial> CreateSky(
            const std::string &id,
            const Vec4 &color = Vec4(0.5f, 0.7f, 1.0f, 1.0f)
        );

        /**
         * @brief Get cached material by ID
         */
        [[nodiscard]] Ref<Material> Get(const std::string &id) const;

        /**
         * @brief Check if material exists
         */
        [[nodiscard]] bool Has(const std::string &id) const;

        /**
         * @brief Get number of cached materials
         */
        [[nodiscard]] size_t Count() const;

        /**
         * @brief Clear all cached materials
         */
        void Clear();

    private:
        MaterialManager() = default;

        std::unordered_map<std::string, Ref<Material> > m_Materials;
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief Unified asset library interface
     * Static facade for all resource managers
     */
    class AssetLibrary final {
    public:
        // No instantiation
        AssetLibrary() = delete;

        /**
         * @brief Initialize asset library (called once at startup)
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

        // Manager accessors
        static ShaderManager &Shaders() { return ShaderManager::Instance(); }
        static TextureManager &Textures() { return TextureManager::Instance(); }
        static MeshManager &Meshes() { return MeshManager::Instance(); }
        static MaterialManager &Materials() { return MaterialManager::Instance(); }
    };
}

#endif // ASHEN_RESOURCEMANAGER_H