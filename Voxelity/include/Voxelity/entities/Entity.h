#ifndef VOXELITY_ENTITY_H
#define VOXELITY_ENTITY_H

#include <glm/glm.hpp>

#include "Ashen/math/BoundingBox.h"

namespace voxelity {
    class World;

    class Entity {
    public:
        virtual ~Entity() = default;

        virtual void update(float deltaTime, const World &world) = 0;

        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};

        glm::vec3 velocity{0.0f};
        bool useGravity = true;
        bool hasCollisions = true;

        glm::vec3 boundingBoxSize{1.0f};

        bool isActive = true;
        bool onGround = false;

        [[nodiscard]] ash::BoundingBox3D getBoundingBox() const {
            return {position - boundingBoxSize * 0.5f, position + boundingBoxSize * 0.5f};
        }

    protected:
        Entity() = default;
    };
}

#endif //VOXELITY_ENTITY_H