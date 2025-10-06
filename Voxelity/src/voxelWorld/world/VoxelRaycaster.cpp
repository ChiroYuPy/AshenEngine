#include "Voxelity/voxelWorld/world/VoxelRaycaster.h"

#include <algorithm>

#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {
    VoxelRaycaster::VoxelRaycaster(World &world)
        : m_world(world), m_maxDistance(10.f) {
    }

    std::optional<RaycastHit> VoxelRaycaster::raycast(const glm::vec3 &origin, const glm::vec3 &direction) const {
        const glm::vec3 normalizedDir = glm::normalize(direction);
        return performDDA(origin, normalizedDir);
    }

    std::optional<RaycastHit> VoxelRaycaster::raycastFromCamera(const glm::vec3 &cameraPos,
                                                                const glm::vec3 &cameraDirection) const {
        return raycast(cameraPos, cameraDirection);
    }

    std::optional<RaycastHit> VoxelRaycaster::performDDA(const glm::vec3 &origin, const glm::vec3 &direction) const {
        // Position actuelle du ray en coordonnées de bloc
        auto mapPos = glm::ivec3(glm::floor(origin));

        // Calcul des deltas et des pas pour chaque axe
        glm::vec3 deltaDist = glm::abs(1.0f / direction);
        glm::ivec3 step;
        glm::vec3 sideDist;

        // Initialisation pour l'axe X
        if (direction.x < 0) {
            step.x = -1;
            sideDist.x = (origin.x - static_cast<float>(mapPos.x)) * deltaDist.x;
        } else {
            step.x = 1;
            sideDist.x = (static_cast<float>(mapPos.x + 1) - origin.x) * deltaDist.x;
        }

        // Initialisation pour l'axe Y
        if (direction.y < 0) {
            step.y = -1;
            sideDist.y = (origin.y - float(mapPos.y)) * deltaDist.y;
        } else {
            step.y = 1;
            sideDist.y = (float(mapPos.y + 1) - origin.y) * deltaDist.y;
        }

        // Initialisation pour l'axe Z
        if (direction.z < 0) {
            step.z = -1;
            sideDist.z = (origin.z - float(mapPos.z)) * deltaDist.z;
        } else {
            step.z = 1;
            sideDist.z = (float(mapPos.z + 1) - origin.z) * deltaDist.z;
        }

        // Variables pour tracker quelle face a été touchée
        int side = 0; // 0=X, 1=Y, 2=Z
        glm::ivec3 previousPos = mapPos;

        // Algorithme DDA principal
        while (true) {
            // Vérifier si on a touché un bloc solide
            if (isBlockSolid(mapPos)) {
                // Calculer la distance
                float distance = 0.0f;
                glm::vec3 hitPoint;

                if (side == 0) {
                    distance = (float(mapPos.x) - origin.x + (1 - step.x) / 2) / direction.x;
                    hitPoint = origin + direction * distance;
                } else if (side == 1) {
                    distance = (float(mapPos.y) - origin.y + (1 - step.y) / 2) / direction.y;
                    hitPoint = origin + direction * distance;
                } else {
                    distance = (float(mapPos.z) - origin.z + (1 - step.z) / 2) / direction.z;
                    hitPoint = origin + direction * distance;
                }

                // Vérifier si on dépasse la distance max
                if (distance > m_maxDistance) {
                    return std::nullopt;
                }

                // Calculer la normale de la face
                glm::ivec3 normal = calculateFaceNormal(mapPos, previousPos);

                return RaycastHit{
                    mapPos,
                    normal,
                    hitPoint,
                    distance
                };
            }

            // Vérifier si on dépasse la distance max
            float currentDist = std::min({sideDist.x, sideDist.y, sideDist.z});
            if (currentDist > m_maxDistance) {
                return std::nullopt;
            }

            // Sauvegarder la position précédente
            previousPos = mapPos;

            // Avancer vers la prochaine cellule
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += step.x;
                    side = 0;
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += step.z;
                    side = 2;
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    sideDist.y += deltaDist.y;
                    mapPos.y += step.y;
                    side = 1;
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += step.z;
                    side = 2;
                }
            }
        }

        return std::nullopt;
    }

    bool VoxelRaycaster::isBlockSolid(const glm::ivec3 &pos) const {
        return m_world.getVoxel(pos.x, pos.y, pos.z) != 0;
    }

    glm::ivec3 VoxelRaycaster::calculateFaceNormal(const glm::ivec3 &blockPos,
                                                   const glm::ivec3 &previousPos) {
        glm::ivec3 diff = previousPos - blockPos;

        // La normale pointe vers la face d'où vient le ray
        if (diff.x != 0) return {diff.x, 0, 0};
        if (diff.y != 0) return {0, diff.y, 0};
        if (diff.z != 0) return {0, 0, diff.z};

        // Fallback (ne devrait pas arriver)
        return {1, 0, 0};
    }
}