#ifndef ASHEN_NODE3D_H
#define ASHEN_NODE3D_H

#include "Node.h"
#include "Ashen/Math/Transform.h"

namespace ash {
    class Node3D : public Node {
    public:
        explicit Node3D(String name = "Node3D");

        Transform3D local_transform;

        Transform3D GetGlobalTransform() const;

        void SetGlobalTransform(const Transform3D &transform);

        Vec3 GetPosition() const;

        void SetPosition(const Vec3 &pos);

        Quaternion GetRotation() const;

        void SetRotation(const Quaternion &rot);

        Vec3 GetScale() const;

        void SetScale(const Vec3 &scl);

        Vec3 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec3 &pos);

        Vec3 GetRight() const;

        Vec3 GetUp() const;

        Vec3 GetForward() const;

        Vec3 ToLocal(const Vec3 &global_point) const;

        Vec3 ToGlobal(const Vec3 &local_point) const;
    };
}

#endif //ASHEN_NODE3D_H
