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

    struct RenderObject {
        Ref<Mesh> mesh;
        Ref<Material> material;
        Mat4 transform;
    };

    struct DirectionalLight {
        Vec3 direction = Vec3(0.0f, -1.0f, 0.0f);
        Vec3 color = Vec3(1.0f);
        float intensity = 1.0f;
    };

    struct PointLight {
        Vec3 position = Vec3(0.0f);
        Vec3 color = Vec3(1.0f);
        float intensity = 1.0f;
    };

    struct SceneEnvironment {
        Ref<TextureCubeMap> skybox;
        Vec3 ambientColor = Vec3(0.03f);
    };

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

    class Renderer3D {
    public:
        static void Init();

        static void Shutdown();

        static void BeginScene(const Camera &camera);

        static void EndScene();

        static void Submit(const RenderObject &object);

        static void Submit(const Ref<Mesh> &mesh, const Ref<Material> &material, const Mat4 &transform = Mat4(1.0f));

        static void DrawImmediate(const Ref<Mesh> &mesh, const Ref<Material> &material,
                                  const Mat4 &transform = Mat4(1.0f));

        static void SetDirectionalLight(const DirectionalLight &light);

        static void AddPointLight(const PointLight &light);

        static void ClearLights();

        static void SetEnvironment(const SceneEnvironment &env);

        static void SetSkybox(const Ref<TextureCubeMap> &skybox);

        static void SetAmbientLight(const Vec3 &color);

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
