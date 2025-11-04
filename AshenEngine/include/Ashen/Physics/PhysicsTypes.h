#ifndef ASHEN_PHYSICS_TYPES_H
#define ASHEN_PHYSICS_TYPES_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    enum class BodyType {
        Static,      // Non-moving, infinite mass
        Kinematic,   // Controlled by user, infinite mass
        Dynamic      // Affected by forces
    };

    enum class ForceMode {
        Force,          // Add continuous force, uses mass
        Impulse,        // Add instant force, uses mass
        VelocityChange, // Add instant velocity change, ignores mass
        Acceleration    // Add continuous acceleration, ignores mass
    };

    enum class CollisionDetectionMode {
        Discrete,    // Default, fast but may miss collisions
        Continuous   // More accurate, detects fast-moving collisions
    };

    struct RaycastHit {
        Vec3 point;
        Vec3 normal;
        float distance;
        class Collider* collider;
        class Rigidbody* rigidbody;
    };

    struct ContactPoint {
        Vec3 point;
        Vec3 normal;
        float penetration;
        Collider* thisCollider;
        Collider* otherCollider;
    };

    struct Collision {
        Vector<ContactPoint> contacts;
        Rigidbody* rigidbody;
        Vec3 relativeVelocity;
        Vec3 impulse;
    };
}

#endif // ASHEN_PHYSICS_TYPES_H