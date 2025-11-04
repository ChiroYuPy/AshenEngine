#ifndef ASHEN_PHYSICS_MATERIAL_H
#define ASHEN_PHYSICS_MATERIAL_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class PhysicsMaterial {
    public:
        PhysicsMaterial(float friction = 0.6f, float bounciness = 0.0f, float density = 1.0f);

        float GetFriction() const { return m_Friction; }
        void SetFriction(float friction) { m_Friction = Clamp(friction, 0.0f, 1.0f); }

        float GetBounciness() const { return m_Bounciness; }
        void SetBounciness(float bounciness) { m_Bounciness = Clamp(bounciness, 0.0f, 1.0f); }

        float GetDensity() const { return m_Density; }
        void SetDensity(float density) { m_Density = Max(density, 0.001f); }

        // Static materials for common types
        static PhysicsMaterial Default();
        static PhysicsMaterial Ice();
        static PhysicsMaterial Rubber();
        static PhysicsMaterial Metal();
        static PhysicsMaterial Wood();

    private:
        float m_Friction;    // 0 = no friction, 1 = max friction
        float m_Bounciness;  // 0 = no bounce, 1 = perfect bounce
        float m_Density;     // kg/m³
    };
}

#endif // ASHEN_PHYSICS_MATERIAL_H
