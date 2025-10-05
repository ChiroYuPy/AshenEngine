#ifndef VOXELITY_PHYSICSSYSTEM_H
#define VOXELITY_PHYSICSSYSTEM_H

#include <vector>
#include <glm/glm.hpp>

#include "Voxelity/entities/Entity.h"

namespace voxelity {
    class World;

    // Résultat d'une détection de collision
    struct CollisionInfo {
        glm::ivec3 blockPos; // Position du bloc
        glm::vec3 penetration; // Vecteur de pénétration
        int axis; // Axe de collision (0=X, 1=Y, 2=Z)
        float distance; // Distance de pénétration

        CollisionInfo()
            : blockPos(0), penetration(0.0f), axis(0), distance(0.0f) {
        }

        CollisionInfo(const glm::ivec3 &pos, const glm::vec3 &pen, int ax, float dist)
            : blockPos(pos), penetration(pen), axis(ax), distance(dist) {
        }
    };

    // Résultat de toutes les collisions d'un sweep
    struct CollisionResult {
        bool hasCollision = false;
        std::vector<CollisionInfo> collisions;
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

        // Trouve toutes les collisions sur un axe spécifique
        std::vector<CollisionInfo> getCollisionsOnAxis(const int axis) const {
            std::vector<CollisionInfo> result;
            for (const auto &col: collisions) {
                if (col.axis == axis) {
                    result.push_back(col);
                }
            }
            return result;
        }
    };

    struct PhysicsConfig {
        float gravity = -32.0f; // Plus réaliste (environ 3x la gravité réelle)
        float terminalVelocity = -78.4f; // Vitesse terminale
        float groundFriction = 0.6f; // Friction au sol
        float airDrag = 0.02f; // Résistance de l'air (petit)
        float collisionEpsilon = 0.001f; // Marge d'erreur pour collisions
    };

    // Système de physique réutilisable
    class PhysicsSystem {
    public:
        explicit PhysicsSystem(const PhysicsConfig &config = PhysicsConfig());

        // Applique la physique et résout les collisions en une passe
        void step(Entity &entity, float deltaTime, const World &world) const;

        // Configuration
        void setConfig(const PhysicsConfig &config) { m_config = config; }
        const PhysicsConfig &getConfig() const { return m_config; }

    private:
        PhysicsConfig m_config;

        // Intégration de la physique (gravité, friction)
        void integrate(Entity &entity, float deltaTime, const World &world) const;

        // Déplacement avec détection de collision (sweep)
        glm::vec3 moveAndCollide(Entity &entity, const glm::vec3 &motion, const World &world) const;

        // Déplacement sur un seul axe avec collision
        float sweepAxis(const pixl::AABB &aabb, float motion, int axis, const World &world,
                        CollisionResult &result) const;

        // Détection de tous les blocs potentiellement en collision
        static void getBroadPhaseBlocks(const pixl::AABB &aabb, std::vector<glm::ivec3> &blocks, const World &world);

        // Applique la friction
        void applyFriction(Entity &entity, float deltaTime) const;
    };
}

#endif //VOXELITY_PHYSICSSYSTEM_H