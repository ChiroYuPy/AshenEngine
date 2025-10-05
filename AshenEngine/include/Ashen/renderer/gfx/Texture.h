#ifndef ASHEN_TEXTURE_H
#define ASHEN_TEXTURE_H

#include <array>
#include <stdexcept>
#include <string>

#include "Ashen/renderer/Bindable.h"
#include "stb_image.h"
#include "glad/glad.h"

namespace ash {
    class Texture : public Bindable {
    public:
        explicit Texture(const GLenum target)
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
            : m_ID(other.m_ID), m_Target(other.m_Target) {
            other.m_ID = 0;
        }

        Texture &operator=(Texture &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteTextures(1, &m_ID);
                m_ID = other.m_ID;
                m_Target = other.m_Target;
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindTexture(m_Target, m_ID);
        }

        void Unbind() const override {
            glBindTexture(m_Target, 0);
        }

        [[nodiscard]] GLuint ID() const { return m_ID; }
        [[nodiscard]] GLenum Target() const { return m_Target; }

    protected:
        GLuint m_ID = 0;
        GLenum m_Target;
    };

    class Texture1D final : public Texture {
    public:
        Texture1D() : Texture(GL_TEXTURE_1D) {
        }

        void SetData(const GLint level, const GLint internalFormat,
                     const GLsizei width,
                     const GLenum format, const GLenum type,
                     const void *data) const {
            Bind();
            glTexImage1D(GL_TEXTURE_1D, level, internalFormat,
                         width, 0, format, type, data);
        }

        void SetWrap(const GLenum s) const {
            Bind();
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, s);
        }

        void SetFilter(const GLenum minFilter, const GLenum magFilter) const {
            Bind();
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);
        }
    };

    class Texture2D final : public Texture {
    public:
        Texture2D() : Texture(GL_TEXTURE_2D) {
        }

        void SetData(const GLint level, const GLint internalFormat,
                     const GLsizei width, const GLsizei height,
                     const GLenum format, const GLenum type,
                     const void *data) const {
            Bind();
            glTexImage2D(GL_TEXTURE_2D, level, internalFormat,
                         width, height, 0, format, type, data);
        }

        void SetWrap(const GLenum s, const GLenum t) const {
            Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
        }

        void SetFilter(const GLenum minFilter, const GLenum magFilter) const {
            Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        }
    };

    class Texture3D final : public Texture {
    public:
        Texture3D() : Texture(GL_TEXTURE_3D) {
        }

        void SetData(const GLint level, const GLint internalFormat,
                     const GLsizei width, const GLsizei height, const GLsizei depth,
                     const GLenum format, const GLenum type,
                     const void *data) const {
            Bind();
            glTexImage3D(GL_TEXTURE_3D, level, internalFormat,
                         width, height, depth, 0, format, type, data);
        }

        void SetWrap(const GLenum s, const GLenum t, const GLenum r) const {
            Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, s);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, t);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, r);
        }

        void SetFilter(const GLenum minFilter, const GLenum magFilter) const {
            Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter);
        }
    };

    class TextureCubeMap final : public Texture {
    public:
        TextureCubeMap() : Texture(GL_TEXTURE_CUBE_MAP) {
        }

        void SetFace(const GLenum faceTarget, const GLint level, const GLint internalFormat,
                     const GLsizei width, const GLsizei height,
                     const GLenum format, const GLenum type,
                     const void *data) const {
            Bind();
            glTexImage2D(faceTarget, level, internalFormat,
                         width, height, 0, format, type, data);
        }

        void SetWrap(const GLenum wrap) const {
            Bind();
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);
        }

        void SetFilter(const GLenum minFilter, const GLenum magFilter) const {
            Bind();
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
        }
    };

    inline TextureCubeMap LoadCubeMap(const std::array<std::string, 6> &facesPaths) {
        TextureCubeMap cubemap;
        int width, height, channels;

        static const GLenum faceTargets[6] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };

        for (int i = 0; i < 6; ++i) {
            unsigned char *data = stbi_load(facesPaths[i].c_str(), &width, &height, &channels, 0);
            if (!data) throw std::runtime_error("Failed to load cubemap face: " + facesPaths[i]);

            const GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;

            cubemap.SetFace(faceTargets[i], 0, format, width, height, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        cubemap.SetWrap(GL_CLAMP_TO_EDGE);
        cubemap.SetFilter(GL_LINEAR, GL_LINEAR);

        return cubemap;
    }
}

#endif //ASHEN_TEXTURE_H