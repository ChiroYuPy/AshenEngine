#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    // Implémentation du constructeur
    VoxelDefinition::VoxelDefinition(const std::string_view displayName,
                                     const ash::Color &color,
                                     const RenderMode renderingMode,
                                     const bool hasCollision)
        : displayName(displayName),
          color(color),
          renderMode(renderingMode),
          hasCollision(hasCollision) {
    }

    // Implémentation du singleton
    VoxelTypeRegistry *VoxelTypeRegistry::instance = nullptr;

    VoxelTypeRegistry::VoxelTypeRegistry() {
        // Initialisation des valeurs par défaut
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

    void VoxelTypeRegistry::resetToDefaults() noexcept {
        // Réinitialiser tous les types à "Unknown Block" par défaut
        for (auto &def: registry) {
            def = VoxelDefinition{};
        }

        // Définir les types de base
        registry[VoxelID::AIR] = VoxelDefinition{
            "Air",
            ash::Color::fromHex("#000000"),
            RenderMode::INVISIBLE,
            false
        };

        registry[VoxelID::DIRT] = VoxelDefinition{
            "Dirt Block",
            ash::Color::fromHex("#80522F"),
            RenderMode::OPAQUE,
            true
        };

        registry[VoxelID::GRASS] = VoxelDefinition{
            "Grass Block",
            ash::Color::fromHex("#7CAC17"),
            RenderMode::OPAQUE,
            true
        };

        registry[VoxelID::STONE] = VoxelDefinition{
            "Stone Block",
            ash::Color::fromHex("#7F7F7F"),
            RenderMode::OPAQUE,
            true
        };

        registry[VoxelID::SAND] = VoxelDefinition{
            "Sand Block",
            ash::Color::fromHex("#FAF0CF"),
            RenderMode::OPAQUE,
            true
        };

        registry[VoxelID::WATER] = VoxelDefinition{
            "Water",
            ash::Color::fromHex("#3F76E480"),
            RenderMode::TRANSPARENT,
            false
        };

        registry[VoxelID::LAVA] = VoxelDefinition{
            "Lava",
            ash::Color::fromHex("#CF4A0F80"),
            RenderMode::TRANSPARENT,
            false
        };

        registry[VoxelID::GLASS] = VoxelDefinition{
            "Glass Block",
            ash::Color::fromHex("#FFFFFF40"),
            RenderMode::TRANSPARENT,
            true
        };

        registry[VoxelID::WOOD] = VoxelDefinition{
            "Wood Log",
            ash::Color::fromHex("#6B4F2F"),
            RenderMode::OPAQUE,
            true
        };

        registry[VoxelID::LEAVES] = VoxelDefinition{
            "Leaves",
            ash::Color::fromHex("#4C9B23A0"),
            RenderMode::TRANSPARENT,
            true
        };
    }

    // Fonctions utilitaires (inchangées)
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

    bool isVoxelTransparent(const VoxelType voxelID) noexcept {
        const RenderMode mode = getRenderMode(voxelID);
        return mode == RenderMode::TRANSPARENT;
    }

    bool isVoxelOpaque(const VoxelType voxelID) noexcept {
        const RenderMode mode = getRenderMode(voxelID);
        return mode == RenderMode::OPAQUE;
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

        // Si les deux voxels sont opaques, pas besoin de rendre la face
        if (isVoxelOpaque(currentVoxel) && isVoxelOpaque(neighborVoxel)) {
            return false;
        }

        // Cas spéciaux pour l'eau et autres liquides
        if (currentVoxel == neighborVoxel && isVoxelLiquid(currentVoxel)) {
            return false; // Pas de faces entre blocs identiques liquides
        }

        return true;
    }
}