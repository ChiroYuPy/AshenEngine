#include "Ashen/ECS/Entity.h"
#include "Ashen/ECS/Scene.h"

namespace ash {
    Entity::Entity(const UUID id, Scene* scene)
        : m_ID(id), m_Scene(scene) {
    }
}