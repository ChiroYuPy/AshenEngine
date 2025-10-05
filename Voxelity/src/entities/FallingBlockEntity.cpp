#include "Voxelity/entities/FallingBlockEntity.h"

#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {
    FallingBlockEntity::FallingBlockEntity(const VoxelType blockType)
        : m_blockType(blockType) {
        // Configuration
        boundingBoxSize = glm::vec3(0.98f); // Légèrement plus petit qu'un bloc
        useGravity = true;
        hasCollisions = true;

        PhysicsConfig config;
        config.gravity = -32.0f;
        config.groundFriction = 0.8f; // Plus de friction pour s'arrêter vite
        m_physics = std::make_unique<PhysicsSystem>(config);
    }

    void FallingBlockEntity::update(const float deltaTime, const World &world) {
        if (!isActive) return;

        // Vieillissement
        m_age += deltaTime;
        if (m_age >= m_lifetime) {
            isActive = false;
            return;
        }

        // Appliquer la physique
        m_physics->step(*this, deltaTime, world);

        // Vérifier l'atterrissage
        checkLanding(world);
    }

    void FallingBlockEntity::checkLanding(const World &world) {
        // Si le bloc est au sol et ne bouge presque plus
        if (onGround &&glm::length(velocity)
        <
        0.1f
        ) {
            // Convertir en bloc solide dans le monde
            const auto blockPos = glm::ivec3(glm::floor(position));

            if (world.getVoxel(blockPos) == VoxelID::AIR)
                const_cast<World &>(world).setVoxel(blockPos, m_blockType);

            // Désactiver l'entité
            isActive = false;
        }
    }
}