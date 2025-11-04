#include "Ashen/Physics/PhysicsWorld.h"
#include "Ashen/Physics/Rigidbody.h"
#include "Ashen/Physics/Collider.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Math/Math.h"

namespace ash {
    PhysicsWorld::PhysicsWorld() {
    }

    PhysicsWorld::~PhysicsWorld() {
    }

    void PhysicsWorld::Step(float deltaTime) {
        printf("DEBUG: PhysicsWorld::Step called with %zu rigidbodies\n", m_Rigidbodies.size());

        IntegrateForces(deltaTime);
        DetectCollisions();
        ResolveCollisions();
        IntegrateVelocities(deltaTime);

        // Clear forces after integration
        for (Rigidbody* rb : m_Rigidbodies) {
            rb->ClearForces();
        }
    }

    void PhysicsWorld::FixedStep(float fixedDeltaTime) {
        Step(fixedDeltaTime);
    }

    void PhysicsWorld::AddRigidbody(Rigidbody* rb) {
        if (!rb) return;

        auto it = std::find(m_Rigidbodies.begin(), m_Rigidbodies.end(), rb);
        if (it == m_Rigidbodies.end()) {
            m_Rigidbodies.push_back(rb);
            rb->SetWorld(this);
        }
    }

    void PhysicsWorld::RemoveRigidbody(Rigidbody* rb) {
        if (!rb) return;

        auto it = std::find(m_Rigidbodies.begin(), m_Rigidbodies.end(), rb);
        if (it != m_Rigidbodies.end()) {
            m_Rigidbodies.erase(it);
            rb->SetWorld(nullptr);
        }
    }

    void PhysicsWorld::AddCollider(Collider* collider) {
        if (!collider) return;

        auto it = std::find(m_Colliders.begin(), m_Colliders.end(), collider);
        if (it == m_Colliders.end()) {
            m_Colliders.push_back(collider);
        }
    }

    void PhysicsWorld::RemoveCollider(Collider* collider) {
        if (!collider) return;

        auto it = std::find(m_Colliders.begin(), m_Colliders.end(), collider);
        if (it != m_Colliders.end()) {
            m_Colliders.erase(it);
        }
    }

    bool PhysicsWorld::Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const {
        Vec3 dir = Normalize(direction);
        float closestDistance = maxDistance;
        bool hitFound = false;

        for (Collider* collider : m_Colliders) {
            BBox<3, float> bounds = collider->GetBounds();

            // Ray-AABB intersection first (broad phase)
            Vec3 invDir = Vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
            Vec3 t0 = (bounds.min - origin) * invDir;
            Vec3 t1 = (bounds.max - origin) * invDir;

            Vec3 tmin = Vec3(Min(t0.x, t1.x), Min(t0.y, t1.y), Min(t0.z, t1.z));
            Vec3 tmax = Vec3(Max(t0.x, t1.x), Max(t0.y, t1.y), Max(t0.z, t1.z));

            float tMinMax = Max(Max(tmin.x, tmin.y), tmin.z);
            float tMaxMin = Min(Min(tmax.x, tmax.y), tmax.z);

            if (tMaxMin >= tMinMax && tMinMax < closestDistance) {
                // Detailed intersection test would go here
                // For now, use AABB hit point
                float distance = Max(tMinMax, 0.0f);

                if (distance < closestDistance) {
                    closestDistance = distance;
                    hit.distance = distance;
                    hit.point = origin + dir * distance;
                    hit.collider = collider;
                    hit.rigidbody = collider->GetRigidbody();

                    // Approximate normal from AABB face
                    Vec3 center = (bounds.min + bounds.max) * 0.5f;
                    Vec3 localHit = hit.point - center;
                    Vec3 extents = (bounds.max - bounds.min) * 0.5f;

                    Vec3 normal(0.0f);
                    float minDist = Infinity<float>();

                    for (int i = 0; i < 3; ++i) {
                        float dist = Abs(Abs(localHit[i]) - extents[i]);
                        if (dist < minDist) {
                            minDist = dist;
                            normal = Vec3(0.0f);
                            normal[i] = localHit[i] > 0 ? 1.0f : -1.0f;
                        }
                    }

                    hit.normal = normal;
                    hitFound = true;
                }
            }
        }

        return hitFound;
    }

    bool PhysicsWorld::RaycastAll(const Vec3& origin, const Vec3& direction, float maxDistance, Vector<RaycastHit>& hits) const {
        Vec3 dir = Normalize(direction);
        hits.clear();

        for (Collider* collider : m_Colliders) {
            RaycastHit hit;
            if (Raycast(origin, dir, maxDistance, hit)) {
                hits.push_back(hit);
            }
        }

        return !hits.empty();
    }

    bool PhysicsWorld::SphereCast(const Vec3& origin, float radius, const Vec3& direction, float maxDistance, RaycastHit& hit) const {
        // Simplified sphere cast using raycast
        // A full implementation would expand the colliders by radius
        return Raycast(origin, direction, maxDistance, hit);
    }

    Vector<Collider*> PhysicsWorld::OverlapSphere(const Vec3& position, float radius) const {
        Vector<Collider*> overlapping;

        for (Collider* collider : m_Colliders) {
            BBox<3, float> bounds = collider->GetBounds();
            Vec3 closestPoint;
            closestPoint.x = Clamp(position.x, bounds.min.x, bounds.max.x);
            closestPoint.y = Clamp(position.y, bounds.min.y, bounds.max.y);
            closestPoint.z = Clamp(position.z, bounds.min.z, bounds.max.z);

            if (LengthSqr(closestPoint - position) <= radius * radius) {
                overlapping.push_back(collider);
            }
        }

        return overlapping;
    }

    Vector<Collider*> PhysicsWorld::OverlapBox(const Vec3& center, const Vec3& halfExtents) const {
        Vector<Collider*> overlapping;

        for (Collider* collider : m_Colliders) {
            BBox<3, float> bounds = collider->GetBounds();

            // AABB intersection
            bool intersects = (Abs(center.x - (bounds.min.x + bounds.max.x) * 0.5f) < halfExtents.x + (bounds.max.x - bounds.min.x) * 0.5f) &&
                             (Abs(center.y - (bounds.min.y + bounds.max.y) * 0.5f) < halfExtents.y + (bounds.max.y - bounds.min.y) * 0.5f) &&
                             (Abs(center.z - (bounds.min.z + bounds.max.z) * 0.5f) < halfExtents.z + (bounds.max.z - bounds.min.z) * 0.5f);

            if (intersects) {
                overlapping.push_back(collider);
            }
        }

        return overlapping;
    }

    void PhysicsWorld::IntegrateForces(float deltaTime) {
        Logger::Info() << "IntegrateForces: Processing " << m_Rigidbodies.size() << " rigidbodies";
        for (Rigidbody* rb : m_Rigidbodies) {
            rb->IntegrateForces(deltaTime);
        }
    }

    void PhysicsWorld::DetectCollisions() {
        m_CollisionPairs.clear();
        BroadPhaseCollisionDetection();
        NarrowPhaseCollisionDetection();
    }

    void PhysicsWorld::BroadPhaseCollisionDetection() {
        // Simple O(n²) broad phase for now
        // Could be optimized with spatial partitioning (quadtree, octree, etc.)
        for (size_t i = 0; i < m_Rigidbodies.size(); ++i) {
            for (size_t j = i + 1; j < m_Rigidbodies.size(); ++j) {
                Rigidbody* rbA = m_Rigidbodies[i];
                Rigidbody* rbB = m_Rigidbodies[j];

                // Skip if both are static or sleeping
                if ((rbA->GetBodyType() == BodyType::Static && rbB->GetBodyType() == BodyType::Static) ||
                    (rbA->IsSleeping() && rbB->IsSleeping())) {
                    continue;
                }

                Collider* colliderA = rbA->GetCollider();
                Collider* colliderB = rbB->GetCollider();

                if (!colliderA || !colliderB) continue;

                // AABB overlap test
                BBox<3, float> boundsA = colliderA->GetBounds();
                BBox<3, float> boundsB = colliderB->GetBounds();

                bool overlap = (boundsA.min.x <= boundsB.max.x && boundsA.max.x >= boundsB.min.x) &&
                              (boundsA.min.y <= boundsB.max.y && boundsA.max.y >= boundsB.min.y) &&
                              (boundsA.min.z <= boundsB.max.z && boundsA.max.z >= boundsB.min.z);

                if (overlap) {
                    CollisionPair pair;
                    pair.rbA = rbA;
                    pair.rbB = rbB;
                    m_CollisionPairs.push_back(pair);
                }
            }
        }
    }

    void PhysicsWorld::NarrowPhaseCollisionDetection() {
        for (CollisionPair& pair : m_CollisionPairs) {
            Collider* colliderA = pair.rbA->GetCollider();
            Collider* colliderB = pair.rbB->GetCollider();

            if (colliderA && colliderB) {
                ContactPoint contact;
                if (colliderA->Intersects(colliderB, contact)) {
                    pair.contact = contact;
                } else {
                    // Remove pairs without actual collision
                    pair.rbA = nullptr;
                }
            }
        }

        // Remove invalid pairs
        m_CollisionPairs.erase(
            std::remove_if(m_CollisionPairs.begin(), m_CollisionPairs.end(),
                [](const CollisionPair& pair) { return pair.rbA == nullptr; }),
            m_CollisionPairs.end()
        );
    }

    void PhysicsWorld::ResolveCollisions() {
        for (int iteration = 0; iteration < m_SolverIterations; ++iteration) {
            for (CollisionPair& pair : m_CollisionPairs) {
                ResolveCollision(pair);
            }
        }
    }

    void PhysicsWorld::ResolveCollision(CollisionPair& pair) {
        Rigidbody* rbA = pair.rbA;
        Rigidbody* rbB = pair.rbB;
        ContactPoint& contact = pair.contact;

        // Skip if either is a trigger
        if (contact.thisCollider->IsTrigger() || contact.otherCollider->IsTrigger()) {
            return;
        }

        float invMassA = rbA->GetInverseMass();
        float invMassB = rbB->GetInverseMass();
        float totalInvMass = invMassA + invMassB;

        if (totalInvMass < 0.0001f) return; // Both static

        // Position correction (separate objects)
        Vec3 correction = contact.normal * contact.penetration / totalInvMass;
        if (rbA->GetBodyType() == BodyType::Dynamic) {
            Vec3 newPos = rbA->GetNode()->GetGlobalPosition() - correction * invMassA;
            rbA->GetNode()->SetGlobalPosition(newPos);
        }
        if (rbB->GetBodyType() == BodyType::Dynamic) {
            Vec3 newPos = rbB->GetNode()->GetGlobalPosition() + correction * invMassB;
            rbB->GetNode()->SetGlobalPosition(newPos);
        }

        // Velocity resolution (impulse-based)
        Vec3 relativeVel = rbB->GetVelocity() - rbA->GetVelocity();
        float velAlongNormal = Dot(relativeVel, contact.normal);

        if (velAlongNormal > 0) return; // Objects separating

        // Calculate restitution (bounciness)
        float restitution = Min(
            contact.thisCollider->GetMaterial().GetBounciness(),
            contact.otherCollider->GetMaterial().GetBounciness()
        );

        // Calculate impulse
        float j = -(1.0f + restitution) * velAlongNormal;
        j /= totalInvMass;

        Vec3 impulse = contact.normal * j;

        if (rbA->GetBodyType() == BodyType::Dynamic) {
            rbA->SetVelocity(rbA->GetVelocity() - impulse * invMassA);
            rbA->WakeUp();
        }
        if (rbB->GetBodyType() == BodyType::Dynamic) {
            rbB->SetVelocity(rbB->GetVelocity() + impulse * invMassB);
            rbB->WakeUp();
        }

        // Apply friction
        Vec3 tangent = relativeVel - contact.normal * velAlongNormal;
        float tangentLength = Length(tangent);

        if (tangentLength > 0.0001f) {
            tangent /= tangentLength;

            float friction = Sqrt(
                contact.thisCollider->GetMaterial().GetFriction() *
                contact.otherCollider->GetMaterial().GetFriction()
            );

            float jt = -Dot(relativeVel, tangent) * friction;
            jt /= totalInvMass;

            Vec3 frictionImpulse = tangent * jt;

            if (rbA->GetBodyType() == BodyType::Dynamic) {
                rbA->SetVelocity(rbA->GetVelocity() - frictionImpulse * invMassA);
            }
            if (rbB->GetBodyType() == BodyType::Dynamic) {
                rbB->SetVelocity(rbB->GetVelocity() + frictionImpulse * invMassB);
            }
        }
    }

    void PhysicsWorld::IntegrateVelocities(float deltaTime) {
        for (Rigidbody* rb : m_Rigidbodies) {
            rb->IntegrateVelocity(deltaTime);
        }
    }

    void PhysicsWorld::DebugDraw() const {
        for (Collider* collider : m_Colliders) {
            collider->DebugDraw();
        }
    }
}