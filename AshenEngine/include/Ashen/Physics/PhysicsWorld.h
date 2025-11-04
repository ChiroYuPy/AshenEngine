#ifndef ASHEN_PHYSICS_WORLD_H
#define ASHEN_PHYSICS_WORLD_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "PhysicsTypes.h"

namespace ash {
    class Rigidbody;
    class Collider;

    class PhysicsWorld {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        // Simulation
        void Step(float deltaTime);
        void FixedStep(float fixedDeltaTime);

        // Gravity
        Vec3 GetGravity() const { return m_Gravity; }
        void SetGravity(const Vec3& gravity) { m_Gravity = gravity; }

        // Rigidbody management
        void AddRigidbody(Rigidbody* rb);
        void RemoveRigidbody(Rigidbody* rb);
        const Vector<Rigidbody*>& GetRigidbodies() const { return m_Rigidbodies; }

        // Collider management
        void AddCollider(Collider* collider);
        void RemoveCollider(Collider* collider);
        const Vector<Collider*>& GetColliders() const { return m_Colliders; }

        // Raycasting
        bool Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const;
        bool RaycastAll(const Vec3& origin, const Vec3& direction, float maxDistance, Vector<RaycastHit>& hits) const;

        // Sphere cast
        bool SphereCast(const Vec3& origin, float radius, const Vec3& direction, float maxDistance, RaycastHit& hit) const;

        // Overlap tests
        Vector<Collider*> OverlapSphere(const Vec3& position, float radius) const;
        Vector<Collider*> OverlapBox(const Vec3& center, const Vec3& halfExtents) const;

        // Settings
        int GetSolverIterations() const { return m_SolverIterations; }
        void SetSolverIterations(int iterations) { m_SolverIterations = Max(iterations, 1); }

        float GetBounceThreshold() const { return m_BounceThreshold; }
        void SetBounceThreshold(float threshold) { m_BounceThreshold = threshold; }

        float GetSleepThreshold() const { return m_SleepThreshold; }
        void SetSleepThreshold(float threshold) { m_SleepThreshold = threshold; }

        // Debug
        void DebugDraw() const;

    private:
        Vector<Rigidbody*> m_Rigidbodies;
        Vector<Collider*> m_Colliders;

        Vec3 m_Gravity = Vec3(0.0f, -9.81f, 0.0f);
        int m_SolverIterations = 6;
        float m_BounceThreshold = 2.0f;
        float m_SleepThreshold = 0.005f;

        // Simulation steps
        void IntegrateForces(float deltaTime);
        void DetectCollisions();
        void ResolveCollisions();
        void IntegrateVelocities(float deltaTime);

        // Collision detection helpers
        struct CollisionPair {
            Rigidbody* rbA;
            Rigidbody* rbB;
            ContactPoint contact;
        };
        Vector<CollisionPair> m_CollisionPairs;

        void BroadPhaseCollisionDetection();
        void NarrowPhaseCollisionDetection();
        void ResolveCollision(CollisionPair& pair);
    };
}

#endif // ASHEN_PHYSICS_WORLD_H