#ifndef VOXELITY_TEXTURECOLORPALETTE_H
#define VOXELITY_TEXTURECOLORPALETTE_H

#include <array>

#include "Ashen/renderer/gfx/Texture.h"

#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    constexpr size_t MAX_COLORS = 256;

    class TextureAtlas {
    public:
        virtual ~TextureAtlas() = default;

        virtual void Update() = 0;

        [[nodiscard]] const pixl::Texture1D &GetTexture() const { return m_texture; }
        [[nodiscard]] size_t GetEntryCount() const { return m_entryCount; }

    protected:
        pixl::Texture1D m_texture;
        size_t m_entryCount = 0;

        TextureAtlas() {
            m_texture.SetWrap(GL_CLAMP_TO_EDGE);
            m_texture.SetFilter(GL_NEAREST, GL_NEAREST);
        }
    };

    class TextureColorPalette final : public TextureAtlas {
    public:
        TextureColorPalette() : m_colors() {
            m_entryCount = MAX_COLORS;
        }

        ~TextureColorPalette() override = default;

        void Update() override {
            updateFromRegistry();
        }

        void updateFromRegistry() {
            for (size_t i = 0; i < MAX_COLORS; ++i) {
                const auto &c = getVoxelColor(static_cast<VoxelType>(i));
                m_colors[i] = c.toVec4();
            }

            m_texture.SetData(0, GL_RGBA32F, MAX_COLORS, GL_RGBA, GL_FLOAT, m_colors.data());
        }

        // Accès direct aux couleurs si nécessaire
        [[nodiscard]] const glm::vec4 &GetColor(const size_t index) const {
            return m_colors[index];
        }

        void SetColor(const size_t index, const glm::vec4 &color) {
            if (index < MAX_COLORS)
                m_colors[index] = color;
        }

    private:
        std::array<glm::vec4, MAX_COLORS> m_colors;
    };
}

#endif //VOXELITY_TEXTURECOLORPALETTE_H
