#ifndef ASHEN_ENTITY_H
#define ASHEN_ENTITY_H

#include "Ashen/Core/UUID.h"
#include "Ashen/Core/Types.h"
#include <typeindex>

namespace ash {
    class Scene;
    class Component;

    class Entity {
    public:
        Entity() = default;
        Entity(UUID id, Scene* scene);

        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        T* GetComponent();

        template<typename T>
        const T* GetComponent() const;

        template<typename T>
        bool HasComponent() const;

        template<typename T>
        void RemoveComponent() const;

        bool IsValid() const { return m_Scene != nullptr; }
        UUID GetUUID() const { return m_ID; }

        explicit operator bool() const { return IsValid(); }
        explicit operator UUID() const { return m_ID; }

        bool operator==(const Entity& other) const {
            return m_ID == other.m_ID && m_Scene == other.m_Scene;
        }

        bool operator!=(const Entity& other) const {
            return !(*this == other);
        }

    private:
        UUID m_ID;
        Scene* m_Scene = nullptr;

        friend class Scene;
    };
}

template<>
struct std::hash<ash::Entity> {
    std::size_t operator()(const ash::Entity& entity) const noexcept {
        return std::hash<ash::UUID>()(entity.GetUUID());
    }
};

#endif // ASHEN_ENTITY_H