#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    VoxelDefinition::VoxelDefinition(const std::string_view displayName,
                                     const ash::Vec4 &color,
                                     const RenderMode renderingMode,
                                     const bool hasCollision)
        : displayName(displayName),
          color(color),
          renderMode(renderingMode),
          hasCollision(hasCollision) {
    }

    VoxelTypeRegistry *VoxelTypeRegistry::instance = nullptr;

    VoxelTypeRegistry::VoxelTypeRegistry() {
        resetToDefaults();
    }

    VoxelTypeRegistry &VoxelTypeRegistry::getInstance() {
        if (instance == nullptr) {
            instance = new VoxelTypeRegistry();
        }
        return *instance;
    }

    const VoxelDefinition &VoxelTypeRegistry::getDefinition(const VoxelType voxelID) const noexcept {
        return registry[voxelID];
    }

    void VoxelTypeRegistry::setDefinition(const VoxelType voxelID, const VoxelDefinition &definition) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID] = definition;
        }
    }

    void VoxelTypeRegistry::setColor(const VoxelType voxelID, const ash::Vec4 &color) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].color = color;
        }
    }

    void VoxelTypeRegistry::setRenderingMode(const VoxelType voxelID, const RenderMode mode) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].renderMode = mode;
        }
    }

    void VoxelTypeRegistry::setCollision(const VoxelType voxelID, const bool hasCollision) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].hasCollision = hasCollision;
        }
    }

    void VoxelTypeRegistry::setDisplayName(const VoxelType voxelID, const std::string_view name) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].displayName = name;
        }
    }

    void VoxelTypeRegistry::setFriction(const VoxelType voxelID, const float friction) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].friction = friction;
        }
    }

    void VoxelTypeRegistry::setBounciness(const VoxelType voxelID, const float bounciness) noexcept {
        if (isValidVoxelID(voxelID)) {
            registry[voxelID].bounciness = bounciness;
        }
    }

    void VoxelTypeRegistry::resetToDefaults() noexcept {
        for (auto &def: registry) {
            def = VoxelDefinition{};
        }

        registry[VoxelID::AIR] = VoxelDefinition{
            "Air", ash::Vec4(0.00f, 0.00f, 0.00f, 1.00f), RenderMode::INVISIBLE, false
        };

        registry[VoxelID::DIRT] = VoxelDefinition{
            "Dirt", ash::Vec4(0.50f, 0.32f, 0.18f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::DIRT].friction = 0.7f;

        registry[VoxelID::GRASS] = VoxelDefinition{
            "Grass", ash::Vec4(0.49f, 0.67f, 0.09f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::GRASS].friction = 0.65f;

        registry[VoxelID::STONE] = VoxelDefinition{
            "Stone", ash::Vec4(0.50f, 0.50f, 0.50f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::STONE].friction = 0.8f;

        registry[VoxelID::SAND] = VoxelDefinition{
            "Sand", ash::Vec4(0.98f, 0.94f, 0.81f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::SAND].friction = 0.5f;

        registry[VoxelID::WATER] = VoxelDefinition{
            "Water", ash::Vec4(0.25f, 0.46f, 0.89f, 0.50f), RenderMode::TRANSPARENT, false
        };
        registry[VoxelID::WATER].friction = 0.1f;

        registry[VoxelID::LAVA] = VoxelDefinition{
            "Lava", ash::Vec4(0.81f, 0.29f, 0.06f, 0.50f), RenderMode::TRANSPARENT, false
        };
        registry[VoxelID::LAVA].friction = 0.2f;

        registry[VoxelID::GLASS] = VoxelDefinition{
            "Glass", ash::Vec4(1.00f, 1.00f, 1.00f, 0.25f), RenderMode::TRANSPARENT, true
        };

        registry[VoxelID::WOOD] = VoxelDefinition{
            "Wood Log", ash::Vec4(0.42f, 0.31f, 0.18f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::WOOD].friction = 0.6f;

        registry[VoxelID::LEAVES] = VoxelDefinition{
            "Leaves", ash::Vec4(0.30f, 0.61f, 0.14f, 0.63f), RenderMode::TRANSPARENT, true
        };

        registry[VoxelID::COBBLESTONE] = VoxelDefinition{
            "Cobblestone", ash::Vec4(0.48f, 0.48f, 0.48f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::COBBLESTONE].friction = 0.75f;

        registry[VoxelID::PLANKS] = VoxelDefinition{
            "Planks", ash::Vec4(0.61f, 0.46f, 0.33f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::BRICK] = VoxelDefinition{
            "Brick", ash::Vec4(0.60f, 0.35f, 0.22f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::BEDROCK] = VoxelDefinition{
            "Bedrock", ash::Vec4(0.29f, 0.29f, 0.29f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::GRAVEL] = VoxelDefinition{
            "Gravel", ash::Vec4(0.54f, 0.54f, 0.54f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::GRAVEL].friction = 0.55f;

        registry[VoxelID::ICE] = VoxelDefinition{
            "Ice", ash::Vec4(0.63f, 0.78f, 0.94f, 0.75f), RenderMode::TRANSPARENT, true
        };
        registry[VoxelID::ICE].friction = 0.98f;

        registry[VoxelID::SNOW] = VoxelDefinition{
            "Snow", ash::Vec4(1.00f, 1.00f, 1.00f, 1.00f), RenderMode::OPAQUE, true
        };
        registry[VoxelID::SNOW].friction = 0.9f;

        registry[VoxelID::CLAY] = VoxelDefinition{
            "Clay", ash::Vec4(0.63f, 0.63f, 0.72f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::OBSIDIAN] = VoxelDefinition{
            "Obsidian", ash::Vec4(0.08f, 0.08f, 0.12f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::COAL_ORE] = VoxelDefinition{
            "Coal Ore", ash::Vec4(0.44f, 0.44f, 0.44f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::IRON_ORE] = VoxelDefinition{
            "Iron Ore", ash::Vec4(0.85f, 0.66f, 0.56f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::GOLD_ORE] = VoxelDefinition{
            "Gold Ore", ash::Vec4(0.99f, 0.93f, 0.29f, 1.00f), RenderMode::OPAQUE, true
        };

        registry[VoxelID::DIAMOND_ORE] = VoxelDefinition{
            "Diamond Ore", ash::Vec4(0.36f, 0.93f, 0.96f, 1.00f), RenderMode::OPAQUE, true
        };
    }

    bool isValidVoxelID(const VoxelType voxelID) noexcept {
        return voxelID <= MAX_TYPE_ID;
    }

    const VoxelDefinition &getVoxelTypeDefinition(const VoxelType voxelID) noexcept {
        return VoxelTypeRegistry::getInstance().getDefinition(voxelID);
    }

    const std::string &getDisplayName(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).displayName;
    }

    ash::Vec4 getVoxelColor(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).color;
    }

    RenderMode getRenderMode(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).renderMode;
    }

    bool doesVoxelHaveCollision(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).hasCollision;
    }

    float getVoxelFriction(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).friction;
    }

    float getVoxelBounciness(const VoxelType voxelID) noexcept {
        return getVoxelTypeDefinition(voxelID).bounciness;
    }

    bool isVoxelTransparent(const VoxelType voxelID) noexcept {
        return getRenderMode(voxelID) == RenderMode::TRANSPARENT;
    }

    bool isVoxelOpaque(const VoxelType voxelID) noexcept {
        return getRenderMode(voxelID) == RenderMode::OPAQUE;
    }

    bool isVoxelAir(const VoxelType voxelID) noexcept {
        return voxelID == VoxelID::AIR;
    }

    bool isVoxelLiquid(const VoxelType voxelID) noexcept {
        return voxelID == VoxelID::WATER || voxelID == VoxelID::LAVA;
    }

    bool isVoxelSolid(const VoxelType voxelID) noexcept {
        return !isVoxelAir(voxelID) && !isVoxelLiquid(voxelID);
    }

    bool shouldRenderVoxelFace(const VoxelType currentVoxel, const VoxelType neighborVoxel) noexcept {
        if (currentVoxel == VoxelID::AIR) return false;
        if (neighborVoxel == VoxelID::AIR) return true;

        if (isVoxelOpaque(currentVoxel) && isVoxelOpaque(neighborVoxel)) {
            return false;
        }

        if (currentVoxel == neighborVoxel && isVoxelLiquid(currentVoxel)) {
            return false;
        }

        return true;
    }
}