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

        // État physique (mis à jour par fixed timestep)
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 velocity{0.0f};

        bool useGravity = true;
        bool hasCollisions = true;
        glm::vec3 boundingBoxSize{1.0f};
        bool isActive = true;
        bool onGround = false;

        // États pour interpolation
        glm::vec3 previousPosition{0.0f};
        glm::vec3 previousRotation{0.0f};

        // Sauvegarder l'état actuel avant mise à jour physique
        void saveState() {
            previousPosition = position;
            previousRotation = rotation;
        }

        // Obtenir la position interpolée pour le rendu
        glm::vec3 getInterpolatedPosition(const float alpha) const {
            return glm::mix(previousPosition, position, alpha);
        }

        // Obtenir la rotation interpolée pour le rendu
        glm::vec3 getInterpolatedRotation(const float alpha) const {
            return glm::mix(previousRotation, rotation, alpha);
        }

        [[nodiscard]] ash::BoundingBox3D getBoundingBox() const {
            return {position - boundingBoxSize * 0.5f, position + boundingBoxSize * 0.5f};
        }

    protected:
        Entity() = default;
    };
}

#endif //VOXELITY_ENTITY_H
