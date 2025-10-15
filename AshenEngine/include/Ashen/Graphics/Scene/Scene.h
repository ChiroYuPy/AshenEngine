#ifndef ASHEN_SCENE_H
#define ASHEN_SCENE_H

#include <string>
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Mesh;
    class Material;
    class Camera;

    /**
     * @brief Simple 3D transform
     */
    struct Transform {
        Vec3 position = Vec3(0.0f);
        Vec3 rotation = Vec3(0.0f); // Euler angles (radians)
        Vec3 scale = Vec3(1.0f);

        Mat4 GetMatrix() const {
            Mat4 mat = glm::translate(Mat4(1.0f), position);
            mat = glm::rotate(mat, rotation.y, Vec3(0, 1, 0));
            mat = glm::rotate(mat, rotation.x, Vec3(1, 0, 0));
            mat = glm::rotate(mat, rotation.z, Vec3(0, 0, 1));
            mat = glm::scale(mat, scale);
            return mat;
        }
    };

    /**
     * @brief Renderable entity with mesh and material
     */
    class MeshEntity {
    public:
        MeshEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
            : m_Mesh(mesh), m_Material(material) {}

        Transform& GetTransform() { return m_Transform; }
        const Transform& GetTransform() const { return m_Transform; }

        void SetMesh(std::shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
        std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }

        void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }
        std::shared_ptr<Material> GetMaterial() const { return m_Material; }

        void SetVisible(const bool visible) { m_Visible = visible; }
        bool IsVisible() const { return m_Visible; }

    private:
        Transform m_Transform;
        std::shared_ptr<Mesh> m_Mesh;
        std::shared_ptr<Material> m_Material;
        bool m_Visible = true;
    };

    /**
     * @brief Simple 3D scene - just a collection of entities
     */
    class Scene {
    public:
        Scene(const std::string& name = "Scene") : m_Name(name) {}

        // Entity management
        std::shared_ptr<MeshEntity> CreateEntity(
            std::shared_ptr<Mesh> mesh,
            std::shared_ptr<Material> material
        ) {
            auto entity = std::make_shared<MeshEntity>(mesh, material);
            m_Entities.push_back(entity);
            return entity;
        }

        void RemoveEntity(const std::shared_ptr<MeshEntity>& entity) {
            m_Entities.erase(
                std::remove(m_Entities.begin(), m_Entities.end(), entity),
                m_Entities.end()
            );
        }

        void Clear() {
            m_Entities.clear();
        }

        // Rendering
        void Render(Camera& camera) {
            Renderer3D::BeginScene(camera);
            Renderer3D::SetEnvironment(m_Environment);
            Renderer3D::ClearLights();

            // Setup lights
            if (m_DirectionalLight) {
                Renderer3D::SetDirectionalLight(*m_DirectionalLight);
            }
            for (const auto& light : m_PointLights) {
                Renderer3D::AddPointLight(light);
            }

            // Render entities
            for (const auto& entity : m_Entities) {
                if (entity && entity->IsVisible()) {
                    Renderer3D::Submit(
                        entity->GetMesh(),
                        entity->GetMaterial(),
                        entity->GetTransform().GetMatrix()
                    );
                }
            }

            Renderer3D::EndScene();
        }

        // Lighting
        void SetDirectionalLight(const DirectionalLight& light) {
            if (!m_DirectionalLight) {
                m_DirectionalLight = std::make_unique<DirectionalLight>();
            }
            *m_DirectionalLight = light;
        }

        void AddPointLight(const PointLight& light) {
            m_PointLights.push_back(light);
        }

        void ClearLights() {
            m_DirectionalLight.reset();
            m_PointLights.clear();
        }

        // Environment
        void SetEnvironment(const SceneEnvironment& env) {
            m_Environment = env;
        }

        SceneEnvironment& GetEnvironment() { return m_Environment; }

        // Properties
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        const Vector<std::shared_ptr<MeshEntity>>& GetEntities() const {
            return m_Entities;
        }

    private:
        std::string m_Name;
        Vector<std::shared_ptr<MeshEntity>> m_Entities;
        std::unique_ptr<DirectionalLight> m_DirectionalLight;
        Vector<PointLight> m_PointLights;
        SceneEnvironment m_Environment;
    };
}

#endif // ASHEN_SCENE_H