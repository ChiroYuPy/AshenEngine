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

        // 1. Gravité et air drag
        integrate(entity, deltaTime, world);

        // 2. Mouvement et collisions
        const glm::vec3 motion = entity.velocity * deltaTime;
        const glm::vec3 actualMotion = moveAndCollide(entity, motion, world);
        entity.position += actualMotion;

        // 3. Friction au sol (appliquée APRÈS le mouvement dans Minecraft)
        applyFriction(entity, deltaTime, world);
    }

    void PhysicsSystem::integrate(Entity &entity, const float deltaTime, const World &world) const {
        if (!entity.useGravity) return;

        // Dans Minecraft, la gravité s'applique AVANT le drag
        entity.velocity.y += m_config.gravity * deltaTime;

        // Air drag vertical (0.98 dans Minecraft)
        entity.velocity.y *= m_config.airDrag;

        // Limiter à la terminal velocity
        if (entity.velocity.y < m_config.terminalVelocity) {
            entity.velocity.y = m_config.terminalVelocity;
        }
    }

    glm::vec3 PhysicsSystem::moveAndCollide(Entity &entity, const glm::vec3 &motion,
                                            const World &world) const {
        if (!entity.hasCollisions) return motion;

        entity.onGround = false;

        ash::BoundingBox3D entityBox = entity.getBoundingBox();
        glm::vec3 remainingMotion = motion;
        glm::vec3 actualMotion(0.0f);

        constexpr int axisOrder[3] = {1, 0, 2};

        for (const int axis: axisOrder) {
            if (std::abs(remainingMotion[axis]) < m_config.collisionEpsilon) continue;

            CollisionResult collisions;
            const float moved = sweepAxis(entityBox, remainingMotion[axis], axis, world, collisions);

            actualMotion[axis] += moved;

            entityBox = entityBox.Offset(glm::vec3(
                axis == 0 ? moved : 0.0f,
                axis == 1 ? moved : 0.0f,
                axis == 2 ? moved : 0.0f
            ));

            if (collisions.hasCollision) {
                remainingMotion[axis] = 0.0f;

                // Appliquer le rebond si le matériau le permet
                if (m_config.useMaterialProperties && !collisions.collisions.empty()) {
                    const float bounciness = getVoxelBounciness(collisions.collisions[0].blockType);
                    if (bounciness > 0.01f) {
                        entity.velocity[axis] = -entity.velocity[axis] * bounciness;
                    } else {
                        entity.velocity[axis] = 0.0f;
                    }
                } else {
                    entity.velocity[axis] = 0.0f;
                }

                if (axis == 1 && motion.y < 0.0f) {
                    entity.onGround = true;
                }
            } else {
                remainingMotion[axis] = 0.0f;
            }
        }

        return actualMotion;
    }

    float PhysicsSystem::sweepAxis(const ash::BoundingBox3D &aabb, const float motion, const int axis,
                                   const World &world, CollisionResult &result) const {
        result.clear();

        if (std::abs(motion) < m_config.collisionEpsilon) return 0.0f;

        ash::BoundingBox3D sweepBox = aabb;
        if (motion > 0.0f) {
            sweepBox.max[axis] += motion;
        } else {
            sweepBox.min[axis] += motion;
        }

        sweepBox = sweepBox.Expand(m_config.collisionEpsilon);

        std::vector<glm::ivec3> blocks;
        getBroadPhaseBlocks(sweepBox, blocks, world);

        float closestHit = motion;
        bool hitFound = false;

        for (const auto &blockPos: blocks) {
            const ash::BoundingBox3D blockBox = ash::BoundingBox3D::fromBlock(blockPos);

            float hitDist;
            if (motion > 0.0f) {
                hitDist = blockBox.min[axis] - aabb.max[axis];
            } else {
                hitDist = blockBox.max[axis] - aabb.min[axis];
            }

            if (std::abs(hitDist) < std::abs(closestHit)) {
                ash::BoundingBox3D testBox = aabb.Offset(glm::vec3(
                    axis == 0 ? hitDist : 0.0f,
                    axis == 1 ? hitDist : 0.0f,
                    axis == 2 ? hitDist : 0.0f
                ));

                if (testBox.Intersects(blockBox)) {
                    closestHit = hitDist;
                    hitFound = true;

                    CollisionInfo info;
                    info.blockPos = blockPos;
                    info.axis = axis;
                    info.distance = std::abs(hitDist);
                    info.penetration[axis] = hitDist;
                    info.blockType = world.getVoxel(blockPos);
                    result.addCollision(info);
                }
            }
        }

        if (!hitFound) {
            return motion;
        }

        const float sign = motion > 0.0f ? 1.0f : -1.0f;
        return closestHit - sign * m_config.collisionEpsilon;
    }

    void PhysicsSystem::getBroadPhaseBlocks(const ash::BoundingBox3D &aabb, std::vector<glm::ivec3> &blocks,
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
                    const VoxelType voxel = world.getVoxel(x, y, z);
                    if (doesVoxelHaveCollision(voxel)) {
                        blocks.emplace_back(x, y, z);
                    }
                }
            }
        }
    }

    void PhysicsSystem::applyFriction(Entity &entity, const float deltaTime, const World &world) const {
        if (entity.onGround) {
            // Friction au sol (Minecraft style)
            float friction = m_config.groundFriction;

            if (m_config.useMaterialProperties) {
                friction = getGroundFriction(entity, world);
            }

            entity.velocity.x *= friction;
            entity.velocity.z *= friction;

            // Arrêter les très petites vitesses
            if (std::abs(entity.velocity.x) < 0.003f) entity.velocity.x = 0.0f;
            if (std::abs(entity.velocity.z) < 0.003f) entity.velocity.z = 0.0f;
        } else {
            // Friction en l'air (horizontal)
            entity.velocity.x *= m_config.horizontalAirDrag;
            entity.velocity.z *= m_config.horizontalAirDrag;
        }
    }

    float PhysicsSystem::getGroundFriction(const Entity &entity, const World &world) const {
        // Vérifier le bloc juste sous les pieds
        const glm::vec3 feetPos = entity.position - glm::vec3(0, entity.boundingBoxSize.y * 0.5f + 0.1f, 0);
        const glm::ivec3 blockPos = glm::floor(feetPos);
        const VoxelType voxel = world.getVoxel(blockPos);

        if (doesVoxelHaveCollision(voxel)) {
            return getVoxelFriction(voxel);
        }

        return m_config.groundFriction;
    }
}