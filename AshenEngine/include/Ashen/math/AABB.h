#ifndef ASHEN_AABB_H
#define ASHEN_AABB_H

#include <glm/glm.hpp>
#include <algorithm>

#include "Math.h"

namespace ash {
    struct AABB {
        Vec3 min;
        Vec3 max;

        explicit AABB() : min(0.0f), max(0.0f) {
        }

        explicit AABB(const Vec3 &center, const Vec3 &size) {
            const Vec3 halfSize = size * 0.5f;
            min = center - halfSize;
            max = center + halfSize;
        }

        explicit AABB(const Vec3 &minPos, const Vec3 &maxPos, bool)
            : min(minPos), max(maxPos) {
        }

        // Centre et taille
        Vec3 center() const { return (min + max) * 0.5f; }
        Vec3 size() const { return max - min; }
        Vec3 halfSize() const { return size() * 0.5f; }

        // Expansion
        AABB expand(const Vec3 &amount) const {
            return AABB(min - amount, max + amount, true);
        }

        AABB expand(const float amount) const {
            return expand(Vec3(amount));
        }

        // Déplacement
        AABB offset(const Vec3 &delta) const {
            return AABB(min + delta, max + delta, true);
        }

        // Tests d'intersection
        bool intersects(const AABB &other) const {
            return (min.x < other.max.x && max.x > other.min.x) &&
                   (min.y < other.max.y && max.y > other.min.y) &&
                   (min.z < other.max.z && max.z > other.min.z);
        }

        bool contains(const Vec3 &point) const {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }

        // Calcule la pénétration sur chaque axe si intersection
        [[nodiscard]] Vec3 getPenetration(const AABB &other) const {
            if (!intersects(other)) return Vec3(0.0f);

            Vec3 penetration;

            // X axis
            const float overlapLeft = max.x - other.min.x;
            const float overlapRight = other.max.x - min.x;
            penetration.x = std::min(overlapLeft, overlapRight);
            if (center().x < other.center().x) penetration.x = -penetration.x;

            // Y axis
            const float overlapBottom = max.y - other.min.y;
            const float overlapTop = other.max.y - min.y;
            penetration.y = std::min(overlapBottom, overlapTop);
            if (center().y < other.center().y) penetration.y = -penetration.y;

            // Z axis
            const float overlapFront = max.z - other.min.z;
            const float overlapBack = other.max.z - min.z;
            penetration.z = std::min(overlapFront, overlapBack);
            if (center().z < other.center().z) penetration.z = -penetration.z;

            return penetration;
        }

        // Trouve l'axe avec la plus petite pénétration
        static int getMinPenetrationAxis(const Vec3 &penetration) {
            const float absX = std::abs(penetration.x);
            const float absY = std::abs(penetration.y);
            const float absZ = std::abs(penetration.z);

            if (absX < absY && absX < absZ) return 0; // X
            if (absY < absZ) return 1; // Y
            return 2; // Z
        }

        // AABB d'un bloc voxel
        static AABB fromBlock(const IVec3 &blockPos) {
            return AABB(Vec3(blockPos), Vec3(blockPos) + Vec3(1.0f), true);
        }
    };
}

#endif //ASHEN_AABB_H