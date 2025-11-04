#include "Ashen/Physics/PhysicsMaterial.h"
#include "Ashen/Math/Math.h"

namespace ash {
    PhysicsMaterial::PhysicsMaterial(float friction, float bounciness, float density)
        : m_Friction(Clamp(friction, 0.0f, 1.0f))
        , m_Bounciness(Clamp(bounciness, 0.0f, 1.0f))
        , m_Density(Max(density, 0.001f)) {
    }

    PhysicsMaterial PhysicsMaterial::Default() {
        return PhysicsMaterial(0.6f, 0.0f, 1.0f);
    }

    PhysicsMaterial PhysicsMaterial::Ice() {
        return PhysicsMaterial(0.1f, 0.1f, 0.92f);
    }

    PhysicsMaterial PhysicsMaterial::Rubber() {
        return PhysicsMaterial(1.0f, 0.8f, 1.1f);
    }

    PhysicsMaterial PhysicsMaterial::Metal() {
        return PhysicsMaterial(0.5f, 0.3f, 7.85f);
    }

    PhysicsMaterial PhysicsMaterial::Wood() {
        return PhysicsMaterial(0.7f, 0.2f, 0.7f);
    }
}
