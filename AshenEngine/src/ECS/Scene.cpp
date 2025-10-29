#include "Ashen/ECS/Scene.h"

#include <ranges>

#include "Ashen/ECS/Component.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Entity Scene::CreateEntity(const String& name) {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(const UUID uuid, const String& name) {
        Entity entity(uuid, this);
        m_Entities[uuid] = entity;

        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(name);

        return entity;
    }

    void Scene::DestroyEntity(const Entity entity) {
        if (!IsValid(entity)) return;

        const UUID entityId = entity.GetUUID();

        RemoveAllComponents(entityId);

        m_Entities.erase(entityId);
    }

    Entity Scene::GetEntity(const UUID uuid) {
        const auto it = m_Entities.find(uuid);
        if (it != m_Entities.end())
            return it->second;

        return Entity();
    }

    void Scene::OnUpdate(float ts) {

    }

    void Scene::OnRender() {

    }

    void Scene::OnEvent(Event& event) {

    }

    bool Scene::IsValid(const Entity entity) const {
        return m_Entities.contains(entity.GetUUID());
    }

    void Scene::Clear() {

        m_ComponentArrays.clear();

        m_Entities.clear();
    }

    void Scene::RemoveAllComponents(const UUID entityId) {
        for (const auto &componentArray: m_ComponentArrays | std::views::values)
            componentArray->RemoveEntity(entityId);
    }

} // namespace ash