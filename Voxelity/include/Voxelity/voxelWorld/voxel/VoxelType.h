#ifndef VOXELITY_VOXEL_TYPE_H
#define VOXELITY_VOXEL_TYPE_H

#include <cstdint>
#include <string>
#include <array>

#include "Ashen/core/Color.h"

namespace voxelity {
    using VoxelType = uint8_t; // 0 = air, max 255 types

    constexpr VoxelType MAX_TYPE_ID = std::numeric_limits<VoxelType>::max();

    enum class RenderMode : uint8_t {
        INVISIBLE, // Air - not rendered at all
        OPAQUE, // Standard opaque block
        TRANSPARENT, // Glass-like blocks with transparency
    };

    struct VoxelDefinition {
        std::string displayName;
        pixl::Color color;
        RenderMode renderMode;
        bool hasCollision;

        explicit VoxelDefinition(
            std::string_view displayName = "Unknown Block",
            const pixl::Color &color = pixl::Color(255, 0, 255, 255),
            RenderMode renderingMode = RenderMode::OPAQUE,
            bool hasCollision = true);
    };

    namespace VoxelID {
        constexpr VoxelType AIR   = 0;
        constexpr VoxelType DIRT  = 1;
        constexpr VoxelType GRASS = 2;
        constexpr VoxelType STONE = 3;
        constexpr VoxelType SAND  = 4;
        constexpr VoxelType WATER = 5;
        constexpr VoxelType LAVA  = 6;
        constexpr VoxelType GLASS = 7;
        constexpr VoxelType WOOD  = 8;
        constexpr VoxelType LEAVES = 9;
    }

    // Classe singleton pour gérer le registre des voxels
    class VoxelTypeRegistry {
    private:
        std::array<VoxelDefinition, 256> registry;
        static VoxelTypeRegistry *instance;

        VoxelTypeRegistry();

    public:
        static VoxelTypeRegistry &getInstance();

        // Accesseurs en lecture seule
        [[nodiscard]] const VoxelDefinition &getDefinition(VoxelType voxelID) const noexcept;

        // Méthodes pour modifier les définitions
        void setDefinition(VoxelType voxelID, const VoxelDefinition &definition) noexcept;

        void setColor(VoxelType voxelID, const pixl::Color &color) noexcept;

        void setRenderingMode(VoxelType voxelID, RenderMode mode) noexcept;

        void setCollision(VoxelType voxelID, bool hasCollision) noexcept;

        void setDisplayName(VoxelType voxelID, std::string_view name) noexcept;

        // Méthode pour réinitialiser aux valeurs par défaut
        void resetToDefaults() noexcept;
    };

    // Fonctions utilitaires (inchangées mais utilisent maintenant le singleton)
    [[nodiscard]] bool isValidVoxelID(VoxelType voxelID) noexcept;

    [[nodiscard]] const VoxelDefinition &getVoxelTypeDefinition(VoxelType voxelID) noexcept;

    [[nodiscard]] const std::string &getDisplayName(VoxelType voxelID) noexcept;

    [[nodiscard]] pixl::Color getVoxelColor(VoxelType voxelID) noexcept;

    [[nodiscard]] RenderMode getRenderMode(VoxelType voxelID) noexcept;

    [[nodiscard]] bool doesVoxelHaveCollision(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelTransparent(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelOpaque(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelAir(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelLiquid(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelSolid(VoxelType voxelID) noexcept;

    [[nodiscard]] bool shouldRenderVoxelFace(VoxelType currentVoxel, VoxelType neighborVoxel) noexcept;

    // Fonctions de convenance pour modifier les types de voxels
    void modifyVoxelColor(VoxelType voxelID, const pixl::Color &newColor) noexcept;

    void modifyVoxelRenderingMode(VoxelType voxelID, RenderMode newMode) noexcept;

    void modifyVoxelLightLevel(VoxelType voxelID, float newLevel) noexcept;

    void modifyVoxelCollision(VoxelType voxelID, bool hasCollision) noexcept;

    void modifyVoxelName(VoxelType voxelID, std::string_view newName) noexcept;
}

#endif //VOXELITY_VOXEL_TYPE_H
