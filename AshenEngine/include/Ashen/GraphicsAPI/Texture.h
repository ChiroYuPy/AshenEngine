#ifndef ASHEN_TEXTURE_H
#define ASHEN_TEXTURE_H

#include <array>
#include <stdexcept>
#include <string>
#include <optional>

#include <stb_image.h>
#include <glad/glad.h>

#include "Ashen/GraphicsAPI/GLEnums.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "Ashen/GraphicsAPI/GLObject.h"

namespace ash {
    struct TextureConfig {
        TextureWrap wrapS = TextureWrap::Repeat;
        TextureWrap wrapT = TextureWrap::Repeat;
        TextureWrap wrapR = TextureWrap::Repeat;
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        std::optional<Vec4> borderColor = std::nullopt;
        bool generateMipmaps = false;
        int maxAnisotropy = 1;

        static TextureConfig Default() { return {}; }

        static TextureConfig Pixelated() {
            TextureConfig config;
            config.minFilter = TextureFilter::Nearest;
            config.magFilter = TextureFilter::Nearest;
            return config;
        }

        static TextureConfig Smooth() {
            TextureConfig config;
            config.minFilter = TextureFilter::Linear;
            config.magFilter = TextureFilter::Linear;
            return config;
        }

        static TextureConfig Mipmapped() {
            TextureConfig config;
            config.minFilter = TextureFilter::LinearMipmapLinear;
            config.magFilter = TextureFilter::Linear;
            config.generateMipmaps = true;
            return config;
        }

        static TextureConfig Clamped() {
            TextureConfig config;
            config.wrapS = TextureWrap::ClampToEdge;
            config.wrapT = TextureWrap::ClampToEdge;
            config.wrapR = TextureWrap::ClampToEdge;
            return config;
        }
    };

    class Texture : public Bindable {
    public:
        explicit Texture(const TextureTarget target)
            : m_Target(target) {
            glGenTextures(1, &m_ID);
        }

        ~Texture() override {
            if (m_ID)
                glDeleteTextures(1, &m_ID);
        }

        Texture(const Texture &) = delete;

        Texture &operator=(const Texture &) = delete;

        Texture(Texture &&other) noexcept
            : m_Target(other.m_Target), m_Config(other.m_Config) {
            m_ID = other.m_ID;
            other.m_ID = 0;
        }

        Texture &operator=(Texture &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteTextures(1, &m_ID);
                m_ID = other.m_ID;
                m_Target = other.m_Target;
                m_Config = other.m_Config;
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindTexture(static_cast<GLenum>(m_Target), m_ID);
        }

        void Unbind() const override {
            glBindTexture(static_cast<GLenum>(m_Target), 0);
        }

        void BindToUnit(const uint32_t unit) const {
            glActiveTexture(GL_TEXTURE0 + unit);
            Bind();
        }

        [[nodiscard]] TextureTarget Target() const { return m_Target; }
        [[nodiscard]] const TextureConfig &GetConfig() const { return m_Config; }

        void SetWrap(TextureWrap s, TextureWrap t, TextureWrap r) {
            m_Config.wrapS = s;
            m_Config.wrapT = t;
            m_Config.wrapR = r;
            Bind();
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_T, static_cast<GLint>(t));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_R, static_cast<GLint>(r));
        }

        void SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
            m_Config.minFilter = minFilter;
            m_Config.magFilter = magFilter;
            Bind();
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
        }

        void SetBorderColor(const Vec4 &color) {
            m_Config.borderColor = color;
            Bind();
            glTexParameterfv(static_cast<GLenum>(m_Target), GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }

        void SetMaxAnisotropy(const int level) {
            m_Config.maxAnisotropy = level;
            Bind();
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MAX_ANISOTROPY, level);
        }

        void GenerateMipmap() {
            Bind();
            glGenerateMipmap(static_cast<GLenum>(m_Target));
            m_Config.generateMipmaps = true;
        }

        void ApplyConfig(const TextureConfig &config) {
            m_Config = config;
            Bind();

            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_S, static_cast<GLint>(config.wrapS));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_T, static_cast<GLint>(config.wrapT));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_WRAP_R, static_cast<GLint>(config.wrapR));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(config.minFilter));
            glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(config.magFilter));

            if (config.borderColor.has_value()) {
                glTexParameterfv(static_cast<GLenum>(m_Target), GL_TEXTURE_BORDER_COLOR,
                                 glm::value_ptr(config.borderColor.value()));
            }

            if (config.maxAnisotropy > 1) {
                glTexParameteri(static_cast<GLenum>(m_Target), GL_TEXTURE_MAX_ANISOTROPY, config.maxAnisotropy);
            }

            if (config.generateMipmaps) {
                glGenerateMipmap(static_cast<GLenum>(m_Target));
            }
        }

    protected:
        TextureTarget m_Target;
        TextureConfig m_Config;
    };

    class Texture1D final : public Texture {
    public:
        Texture1D() : Texture(TextureTarget::Texture1D) {
        }

        void SetData(TextureFormat internalFormat, const GLsizei width,
                     TextureFormat format, PixelDataType type,
                     const void *data, const GLint level = 0) {
            Bind();
            glTexImage1D(GL_TEXTURE_1D, level, static_cast<GLint>(internalFormat),
                         width, 0, static_cast<GLenum>(format), static_cast<GLenum>(type), data);

            if (m_Config.generateMipmaps && level == 0)
                GenerateMipmap();
        }

        void SetWrap(const TextureWrap s) {
            Texture::SetWrap(s, s, s);
        }
    };

    class Texture2D final : public Texture {
    public:
        Texture2D()
            : Texture(TextureTarget::Texture2D),
              m_Width(0),
              m_Height(0),
              m_InternalFormat(TextureFormat::RGBA8),
              m_Format(TextureFormat::RGBA),
              m_Type(PixelDataType::UnsignedByte) {
        }

        void SetData(TextureFormat internalFormat,
                     const GLsizei width, const GLsizei height,
                     TextureFormat format, PixelDataType type,
                     const void *data, const GLint level = 0) {
            Bind();
            glTexImage2D(GL_TEXTURE_2D, level, static_cast<GLint>(internalFormat),
                         width, height, 0, static_cast<GLenum>(format),
                         static_cast<GLenum>(type), data);

            m_Width = width;
            m_Height = height;
            m_InternalFormat = internalFormat;
            m_Format = format;
            m_Type = type;

            if (m_Config.generateMipmaps && level == 0)
                GenerateMipmap();
        }

        void SetWrap(const TextureWrap s, const TextureWrap t) {
            Texture::SetWrap(s, t, s);
        }

        [[nodiscard]] GLsizei GetWidth() const noexcept { return m_Width; }
        [[nodiscard]] GLsizei GetHeight() const noexcept { return m_Height; }

    private:
        GLsizei m_Width;
        GLsizei m_Height;
        TextureFormat m_InternalFormat;
        TextureFormat m_Format;
        PixelDataType m_Type;
    };

    class Texture3D final : public Texture {
    public:
        Texture3D() : Texture(TextureTarget::Texture3D) {
        }

        void SetData(TextureFormat internalFormat,
                     const GLsizei width, const GLsizei height, const GLsizei depth,
                     TextureFormat format, PixelDataType type,
                     const void *data, const GLint level = 0) {
            Bind();
            glTexImage3D(GL_TEXTURE_3D, level, static_cast<GLint>(internalFormat),
                         width, height, depth, 0, static_cast<GLenum>(format),
                         static_cast<GLenum>(type), data);

            if (m_Config.generateMipmaps && level == 0)
                GenerateMipmap();
        }
    };

    class TextureCubeMap final : public Texture {
    public:
        TextureCubeMap() : Texture(TextureTarget::CubeMap) {
        }

        void SetFace(CubeMapFace face, TextureFormat internalFormat,
                     const GLsizei width, const GLsizei height,
                     TextureFormat format, PixelDataType type,
                     const void *data, const GLint level = 0) const {
            Bind();
            glTexImage2D(static_cast<GLenum>(face), level, static_cast<GLint>(internalFormat),
                         width, height, 0, static_cast<GLenum>(format),
                         static_cast<GLenum>(type), data);
        }

        static Ref<TextureCubeMap> LoadFromFiles(
            const std::array<std::string, 6> &facesPaths,
            const TextureConfig &config = TextureConfig::Clamped()) {
            auto cubemap = std::make_shared<TextureCubeMap>();
            int width, height, channels;

            constexpr CubeMapFace faces[6] = {
                CubeMapFace::PositiveX, CubeMapFace::NegativeX,
                CubeMapFace::PositiveY, CubeMapFace::NegativeY,
                CubeMapFace::PositiveZ, CubeMapFace::NegativeZ
            };

            for (int i = 0; i < 6; ++i) {
                unsigned char *data = stbi_load(facesPaths[i].c_str(), &width, &height, &channels, 0);
                if (!data)
                    throw std::runtime_error("Failed to load cubemap face: " + facesPaths[i]);

                const TextureFormat format = (channels == 3) ? TextureFormat::RGB8 : TextureFormat::RGBA8;
                const TextureFormat dataFormat = (channels == 3) ? TextureFormat::RGB : TextureFormat::RGBA;

                cubemap->SetFace(faces[i], format, width, height, dataFormat,
                                 PixelDataType::UnsignedByte, data);

                stbi_image_free(data);
            }

            cubemap->ApplyConfig(config);
            return cubemap;
        }
    };
}

#endif //ASHEN_TEXTURE_H