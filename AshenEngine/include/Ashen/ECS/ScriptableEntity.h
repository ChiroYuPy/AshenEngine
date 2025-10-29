#ifndef ASHEN_SCRIPTABLE_ENTITY_H
#define ASHEN_SCRIPTABLE_ENTITY_H

#include "Ashen/ECS/Entity.h"

namespace ash {
    // Classe de base pour les scripts natifs attachés aux entités
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() = default;

        template<typename T>
        T* GetComponent() {
            return m_Entity.GetComponent<T>();
        }

        template<typename T>
        bool HasComponent() const {
            return m_Entity.HasComponent<T>();
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float ts) {}

    private:
        Entity m_Entity;
        friend class Scene;
    };

} // namespace ash

#endif // ASHEN_SCRIPTABLE_ENTITY_H