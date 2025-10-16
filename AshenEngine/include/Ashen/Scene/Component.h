#ifndef ASHEN_COMPONENT_H
#define ASHEN_COMPONENT_H

#include <string>
#include <typeinfo>

#include "Property.h"

namespace ash {
    class PropertyRegistry;
    class Node;

    class Component {
    public:
        explicit Component(Node* owner) : m_Owner(owner), m_Active(true) {}
        virtual ~Component() = default;

        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

        [[nodiscard]] virtual std::string GetComponentName() const = 0;
        [[nodiscard]] virtual const std::type_info& GetComponentType() const = 0;

        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float ts) {}
        virtual void OnRender() {}

        [[nodiscard]] Node* GetOwner() const { return m_Owner; }
        [[nodiscard]] bool IsActive() const { return m_Active; }
        void SetActive(const bool active) { m_Active = active; }

        [[nodiscard]] PropertyRegistry& GetProperties() { return m_Properties; }

    protected:
        Node* m_Owner;
        bool m_Active;
        PropertyRegistry m_Properties{};
    };

    template<typename T>
    class TypedComponent final : public Component {
    public:
        explicit TypedComponent(Node* owner) : Component(owner) {}

        [[nodiscard]] std::string GetComponentName() const override {
            return typeid(T).name();
        }

        [[nodiscard]] const std::type_info& GetComponentType() const override {
            return typeid(T);
        }
    };
}

#endif //ASHEN_COMPONENT_H