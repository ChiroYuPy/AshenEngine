#include "Ashen/Physics/Rigidbody.h"
#include "Ashen/Physics/PhysicsWorld.h"
#include "Ashen/Physics/Collider.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Math.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Rigidbody::Rigidbody(Node3D* node)
        : m_Node(node) {
        UpdateInverseMass();
    }

    Rigidbody::~Rigidbody() {
        if (m_World) {
            m_World->RemoveRigidbody(this);
        }
    }

    void Rigidbody::SetBodyType(BodyType type) {
        m_BodyType = type;
        UpdateInverseMass();

        if (type == BodyType::Static) {
            m_Velocity = Vec3(0.0f);
            m_AngularVelocity = Vec3(0.0f);
        }
    }

    void Rigidbody::SetMass(float mass) {
        m_Mass = Max(mass, 0.001f);
        UpdateInverseMass();
    }

    void Rigidbody::UpdateInverseMass() {
        if (m_BodyType == BodyType::Dynamic) {
            m_InverseMass = 1.0f / m_Mass;
        } else {
            m_InverseMass = 0.0f; // Infinite mass for static/kinematic
        }
    }

    void Rigidbody::AddForce(const Vec3& force, ForceMode mode) {
        if (m_BodyType != BodyType::Dynamic) return;

        switch (mode) {
            case ForceMode::Force:
                m_Force += force;
                break;
            case ForceMode::Acceleration:
                m_Force += force * m_Mass;
                break;
            case ForceMode::Impulse:
                m_Velocity += force * m_InverseMass;
                break;
            case ForceMode::VelocityChange:
                m_Velocity += force;
                break;
        }

        WakeUp();
    }

    void Rigidbody::AddForceAtPosition(const Vec3& force, const Vec3& position, ForceMode mode) {
        AddForce(force, mode);

        // Calculate torque from force at position
        Vec3 relativePos = position - m_Node->GetGlobalPosition();
        Vec3 torque = Cross(relativePos, force);
        AddTorque(torque, mode);
    }

    void Rigidbody::AddTorque(const Vec3& torque, ForceMode mode) {
        if (m_BodyType != BodyType::Dynamic) return;

        switch (mode) {
            case ForceMode::Force:
                m_Torque += torque;
                break;
            case ForceMode::Acceleration:
                m_Torque += torque * m_Mass;
                break;
            case ForceMode::Impulse:
                m_AngularVelocity += torque * m_InverseMass;
                break;
            case ForceMode::VelocityChange:
                m_AngularVelocity += torque;
                break;
        }

        WakeUp();
    }

    void Rigidbody::MovePosition(const Vec3& position) {
        if (m_BodyType == BodyType::Kinematic) {
            m_Node->SetGlobalPosition(position);
        }
    }

    void Rigidbody::MoveRotation(const Quaternion& rotation) {
        if (m_BodyType == BodyType::Kinematic) {
            m_Node->SetRotation(rotation);
        }
    }

    void Rigidbody::IntegrateForces(float deltaTime) {
        if (m_BodyType != BodyType::Dynamic || m_IsSleeping) {
            printf("DEBUG: IntegrateForces skipped - BodyType=%d Sleeping=%d\n", (int)m_BodyType, m_IsSleeping);
            return;
        }

        // Apply gravity
        if (m_UseGravity && m_World) {
            m_Force += m_World->GetGravity() * m_Mass;
            printf("DEBUG: Applying gravity - Gravity.y=%.2f Force.y=%.2f\n", m_World->GetGravity().y, m_Force.y);
        } else {
            printf("DEBUG: Gravity not applied - UseGravity=%d World=%p\n", m_UseGravity, m_World);
        }

        // Integrate acceleration
        Vec3 acceleration = m_Force * m_InverseMass;
        m_Velocity += acceleration * deltaTime;
        printf("DEBUG: After integrate - Velocity.y=%.2f Acceleration.y=%.2f\n", m_Velocity.y, acceleration.y);

        // Apply drag
        m_Velocity *= (1.0f - m_Drag * deltaTime);
        m_AngularVelocity *= (1.0f - m_AngularDrag * deltaTime);
    }

    void Rigidbody::IntegrateVelocity(float deltaTime) {
        if (m_BodyType != BodyType::Dynamic || m_IsSleeping) {
            Logger::Info() << "IntegrateVelocity skipped: BodyType=" << (int)m_BodyType << " Sleeping=" << m_IsSleeping;
            return;
        }

        // Apply constraints
        Vec3 velocity = m_Velocity;
        if (m_FreezePositionX) velocity.x = 0.0f;
        if (m_FreezePositionY) velocity.y = 0.0f;
        if (m_FreezePositionZ) velocity.z = 0.0f;

        Vec3 angularVelocity = m_AngularVelocity;
        if (m_FreezeRotationX) angularVelocity.x = 0.0f;
        if (m_FreezeRotationY) angularVelocity.y = 0.0f;
        if (m_FreezeRotationZ) angularVelocity.z = 0.0f;

        // Integrate position
        Vec3 oldPosition = m_Node->GetGlobalPosition();
        Vec3 newPosition = oldPosition + velocity * deltaTime;
        m_Node->SetGlobalPosition(newPosition);
        Logger::Info() << "Position: " << oldPosition.y << " -> " << newPosition.y << " (delta=" << (velocity.y * deltaTime) << ")";

        // Integrate rotation
        if (LengthSqr(angularVelocity) > 0.0001f) {
            float angle = Length(angularVelocity) * deltaTime;
            Vec3 axis = Normalize(angularVelocity);
            Quaternion deltaRotation = glm::angleAxis(angle, axis);
            Quaternion newRotation = deltaRotation * m_Node->GetRotation();
            m_Node->SetRotation(glm::normalize(newRotation));
        }

        // Check for sleeping
        float velLengthSqr = LengthSqr(velocity);
        if (m_World && velLengthSqr < m_World->GetSleepThreshold()) {
            Logger::Info() << "Going to sleep: velLengthSqr=" << velLengthSqr << " threshold=" << m_World->GetSleepThreshold();
            Sleep();
        }
    }

    void Rigidbody::ClearForces() {
        m_Force = Vec3(0.0f);
        m_Torque = Vec3(0.0f);
    }
}