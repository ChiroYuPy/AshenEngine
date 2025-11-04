#ifndef ASHEN_RIGIDBODY_H
#define ASHEN_RIGIDBODY_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Transform.h"
#include "Ashen/Math/Math.h"
#include "PhysicsTypes.h"

namespace ash {
    class Node3D;
    class Collider;
    class PhysicsWorld;

    class Rigidbody {
    public:
        Rigidbody(Node3D* node);
        ~Rigidbody();

        // Body type
        BodyType GetBodyType() const { return m_BodyType; }
        void SetBodyType(BodyType type);

        // Mass properties
        float GetMass() const { return m_Mass; }
        void SetMass(float mass);

        float GetInverseMass() const { return m_InverseMass; }

        bool UseGravity() const { return m_UseGravity; }
        void SetUseGravity(bool useGravity) { m_UseGravity = useGravity; }

        // Velocity
        Vec3 GetVelocity() const { return m_Velocity; }
        void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }

        Vec3 GetAngularVelocity() const { return m_AngularVelocity; }
        void SetAngularVelocity(const Vec3& velocity) { m_AngularVelocity = velocity; }

        // Drag
        float GetDrag() const { return m_Drag; }
        void SetDrag(float drag) { m_Drag = Max(drag, 0.0f); }

        float GetAngularDrag() const { return m_AngularDrag; }
        void SetAngularDrag(float drag) { m_AngularDrag = Max(drag, 0.0f); }

        // Constraints
        bool IsFreezePositionX() const { return m_FreezePositionX; }
        void SetFreezePositionX(bool freeze) { m_FreezePositionX = freeze; }

        bool IsFreezePositionY() const { return m_FreezePositionY; }
        void SetFreezePositionY(bool freeze) { m_FreezePositionY = freeze; }

        bool IsFreezePositionZ() const { return m_FreezePositionZ; }
        void SetFreezePositionZ(bool freeze) { m_FreezePositionZ = freeze; }

        bool IsFreezeRotationX() const { return m_FreezeRotationX; }
        void SetFreezeRotationX(bool freeze) { m_FreezeRotationX = freeze; }

        bool IsFreezeRotationY() const { return m_FreezeRotationY; }
        void SetFreezeRotationY(bool freeze) { m_FreezeRotationY = freeze; }

        bool IsFreezeRotationZ() const { return m_FreezeRotationZ; }
        void SetFreezeRotationZ(bool freeze) { m_FreezeRotationZ = freeze; }

        // Collision detection
        CollisionDetectionMode GetCollisionDetectionMode() const { return m_CollisionDetectionMode; }
        void SetCollisionDetectionMode(CollisionDetectionMode mode) { m_CollisionDetectionMode = mode; }

        // Force application
        void AddForce(const Vec3& force, ForceMode mode = ForceMode::Force);
        void AddForceAtPosition(const Vec3& force, const Vec3& position, ForceMode mode = ForceMode::Force);
        void AddTorque(const Vec3& torque, ForceMode mode = ForceMode::Force);

        // Sleeping
        bool IsSleeping() const { return m_IsSleeping; }
        void WakeUp() { m_IsSleeping = false; }
        void Sleep() { m_IsSleeping = true; }

        // Position/Rotation (for kinematic bodies)
        void MovePosition(const Vec3& position);
        void MoveRotation(const Quaternion& rotation);

        // Attached node and collider
        Node3D* GetNode() const { return m_Node; }
        Collider* GetCollider() const { return m_Collider; }
        void SetCollider(Collider* collider) { m_Collider = collider; }

        // Physics world
        PhysicsWorld* GetWorld() const { return m_World; }
        void SetWorld(PhysicsWorld* world) { m_World = world; }

        // Internal physics integration
        void IntegrateForces(float deltaTime);
        void IntegrateVelocity(float deltaTime);
        void ClearForces();

    private:
        Node3D* m_Node;
        Collider* m_Collider = nullptr;
        PhysicsWorld* m_World = nullptr;

        BodyType m_BodyType = BodyType::Dynamic;
        float m_Mass = 1.0f;
        float m_InverseMass = 1.0f;
        bool m_UseGravity = true;

        Vec3 m_Velocity = Vec3(0.0f);
        Vec3 m_AngularVelocity = Vec3(0.0f);
        Vec3 m_Force = Vec3(0.0f);
        Vec3 m_Torque = Vec3(0.0f);

        float m_Drag = 0.0f;
        float m_AngularDrag = 0.05f;

        bool m_FreezePositionX = false;
        bool m_FreezePositionY = false;
        bool m_FreezePositionZ = false;
        bool m_FreezeRotationX = false;
        bool m_FreezeRotationY = false;
        bool m_FreezeRotationZ = false;

        CollisionDetectionMode m_CollisionDetectionMode = CollisionDetectionMode::Discrete;
        bool m_IsSleeping = false;

        void UpdateInverseMass();
    };
}

#endif // ASHEN_RIGIDBODY_H