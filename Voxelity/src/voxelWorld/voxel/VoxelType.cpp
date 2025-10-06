#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    VoxelDefinition::VoxelDefinition(const std::string_view displayName,
                                     const ash::Color &color,
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

    void VoxelTypeRegistry::setColor(const VoxelType voxelID, const ash::Color &color) noexcept {
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
            "Air", ash::Color::fromHex("#000000"), RenderMode::INVISIBLE, false
        };

        registry[VoxelID::DIRT] = VoxelDefinition{
            "Dirt", ash::Color::fromHex("#80522F"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::DIRT].friction = 0.7f;

        registry[VoxelID::GRASS] = VoxelDefinition{
            "Grass", ash::Color::fromHex("#7CAC17"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::GRASS].friction = 0.65f;

        registry[VoxelID::STONE] = VoxelDefinition{
            "Stone", ash::Color::fromHex("#7F7F7F"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::STONE].friction = 0.8f;

        registry[VoxelID::SAND] = VoxelDefinition{
            "Sand", ash::Color::fromHex("#FAF0CF"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::SAND].friction = 0.5f;

        registry[VoxelID::WATER] = VoxelDefinition{
            "Water", ash::Color::fromHex("#3F76E480"), RenderMode::TRANSPARENT, false
        };
        registry[VoxelID::WATER].friction = 0.1f;

        registry[VoxelID::LAVA] = VoxelDefinition{
            "Lava", ash::Color::fromHex("#CF4A0F80"), RenderMode::TRANSPARENT, false
        };
        registry[VoxelID::LAVA].friction = 0.2f;

        registry[VoxelID::GLASS] = VoxelDefinition{
            "Glass", ash::Color::fromHex("#FFFFFF40"), RenderMode::TRANSPARENT, true
        };

        registry[VoxelID::WOOD] = VoxelDefinition{
            "Wood Log", ash::Color::fromHex("#6B4F2F"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::WOOD].friction = 0.6f;

        registry[VoxelID::LEAVES] = VoxelDefinition{
            "Leaves", ash::Color::fromHex("#4C9B23A0"), RenderMode::TRANSPARENT, true
        };

        registry[VoxelID::COBBLESTONE] = VoxelDefinition{
            "Cobblestone", ash::Color::fromHex("#7A7A7A"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::COBBLESTONE].friction = 0.75f;

        registry[VoxelID::PLANKS] = VoxelDefinition{
            "Planks", ash::Color::fromHex("#9B7653"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::BRICK] = VoxelDefinition{
            "Brick", ash::Color::fromHex("#9A5839"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::BEDROCK] = VoxelDefinition{
            "Bedrock", ash::Color::fromHex("#4A4A4A"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::GRAVEL] = VoxelDefinition{
            "Gravel", ash::Color::fromHex("#8A8A8A"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::GRAVEL].friction = 0.55f;

        registry[VoxelID::ICE] = VoxelDefinition{
            "Ice", ash::Color::fromHex("#A0C8F0C0"), RenderMode::TRANSPARENT, true
        };
        registry[VoxelID::ICE].friction = 0.98f; // Très glissant

        registry[VoxelID::SNOW] = VoxelDefinition{
            "Snow", ash::Color::fromHex("#FFFFFF"), RenderMode::OPAQUE, true
        };
        registry[VoxelID::SNOW].friction = 0.9f;

        registry[VoxelID::CLAY] = VoxelDefinition{
            "Clay", ash::Color::fromHex("#A0A0B8"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::OBSIDIAN] = VoxelDefinition{
            "Obsidian", ash::Color::fromHex("#14141E"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::COAL_ORE] = VoxelDefinition{
            "Coal Ore", ash::Color::fromHex("#6F6F6F"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::IRON_ORE] = VoxelDefinition{
            "Iron Ore", ash::Color::fromHex("#D8A88E"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::GOLD_ORE] = VoxelDefinition{
            "Gold Ore", ash::Color::fromHex("#FCEE4B"), RenderMode::OPAQUE, true
        };

        registry[VoxelID::DIAMOND_ORE] = VoxelDefinition{
            "Diamond Ore", ash::Color::fromHex("#5DECF5"), RenderMode::OPAQUE, true
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

    ash::Color getVoxelColor(const VoxelType voxelID) noexcept {
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