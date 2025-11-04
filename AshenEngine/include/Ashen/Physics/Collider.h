#ifndef ASHEN_COLLIDER_H
#define ASHEN_COLLIDER_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Transform.h"
#include "Ashen/Math/BBox.h"
#include "PhysicsTypes.h"
#include "PhysicsMaterial.h"

namespace ash {
    class Rigidbody;
    class Node3D;

    enum class ColliderType {
        Box,
        Sphere,
        Capsule,
        Mesh
    };

    class Collider {
    public:
        Collider(ColliderType type);
        virtual ~Collider() = default;

        ColliderType GetType() const { return m_Type; }

        // Center offset from the node's origin
        Vec3 GetCenter() const { return m_Center; }
        void SetCenter(const Vec3& center) { m_Center = center; }

        // Trigger mode - no physics response, only callbacks
        bool IsTrigger() const { return m_IsTrigger; }
        void SetTrigger(bool isTrigger) { m_IsTrigger = isTrigger; }

        // Physics material
        PhysicsMaterial& GetMaterial() { return m_Material; }
        const PhysicsMaterial& GetMaterial() const { return m_Material; }
        void SetMaterial(const PhysicsMaterial& material) { m_Material = material; }

        // Attached rigidbody
        Rigidbody* GetRigidbody() const { return m_Rigidbody; }
        void SetRigidbody(Rigidbody* rb) { m_Rigidbody = rb; }

        // Attached node
        Node3D* GetNode() const { return m_Node; }
        void SetNode(Node3D* node) { m_Node = node; }

        // Bounds calculation
        virtual BBox<3, float> GetBounds() const = 0;

        // Collision detection
        virtual bool TestPoint(const Vec3& point) const = 0;
        virtual bool Intersects(const Collider* other, ContactPoint& contact) const = 0;

        // Debug visualization
        virtual void DebugDraw() const = 0;

    protected:
        ColliderType m_Type;
        Vec3 m_Center = Vec3(0.0f);
        bool m_IsTrigger = false;
        PhysicsMaterial m_Material;
        Rigidbody* m_Rigidbody = nullptr;
        Node3D* m_Node = nullptr;
    };
}

#endif // ASHEN_COLLIDER_H