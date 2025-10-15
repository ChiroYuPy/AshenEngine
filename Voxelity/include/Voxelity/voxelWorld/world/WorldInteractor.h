#ifndef VOXELITY_WORLDINTERACTOR_H
#define VOXELITY_WORLDINTERACTOR_H

#include "Ashen/Core/Types.h"

#include "Voxelity/voxelWorld/world/VoxelRaycaster.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    class World;
    class WorldRenderer;
    class Camera;

    class WorldInteractor {
    public:
        WorldInteractor(World &world, WorldRenderer &chunkRenderer);

        bool PlaceBlock(const glm::vec3 &cameraPos, const glm::vec3 &cameraDirection) const;

        bool BreakBlock(const glm::vec3 &cameraPos, const glm::vec3 &cameraDirection) const;

        bool placeBlockAt(const glm::ivec3 &position, int blockType = 1) const;

        bool breakBlockAt(const glm::ivec3 &position) const;

        void setMaxReach(const float reach) const { m_raycaster->setMaxDistance(reach); }
        float getMaxReach() const { return m_raycaster->getMaxDistance(); }

        void setSelectedVoxelID(const VoxelType blockType) { m_selectedBlockType = blockType; }
        int getSelectedBlockType() const { return m_selectedBlockType; }

        [[nodiscard]] std::optional<RaycastHit> getTargetedBlock(const glm::vec3 &cameraPos,
                                                                 const glm::vec3 &cameraDirection) const;

    private:
        World &m_world;
        WorldRenderer &m_chunkRenderer;
        ash::Own<VoxelRaycaster> m_raycaster;

        VoxelType m_selectedBlockType;
    };
}

#endif //VOXELITY_WORLDINTERACTOR_H
