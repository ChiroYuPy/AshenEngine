#ifndef VOXELITY_TEXTUREATLASSYSTEM_H
#define VOXELITY_TEXTUREATLASSYSTEM_H

#include <ranges>

#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Core/Types.h"
#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace ash {
    class ITextureAtlas {
    public:
        virtual ~ITextureAtlas() = default;

        virtual void Update() = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;

        [[nodiscard]] virtual size_t GetEntryCount() const = 0;

        [[nodiscard]] virtual uint32_t GetTextureID() const = 0;
    };

    class ColorPalette1D final : public ITextureAtlas {
    public:
        static constexpr size_t MAX_COLORS = 256;

        ColorPalette1D() {
            m_texture.SetWrap(TextureWrap::ClampToEdge);
            m_texture.SetFilter(TextureFilter::Nearest, TextureFilter::Nearest);
            m_colors.fill(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
        }

        void Update() override {
            UpdateFromRegistry();
        }

        void UpdateFromRegistry() {
            for (size_t i = 0; i < MAX_COLORS; ++i) {
                m_colors[i] = voxelity::getVoxelColor(static_cast<voxelity::VoxelType>(i));
            }

            m_texture.SetData(
                TextureFormat::RGBA32F,
                MAX_COLORS,
                TextureFormat::RGBA,
                PixelDataType::Float,
                m_colors.data()
            );
        }

        void Bind(const uint32_t slot = 0) const override {
            glBindTextureUnit(slot, m_texture.ID());
        }

        [[nodiscard]] size_t GetEntryCount() const override { return MAX_COLORS; }
        [[nodiscard]] uint32_t GetTextureID() const override { return m_texture.ID(); }

        [[nodiscard]] const glm::vec4 &GetColor(const size_t index) const {
            return m_colors[index];
        }

        void SetColor(const size_t index, const glm::vec4 &color) {
            if (index < MAX_COLORS)
                m_colors[index] = color;
        }

    private:
        Texture1D m_texture;
        std::array<glm::vec4, MAX_COLORS> m_colors{};
    };

    struct AtlasEntry {
        uint32_t index;
        glm::vec2 uvMin;
        glm::vec2 uvMax;
        glm::ivec2 pixelSize;
    };

    class TextureAtlas2D final : public ITextureAtlas {
    public:
        struct Config {
            uint32_t tileSize = 16;
            uint32_t atlasWidth = 256;
            uint32_t atlasHeight = 256;
            TextureFilter minFilter = TextureFilter::Nearest;
            TextureFilter magFilter = TextureFilter::Nearest;
            bool generateMipmaps = false;
        };

        explicit TextureAtlas2D(const Config &config = Config{
            16, 256, 256, TextureFilter::Nearest, TextureFilter::Nearest, false
        })
            : m_config(config) {
            m_tilesPerRow = m_config.atlasWidth / m_config.tileSize;
            m_tilesPerCol = m_config.atlasHeight / m_config.tileSize;
            m_maxTiles = m_tilesPerRow * m_tilesPerCol;

            m_texture.SetFilter(config.minFilter, config.magFilter);
            m_texture.SetWrap(TextureWrap::ClampToEdge, TextureWrap::ClampToEdge);

            m_pixels.resize(m_config.atlasWidth * m_config.atlasHeight * 4, 0);

            m_texture.SetData(TextureFormat::RGBA8, static_cast<GLsizei>(m_config.atlasWidth),
                              static_cast<GLsizei>(m_config.atlasHeight),
                              TextureFormat::RGBA, PixelDataType::UnsignedByte, m_pixels.data()
            );
        }

        bool AddTexture(const std::string &name, const std::string &filepath) {
            int width, height, channels;
            stbi_set_flip_vertically_on_load(false);
            unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

            if (!data) return false;

            const bool success = AddTextureData(name, data, width, height);
            stbi_image_free(data);
            return success;
        }

        bool AddTextureData(const std::string &name, const unsigned char *data, const int width, const int height) {
            if (m_entries.size() >= m_maxTiles) return false;
            if (width > m_config.tileSize || height > m_config.tileSize) return false;

            const auto index = static_cast<uint32_t>(m_entries.size());
            const uint32_t tileX = index % m_tilesPerRow;
            const uint32_t tileY = index / m_tilesPerRow;

            const uint32_t pixelX = tileX * m_config.tileSize;
            const uint32_t pixelY = tileY * m_config.tileSize;

            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x) {
                    const uint32_t atlasX = pixelX + x;
                    const uint32_t atlasY = pixelY + y;
                    const uint32_t atlasIdx = (atlasY * m_config.atlasWidth + atlasX) * 4;
                    const uint32_t srcIdx = (y * width + x) * 4;

                    m_pixels[atlasIdx + 0] = data[srcIdx + 0];
                    m_pixels[atlasIdx + 1] = data[srcIdx + 1];
                    m_pixels[atlasIdx + 2] = data[srcIdx + 2];
                    m_pixels[atlasIdx + 3] = data[srcIdx + 3];
                }

            const float uvMinX = static_cast<float>(pixelX) / static_cast<float>(m_config.atlasWidth);
            const float uvMinY = static_cast<float>(pixelY) / static_cast<float>(m_config.atlasHeight);
            const float uvMaxX = static_cast<float>(pixelX + width) / static_cast<float>(m_config.atlasWidth);
            const float uvMaxY = static_cast<float>(pixelY + height) / static_cast<float>(m_config.atlasHeight);

            const AtlasEntry entry{
                index,
                glm::vec2(uvMinX, uvMinY),
                glm::vec2(uvMaxX, uvMaxY),
                glm::ivec2(width, height)
            };

            m_entries[name] = entry;
            m_indexToName[index] = name;

            return true;
        }

        bool AddSolidColor(const std::string &name, const glm::vec4 &color) {
            Vector<unsigned char> data(m_config.tileSize * m_config.tileSize * 4);

            for (size_t i = 0; i < data.size(); i += 4) {
                data[i + 0] = static_cast<unsigned char>(color.r * 255);
                data[i + 1] = static_cast<unsigned char>(color.g * 255);
                data[i + 2] = static_cast<unsigned char>(color.b * 255);
                data[i + 3] = static_cast<unsigned char>(color.a * 255);
            }

            return AddTextureData(name, data.data(), m_config.tileSize, m_config.tileSize);
        }

        void Update() override {
            m_texture.SetData(TextureFormat::RGBA8, m_config.atlasWidth, m_config.atlasHeight,
                              TextureFormat::RGBA, PixelDataType::UnsignedByte, m_pixels.data()
            );

            if (m_config.generateMipmaps)
                m_texture.GenerateMipmap();
        }

        void Bind(const uint32_t slot = 0) const override {
            glBindTextureUnit(slot, m_texture.ID());
        }

        [[nodiscard]] const AtlasEntry *GetEntry(const std::string &name) const {
            const auto it = m_entries.find(name);
            return it != m_entries.end() ? &it->second : nullptr;
        }

        [[nodiscard]] const AtlasEntry *GetEntry(const uint32_t index) const {
            const auto it = m_indexToName.find(index);
            if (it != m_indexToName.end())
                return GetEntry(it->second);

            return nullptr;
        }

        [[nodiscard]] size_t GetEntryCount() const override { return m_entries.size(); }
        [[nodiscard]] uint32_t GetTextureID() const override { return m_texture.ID(); }
        [[nodiscard]] const Config &GetConfig() const { return m_config; }

    private:
        Config m_config;
        Texture2D m_texture;
        Vector<unsigned char> m_pixels;

        uint32_t m_tilesPerRow;
        uint32_t m_tilesPerCol;
        uint32_t m_maxTiles;

        std::unordered_map<std::string, AtlasEntry> m_entries;
        std::unordered_map<uint32_t, std::string> m_indexToName;
    };

    class TextureArray final : public ITextureAtlas {
    public:
        struct Config {
            uint32_t layerWidth = 16;
            uint32_t layerHeight = 16;
            uint32_t maxLayers = 256;
            TextureFormat format = TextureFormat::RGBA8;
            TextureFilter minFilter = TextureFilter::Nearest;
            TextureFilter magFilter = TextureFilter::Nearest;
            bool generateMipmaps = false;
        };

        explicit TextureArray(const Config &config = Config{
            16, 16, 256, TextureFormat::RGBA8, TextureFilter::Nearest, TextureFilter::Nearest, false
        })
            : m_config(config), m_currentLayer(0) {
            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

            glTexImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                static_cast<GLint>(config.format),
                config.layerWidth,
                config.layerHeight,
                config.maxLayers,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr
            );

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(config.minFilter));
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(config.magFilter));
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        ~TextureArray() override {
            if (m_textureID)
                glDeleteTextures(1, &m_textureID);
        }

        bool AddLayer(const std::string &name, const std::string &filepath) {
            if (m_currentLayer >= m_config.maxLayers) return false;

            int width, height, channels;
            stbi_set_flip_vertically_on_load(false);
            unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

            if (!data) return false;

            const bool success = AddLayerData(name, data, width, height);
            stbi_image_free(data);
            return success;
        }

        bool AddLayerData(const std::string &name, const unsigned char *data, const int width, const int height) {
            if (m_currentLayer >= m_config.maxLayers) return false;
            if (width != m_config.layerWidth || height != m_config.layerHeight) return false;

            glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                0, 0, m_currentLayer,
                width, height, 1,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            m_nameToLayer[name] = m_currentLayer;
            m_layerToName[m_currentLayer] = name;
            m_currentLayer++;

            return true;
        }

        bool AddSolidColor(const std::string &name, const glm::vec4 &color) {
            Vector<unsigned char> data(m_config.layerWidth * m_config.layerHeight * 4);

            for (size_t i = 0; i < data.size(); i += 4) {
                data[i + 0] = static_cast<unsigned char>(color.r * 255);
                data[i + 1] = static_cast<unsigned char>(color.g * 255);
                data[i + 2] = static_cast<unsigned char>(color.b * 255);
                data[i + 3] = static_cast<unsigned char>(color.a * 255);
            }

            return AddLayerData(name, data.data(), m_config.layerWidth, m_config.layerHeight);
        }

        void Update() override {
            if (m_config.generateMipmaps) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
                glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            }
        }

        void Bind(const uint32_t slot = 0) const override {
            glBindTextureUnit(slot, m_textureID);
        }

        [[nodiscard]] int GetLayerIndex(const std::string &name) const {
            const auto it = m_nameToLayer.find(name);
            return it != m_nameToLayer.end() ? it->second : -1;
        }

        [[nodiscard]] size_t GetEntryCount() const override { return m_currentLayer; }
        [[nodiscard]] uint32_t GetTextureID() const override { return m_textureID; }
        [[nodiscard]] const Config &GetConfig() const { return m_config; }

    private:
        Config m_config;
        GLuint m_textureID = 0;
        uint32_t m_currentLayer;

        std::unordered_map<std::string, uint32_t> m_nameToLayer;
        std::unordered_map<uint32_t, std::string> m_layerToName;
    };

    class TextureAtlasManager {
    public:
        template<typename T, typename... Args>
        T *CreateAtlas(const std::string &name, Args &&... args) {
            static_assert(std::is_base_of_v<ITextureAtlas, T>,
                          "T must inherit from ITextureAtlas");

            auto atlas = std::make_unique<T>(std::forward<Args>(args)...);
            T *ptr = atlas.get();
            m_atlases[name] = std::move(atlas);
            return ptr;
        }

        ITextureAtlas *GetAtlas(const std::string &name) {
            const auto it = m_atlases.find(name);
            return it != m_atlases.end() ? it->second.get() : nullptr;
        }

        template<typename T>
        T *GetAtlasAs(const std::string &name) {
            return dynamic_cast<T *>(GetAtlas(name));
        }

        void UpdateAll() {
            for (const auto &atlas: m_atlases | std::views::values)
                atlas->Update();
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<ITextureAtlas> > m_atlases;
    };
}

#endif // VOXELITY_TEXTUREATLASSYSTEM_H