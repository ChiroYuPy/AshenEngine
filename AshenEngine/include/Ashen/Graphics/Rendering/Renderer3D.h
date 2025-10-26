#ifndef ASHEN_RENDERER3D_H
#define ASHEN_RENDERER3D_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Camera;
    class Mesh;
    class Material;
    class ShaderProgram;
    class TextureCubeMap;

    /**
     * @brief Simple 3D object to render
     */
    struct RenderObject {
        Ref<Mesh> mesh;
        Ref<Material> material;
        Mat4 transform;
    };

    /**
     * @brief Directional light (sun)
     */
    struct DirectionalLight {
        Vec3 direction = Vec3(0.0f, -1.0f, 0.0f);
        Vec3 color = Vec3(1.0f);
        float intensity = 1.0f;
    };

    /**
     * @brief Point light
     */
    struct PointLight {
        Vec3 position = Vec3(0.0f);
        Vec3 color = Vec3(1.0f);
        float intensity = 1.0f;
    };

    /**
     * @brief Scene environment
     */
    struct SceneEnvironment {
        Ref<TextureCubeMap> skybox;
        Vec3 ambientColor = Vec3(0.03f);
    };

    /**
     * @brief Render statistics
     */
    struct RenderStats {
        uint32_t drawCalls = 0;
        uint32_t triangles = 0;
        uint32_t vertices = 0;

        void Reset() {
            drawCalls = 0;
            triangles = 0;
            vertices = 0;
        }
    };

    /**
     * @brief Simple 3D renderer - no frustum culling, no complex batching
     */
    class Renderer3D {
    public:
        static void Init();

        static void Shutdown();

        // Scene management
        static void BeginScene(const Camera &camera);

        static void EndScene();

        // Submit objects for rendering
        static void Submit(const RenderObject &object);

        static void Submit(
            const Ref<Mesh> &mesh,
            const Ref<Material> &material,
            const Mat4 &transform = Mat4(1.0f)
        );

        // Draw immediately without queuing
        static void DrawImmediate(
            const Ref<Mesh> &mesh,
            const Ref<Material> &material,
            const Mat4 &transform = Mat4(1.0f)
        );

        // Lighting
        static void SetDirectionalLight(const DirectionalLight &light);

        static void AddPointLight(const PointLight &light);

        static void ClearLights();

        // Environment
        static void SetEnvironment(const SceneEnvironment &env);

        static void SetSkybox(const Ref<TextureCubeMap> &skybox);

        static void SetAmbientLight(const Vec3 &color);

        // Utilities
        static void SetWireframeMode(bool enabled);

        static const RenderStats &GetStats();

        static void ResetStats();

    private:
        struct SceneData;
        static std::unique_ptr<SceneData> s_Data;

        static void SetupLighting(const Ref<ShaderProgram> &shader);

        static void FlushRenderQueue();
    };
}

#endif // ASHEN_RENDERER3D_H