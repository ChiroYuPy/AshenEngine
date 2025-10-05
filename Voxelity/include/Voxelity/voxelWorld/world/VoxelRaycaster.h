#ifndef VOXELITY_VOXELRAYCASTER_H
#define VOXELITY_VOXELRAYCASTER_H

#include <optional>
#include <glm/glm.hpp>

namespace voxelity {
    class World;

    struct RaycastHit {
        glm::ivec3 blockPos; // Position du bloc touché
        glm::ivec3 normalFace; // Normale de la face touchée (-1, 0, 1 pour chaque axe)
        glm::vec3 hitPoint; // Point exact de collision
        float distance; // Distance depuis l'origine du ray
    };

    class VoxelRaycaster {
    public:
        explicit VoxelRaycaster(World &world);

        // Effectue un raycast depuis une position dans une direction
        [[nodiscard]] std::optional<RaycastHit> raycast(const glm::vec3 &origin, const glm::vec3 &direction) const;

        // Raycast depuis la caméra
        [[nodiscard]] std::optional<RaycastHit> raycastFromCamera(const glm::vec3 &cameraPos,
                                                                  const glm::vec3 &cameraDirection) const;

        void setMaxDistance(const float distance) { m_maxDistance = distance; }
        [[nodiscard]] float getMaxDistance() const { return m_maxDistance; }

    private:
        World &m_world;
        float m_maxDistance;

        // Algorithme DDA (Digital Differential Analyzer) pour traverser la grille voxel
        [[nodiscard]] std::optional<RaycastHit> performDDA(const glm::vec3 &origin, const glm::vec3 &direction) const;

        // Vérifie si un bloc existe à la position donnée
        [[nodiscard]] bool isBlockSolid(const glm::ivec3 &pos) const;

        // Calcule la normale de la face touchée
        static glm::ivec3 calculateFaceNormal(const glm::ivec3 &blockPos, const glm::ivec3 &previousPos);
    };
}

#endif //VOXELITY_VOXELRAYCASTER_H