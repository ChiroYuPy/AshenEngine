#ifndef ASHEN_SCENE_H
#define ASHEN_SCENE_H

#include "Ashen/Core/Types.h"
#include "Ashen/Core/UUID.h"
#include "Ashen/ECS/Entity.h"
#include "Ashen/Events/Event.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

namespace ash {
    class ComponentArrayBase {
    public:
        virtual ~ComponentArrayBase() = default;
        virtual void RemoveEntity(UUID entityId) = 0;
        virtual size_t Size() const = 0;
    };

    template<typename T>
    class ComponentArray : public ComponentArrayBase {
    public:
        void Insert(const UUID entityId, T component) {
            if (m_EntityToIndex.contains(entityId)) {
                size_t index = m_EntityToIndex[entityId];
                m_Components[index] = MovePtr(component);
            } else {
                const size_t newIndex = m_Components.size();
                m_EntityToIndex[entityId] = newIndex;
                m_IndexToEntity[newIndex] = entityId;
                m_Components.push_back(MovePtr(component));
            }
        }

        void Remove(const UUID entityId) {
            if (!m_EntityToIndex.contains(entityId)) return;

            size_t indexOfRemoved = m_EntityToIndex[entityId];
            size_t indexOfLast = m_Components.size() - 1;

            if (indexOfRemoved != indexOfLast) {
                m_Components[indexOfRemoved] = MovePtr(m_Components[indexOfLast]);

                const UUID entityOfLast = m_IndexToEntity[indexOfLast];
                m_EntityToIndex[entityOfLast] = indexOfRemoved;
                m_IndexToEntity[indexOfRemoved] = entityOfLast;
            }

            m_EntityToIndex.erase(entityId);
            m_IndexToEntity.erase(indexOfLast);
            m_Components.pop_back();
        }

        T* Get(const UUID entityId) {
            if (!m_EntityToIndex.contains(entityId)) return nullptr;
            return &m_Components[m_EntityToIndex[entityId]];
        }

        const T* Get(const UUID entityId) const {
            const auto it = m_EntityToIndex.find(entityId);
            if (it == m_EntityToIndex.end()) return nullptr;
            return &m_Components[it->second];
        }

        bool Has(const UUID entityId) const {
            return m_EntityToIndex.contains(entityId);
        }

        void RemoveEntity(const UUID entityId) override {
            Remove(entityId);
        }

        size_t Size() const override {
            return m_Components.size();
        }

        auto begin() { return m_Components.begin(); }
        auto end() { return m_Components.end(); }
        auto begin() const { return m_Components.begin(); }
        auto end() const { return m_Components.end(); }

        std::vector<std::pair<UUID, T*>> GetAll() {
            std::vector<std::pair<UUID, T*>> result;
            result.reserve(m_Components.size());
            for (auto& [entityId, index] : m_EntityToIndex)
                result.emplace_back(entityId, &m_Components[index]);

            return result;
        }

    private:
        std::vector<T> m_Components;
        std::unordered_map<UUID, size_t> m_EntityToIndex;
        std::unordered_map<size_t, UUID> m_IndexToEntity;
    };

    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Entity CreateEntity(const String& name = "Entity");
        Entity CreateEntityWithUUID(UUID uuid, const String& name = "Entity");
        void DestroyEntity(Entity entity);
        Entity GetEntity(UUID uuid);

        void OnUpdate(float ts);
        void OnRender();
        void OnEvent(Event& event);

        template<typename... Components>
        std::vector<Entity> GetEntitiesWith();

        template<typename T>
        ComponentArray<T>* GetComponentArray() {
            const std::type_index typeIndex(typeid(T));

            if (!m_ComponentArrays.contains(typeIndex))
                m_ComponentArrays[typeIndex] = std::make_unique<ComponentArray<T>>();

            return static_cast<ComponentArray<T>*>(m_ComponentArrays[typeIndex].get());
        }

        size_t GetEntityCount() const { return m_Entities.size(); }
        bool IsValid(Entity entity) const;

        void Clear();

    private:
        std::unordered_map<UUID, Entity> m_Entities;
        std::unordered_map<std::type_index, std::unique_ptr<ComponentArrayBase>> m_ComponentArrays;

        friend class Entity;

        template<typename T>
        void AddComponent(UUID entityId, T component) {
            GetComponentArray<T>()->Insert(entityId, MovePtr(component));
        }

        template<typename T>
        void RemoveComponent(UUID entityId) {
            GetComponentArray<T>()->Remove(entityId);
        }

        template<typename T>
        T* GetComponent(UUID entityId) {
            return GetComponentArray<T>()->Get(entityId);
        }

        template<typename T>
        const T* GetComponent(UUID entityId) const {
            const std::type_index typeIndex(typeid(T));
            const auto it = m_ComponentArrays.find(typeIndex);
            if (it == m_ComponentArrays.end()) return nullptr;

            return static_cast<const ComponentArray<T>*>(it->second.get())->Get(entityId);
        }

        template<typename T>
        bool HasComponent(UUID entityId) const {
            const std::type_index typeIndex(typeid(T));
            const auto it = m_ComponentArrays.find(typeIndex);
            if (it == m_ComponentArrays.end()) return false;

            return static_cast<const ComponentArray<T>*>(it->second.get())->Has(entityId);
        }

        void RemoveAllComponents(UUID entityId);
    };

    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args) {
        T component(std::forward<Args>(args)...);
        m_Scene->AddComponent<T>(m_ID, MovePtr(component));
        return *m_Scene->GetComponent<T>(m_ID);
    }

    template<typename T>
    T* Entity::GetComponent() {
        return m_Scene->GetComponent<T>(m_ID);
    }

    template<typename T>
    const T* Entity::GetComponent() const {
        return m_Scene->GetComponent<T>(m_ID);
    }

    template<typename T>
    bool Entity::HasComponent() const {
        return m_Scene->HasComponent<T>(m_ID);
    }

    template<typename T>
    void Entity::RemoveComponent() const {
        m_Scene->RemoveComponent<T>(m_ID);
    }

    // Implémentation de GetEntitiesWith
    template<typename... Components>
    std::vector<Entity> Scene::GetEntitiesWith() {
        std::vector<Entity> result;

        for (auto& [uuid, entity] : m_Entities)
            if ((HasComponent<Components>(uuid) && ...))
                result.push_back(entity);

        return result;
    }

} // namespace ash

#endif // ASHEN_SCENE_H