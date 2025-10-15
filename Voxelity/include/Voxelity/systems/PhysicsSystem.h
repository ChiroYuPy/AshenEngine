#ifndef VOXELITY_PHYSICSSYSTEM_H
#define VOXELITY_PHYSICSSYSTEM_H

#include <glm/glm.hpp>

#include "Ashen/Core/Types.h"
#include "Voxelity/entities/Entity.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    class World;

    struct CollisionInfo {
        glm::ivec3 blockPos;
        glm::vec3 penetration;
        int axis;
        float distance;
        VoxelType blockType; // Type de bloc touché

        CollisionInfo()
            : blockPos(0), penetration(0.0f), axis(0), distance(0.0f), blockType(0) {
        }
    };

    struct CollisionResult {
        bool hasCollision = false;
        ash::Vector<CollisionInfo> collisions;
        glm::vec3 totalPenetration{0.0f};

        void addCollision(const CollisionInfo &info) {
            collisions.push_back(info);
            hasCollision = true;
        }

        void clear() {
            collisions.clear();
            totalPenetration = glm::vec3(0.0f);
            hasCollision = false;
        }

        ash::Vector<CollisionInfo> getCollisionsOnAxis(const int axis) const {
            ash::Vector<CollisionInfo> result;
            for (const auto &col: collisions) {
                if (col.axis == axis) {
                    result.push_back(col);
                }
            }
            return result;
        }
    };

    struct PhysicsConfig {
        // Physique Minecraft exacte (Java Edition)
        float gravity = -32.0f; // Gravity: -0.08 blocks/tick = -32 m/s²
        float terminalVelocity = -78.4f; // Terminal velocity: -3.92 blocks/tick
        float groundFriction = 0.546f; // Block friction: 0.6 (ground) * 0.91 = 0.546
        float airDrag = 0.98f; // Air resistance: 0.98 (vertical)
        float horizontalAirDrag = 0.91f; // Air resistance: 0.91 (horizontal)
        float collisionEpsilon = 0.001f;
        bool useMaterialProperties = true;
    };

    class PhysicsSystem {
    public:
        explicit PhysicsSystem(const PhysicsConfig &config = PhysicsConfig());

        void step(Entity &entity, float deltaTime, const World &world) const;

        void setConfig(const PhysicsConfig &config) { m_config = config; }
        const PhysicsConfig &getConfig() const { return m_config; }

    private:
        PhysicsConfig m_config;

        void integrate(Entity &entity, float deltaTime, const World &world) const;

        glm::vec3 moveAndCollide(Entity &entity, const glm::vec3 &motion, const World &world) const;

        float sweepAxis(const ash::BBox3 &aabb, float motion, int axis,
                        const World &world, CollisionResult &result) const;

        static void getBroadPhaseBlocks(const ash::BBox3 &aabb,
                                        ash::Vector<glm::ivec3> &blocks,
                                        const World &world);

        void applyFriction(Entity &entity, float deltaTime, const World &world) const;

        float getGroundFriction(const Entity &entity, const World &world) const;
    };
}

#endif