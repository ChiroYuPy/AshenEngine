#ifndef VOXELITY_VOXEL_TYPE_H
#define VOXELITY_VOXEL_TYPE_H

#include <cstdint>
#include <string>
#include <array>

#include "Ashen/Math/Math.h"

namespace voxelity {
    using VoxelType = uint8_t;
    constexpr VoxelType MAX_TYPE_ID = std::numeric_limits<VoxelType>::max();

    enum class RenderMode : uint8_t {
        INVISIBLE,
        OPAQUE,
        TRANSPARENT,
    };

    struct VoxelDefinition {
        std::string displayName;
        ash::Vec4 color;
        RenderMode renderMode;
        bool hasCollision;
        float friction = 0.6f; // Coefficient de friction
        float bounciness = 0.0f; // Coefficient de rebond

        explicit VoxelDefinition(
            std::string_view displayName = "Unknown Block",
            const ash::Vec4 &color = ash::Vec4(255, 0, 255, 255),
            RenderMode renderingMode = RenderMode::OPAQUE,
            bool hasCollision = true);
    };

    namespace VoxelID {
        constexpr VoxelType AIR = 0;
        constexpr VoxelType DIRT = 1;
        constexpr VoxelType GRASS = 2;
        constexpr VoxelType STONE = 3;
        constexpr VoxelType SAND = 4;
        constexpr VoxelType WATER = 5;
        constexpr VoxelType LAVA = 6;
        constexpr VoxelType GLASS = 7;
        constexpr VoxelType WOOD = 8;
        constexpr VoxelType LEAVES = 9;
        constexpr VoxelType COBBLESTONE = 10;
        constexpr VoxelType PLANKS = 11;
        constexpr VoxelType BRICK = 12;
        constexpr VoxelType BEDROCK = 13;
        constexpr VoxelType GRAVEL = 14;
        constexpr VoxelType ICE = 15;
        constexpr VoxelType SNOW = 16;
        constexpr VoxelType CLAY = 17;
        constexpr VoxelType OBSIDIAN = 18;
        constexpr VoxelType COAL_ORE = 19;
        constexpr VoxelType IRON_ORE = 20;
        constexpr VoxelType GOLD_ORE = 21;
        constexpr VoxelType DIAMOND_ORE = 22;
    }

    class VoxelTypeRegistry {
    private:
        std::array<VoxelDefinition, 256> registry;
        static VoxelTypeRegistry *instance;

        VoxelTypeRegistry();

    public:
        static VoxelTypeRegistry &getInstance();

        [[nodiscard]] const VoxelDefinition &getDefinition(VoxelType voxelID) const noexcept;

        void setDefinition(VoxelType voxelID, const VoxelDefinition &definition) noexcept;

        void setColor(VoxelType voxelID, const ash::Vec4 &color) noexcept;

        void setRenderingMode(VoxelType voxelID, RenderMode mode) noexcept;

        void setCollision(VoxelType voxelID, bool hasCollision) noexcept;

        void setDisplayName(VoxelType voxelID, std::string_view name) noexcept;

        void setFriction(VoxelType voxelID, float friction) noexcept;

        void setBounciness(VoxelType voxelID, float bounciness) noexcept;

        void resetToDefaults() noexcept;
    };

    // Fonctions utilitaires
    [[nodiscard]] bool isValidVoxelID(VoxelType voxelID) noexcept;

    [[nodiscard]] const VoxelDefinition &getVoxelTypeDefinition(VoxelType voxelID) noexcept;

    [[nodiscard]] const std::string &getDisplayName(VoxelType voxelID) noexcept;

    [[nodiscard]] ash::Vec4 getVoxelColor(VoxelType voxelID) noexcept;

    [[nodiscard]] RenderMode getRenderMode(VoxelType voxelID) noexcept;

    [[nodiscard]] bool doesVoxelHaveCollision(VoxelType voxelID) noexcept;

    [[nodiscard]] float getVoxelFriction(VoxelType voxelID) noexcept;

    [[nodiscard]] float getVoxelBounciness(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelTransparent(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelOpaque(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelAir(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelLiquid(VoxelType voxelID) noexcept;

    [[nodiscard]] bool isVoxelSolid(VoxelType voxelID) noexcept;

    [[nodiscard]] bool shouldRenderVoxelFace(VoxelType currentVoxel, VoxelType neighborVoxel) noexcept;
}

#endif