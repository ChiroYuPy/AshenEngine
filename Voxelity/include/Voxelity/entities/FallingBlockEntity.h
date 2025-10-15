#ifndef VOXELITY_FALLINGBLOCKENTITY_H
#define VOXELITY_FALLINGBLOCKENTITY_H

#include "Ashen/Core/Types.h"

#include "Voxelity/voxelWorld/voxel/VoxelType.h"
#include "Voxelity/systems/PhysicsSystem.h"

namespace voxelity {
    class FallingBlockEntity final : public Entity {
    public:
        explicit FallingBlockEntity(VoxelType blockType = VoxelID::DIRT);

        void update(float deltaTime, const World &world) override;

        [[nodiscard]] VoxelType getBlockType() const { return m_blockType; }
        void setBlockType(const VoxelType type) { m_blockType = type; }

        // Durée de vie (se désactive après un certain temps)
        void setLifetime(const float seconds) { m_lifetime = seconds; }
        [[nodiscard]] float getLifetime() const { return m_lifetime; }

    private:
        VoxelType m_blockType;
        ash::Own<PhysicsSystem> m_physics;

        float m_lifetime = 5.0f;
        float m_age = 0.0f;

        void checkLanding(const World &world);
    };
}

#endif //VOXELITY_FALLINGBLOCKENTITY_H