#include "Voxelity/systems/PhysicsSystem.h"

#include "Voxelity/voxelWorld/world/World.h"
#include <algorithm>
#include <cmath>

namespace voxelity {
    PhysicsSystem::PhysicsSystem(const PhysicsConfig &config)
        : m_config(config) {
    }

    void PhysicsSystem::step(Entity &entity, const float deltaTime, const World &world) const {
        if (!entity.isActive) return;

        // 1. Intégration des forces (gravité)
        integrate(entity, deltaTime, world);

        // 2. Déplacement avec collision
        const glm::vec3 motion = entity.velocity * deltaTime;
        const glm::vec3 actualMotion = moveAndCollide(entity, motion, world);

        // 3. Mettre à jour la position
        entity.position += actualMotion;

        // 4. Appliquer la friction
        applyFriction(entity, deltaTime);
    }

    void PhysicsSystem::integrate(Entity &entity, const float deltaTime, const World &world) const {
        if (!entity.useGravity) return;

        float gravityFactor = 1.0f;
        float dragFactor = m_config.airDrag; // drag normal

        // Vérifier si l'entité est dans un liquide
        const glm::ivec3 blockPos = glm::floor(entity.position); // bloc "occupé" par le joueur
        const auto voxel = world.getVoxel(blockPos.x, blockPos.y, blockPos.z);
        if (voxel && isVoxelLiquid(voxel)) {
            // bloc liquide
            gravityFactor = 0.3f; // chute plus lente (flottabilité)
            dragFactor = m_config.airDrag * 5.0f; // drag plus fort pour simuler la résistance de l'eau
        }

        // Appliquer la gravité
        entity.velocity.y += m_config.gravity * gravityFactor * deltaTime;

        // Limiter à la vitesse terminale
        entity.velocity.y = std::max(entity.velocity.y, m_config.terminalVelocity);

        // Appliquer le drag si l'entité n'est pas au sol
        if (!entity.onGround) {
            entity.velocity *= (1.0f - dragFactor * deltaTime);
        }
    }

    glm::vec3 PhysicsSystem::moveAndCollide(Entity &entity, const glm::vec3 &motion,
                                            const World &world) const {
        if (!entity.hasCollisions) return motion;

        entity.onGround = false;

        pixl::AABB entityBox(entity.position, entity.boundingBoxSize);
        glm::vec3 remainingMotion = motion;
        glm::vec3 actualMotion(0.0f);

        // Ordre: Y (vertical) -> X -> Z pour éviter les blocages
        constexpr int axisOrder[3] = {1, 0, 2}; // Y, X, Z

        for (const int axis: axisOrder) {
            if (std::abs(remainingMotion[axis]) < m_config.collisionEpsilon) continue;

            CollisionResult collisions;
            const float moved = sweepAxis(entityBox, remainingMotion[axis], axis, world, collisions);

            actualMotion[axis] += moved;
            entityBox = entityBox.offset(glm::vec3(
                axis == 0 ? moved : 0.0f,
                axis == 1 ? moved : 0.0f,
                axis == 2 ? moved : 0.0f
            ));

            // Si collision détectée
            if (collisions.hasCollision) {
                remainingMotion[axis] = 0.0f;
                entity.velocity[axis] = 0.0f;

                // Détecter si on est au sol (collision par le bas)
                if (axis == 1 && motion.y < 0.0f) {
                    entity.onGround = true;
                }
            } else {
                remainingMotion[axis] = 0.0f;
            }
        }

        return actualMotion;
    }

    float PhysicsSystem::sweepAxis(const pixl::AABB &aabb, const float motion, const int axis,
                                   const World &world, CollisionResult &result) const {
        result.clear();

        if (std::abs(motion) < m_config.collisionEpsilon) return 0.0f;

        // Créer une AABB élargie pour le sweep
        const auto sweepVector = glm::vec3(
            axis == 0 ? motion : 0.0f,
            axis == 1 ? motion : 0.0f,
            axis == 2 ? motion : 0.0f
        );

        pixl::AABB sweepBox = aabb;
        if (motion > 0.0f) {
            sweepBox.max[axis] += motion;
        } else {
            sweepBox.min[axis] += motion;
        }

        // Expansion pour être sûr de détecter les collisions
        sweepBox = sweepBox.expand(m_config.collisionEpsilon);

        // Récupérer tous les blocs potentiels
        std::vector<glm::ivec3> blocks;
        getBroadPhaseBlocks(sweepBox, blocks, world);

        float closestHit = motion;
        bool hitFound = false;

        // Tester chaque bloc
        for (const auto &blockPos: blocks) {
            const pixl::AABB blockBox = pixl::AABB::fromBlock(blockPos);

            // Tester l'intersection sur l'axe du mouvement
            pixl::AABB testBox = aabb.offset(glm::vec3(
                axis == 0 ? closestHit : 0.0f,
                axis == 1 ? closestHit : 0.0f,
                axis == 2 ? closestHit : 0.0f
            ));

            if (testBox.intersects(blockBox)) {
                // Calculer la distance exacte de collision
                float hitDist;
                if (motion > 0.0f) {
                    hitDist = blockBox.min[axis] - aabb.max[axis];
                } else {
                    hitDist = blockBox.max[axis] - aabb.min[axis];
                }

                // Si c'est la collision la plus proche
                if (std::abs(hitDist) < std::abs(closestHit)) {
                    closestHit = hitDist;
                    hitFound = true;

                    CollisionInfo info;
                    info.blockPos = blockPos;
                    info.axis = axis;
                    info.distance = std::abs(hitDist);
                    info.penetration[axis] = hitDist;
                    result.addCollision(info);
                }
            }
        }

        // Si aucune collision, on peut se déplacer complètement
        if (!hitFound) {
            return motion;
        }

        // Sinon, se déplacer jusqu'au point de collision (moins epsilon)
        const float sign = motion > 0.0f ? 1.0f : -1.0f;
        return closestHit - sign * m_config.collisionEpsilon;
    }

    void PhysicsSystem::getBroadPhaseBlocks(const pixl::AABB &aabb, std::vector<glm::ivec3> &blocks,
                                            const World &world) {
        blocks.clear();

        const int minX = static_cast<int>(std::floor(aabb.min.x));
        const int maxX = static_cast<int>(std::ceil(aabb.max.x));
        const int minY = static_cast<int>(std::floor(aabb.min.y));
        const int maxY = static_cast<int>(std::ceil(aabb.max.y));
        const int minZ = static_cast<int>(std::floor(aabb.min.z));
        const int maxZ = static_cast<int>(std::ceil(aabb.max.z));

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                for (int z = minZ; z <= maxZ; ++z) {
                    if (isVoxelSolid(world.getVoxel(x, y, z))) {
                        blocks.emplace_back(x, y, z);
                    }
                }
            }
        }
    }

    void PhysicsSystem::applyFriction(Entity &entity, const float deltaTime) const {
        if (entity.onGround) {
            // Friction au sol (décélération rapide)
            const float frictionFactor = std::pow(m_config.groundFriction, deltaTime * 10.0f);
            entity.velocity.x *= frictionFactor;
            entity.velocity.z *= frictionFactor;

            // Arrêter si la vitesse est très faible
            if (std::abs(entity.velocity.x) < 0.001f) entity.velocity.x = 0.0f;
            if (std::abs(entity.velocity.z) < 0.001f) entity.velocity.z = 0.0f;
        }
    }
}
