#include "Voxelity/voxelWorld/world/WorldInteractor.h"

#include <format>

#include "Ashen/Core/Logger.h"

#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/render/WorldRenderer.h"

namespace voxelity {
    WorldInteractor::WorldInteractor(World &world, WorldRenderer &chunkRenderer)
        : m_world(world), m_chunkRenderer(chunkRenderer), m_selectedBlockType(VoxelID::DIRT) {
        m_raycaster = std::make_unique<VoxelRaycaster>(world);
    }

    bool WorldInteractor::PlaceBlock(const glm::vec3 &cameraPos, const glm::vec3 &cameraDirection) const {
        const auto hit = m_raycaster->raycastFromCamera(cameraPos, cameraDirection);
        if (!hit) return false;

        const glm::ivec3 placePos = hit->blockPos + hit->normalFace;

        return placeBlockAt(placePos, m_selectedBlockType);
    }

    bool WorldInteractor::BreakBlock(const glm::vec3 &cameraPos, const glm::vec3 &cameraDirection) const {
        const auto hit = m_raycaster->raycastFromCamera(cameraPos, cameraDirection);
        if (!hit) return false;

        return breakBlockAt(hit->blockPos);
    }

    bool WorldInteractor::placeBlockAt(const glm::ivec3 &position, const int blockType) const {
        if (m_world.getVoxel(position.x, position.y, position.z) != VoxelID::AIR) {
            ash::Logger::Warn() << "Cannot place block: position already occupied";
            return false;
        }

        m_world.setVoxel(position.x, position.y, position.z, blockType);

        ash::Logger::Info() << std::format("Block placed at ({}, {}, {})", position.x, position.y, position.z);
        return true;
    }

    bool WorldInteractor::breakBlockAt(const glm::ivec3 &position) const {
        if (m_world.getVoxel(position.x, position.y, position.z) == VoxelID::AIR) {
            ash::Logger::Warn() << "Cannot break block: no block at position";
            return false;
        }

        m_world.setVoxel(position.x, position.y, position.z, VoxelID::AIR);

        ash::Logger::Info() << std::format("Block broken at ({}, {}, {})", position.x, position.y, position.z);
        return true;
    }

    std::optional<RaycastHit> WorldInteractor::getTargetedBlock(const glm::vec3 &cameraPos,
                                                                const glm::vec3 &cameraDirection) const {
        return m_raycaster->raycastFromCamera(cameraPos, cameraDirection);
    }
}