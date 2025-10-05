#ifndef ASHEN_FRAMEBUFFER_H
#define ASHEN_FRAMEBUFFER_H

#include <stdexcept>

#include "Texture.h"
#include "glad/glad.h"
#include "Pixl/core/Types.h"
#include "Pixl/renderer/Bindable.h"
#include "Pixl/renderer/RenderCommand.h"

namespace pixl {
    class Framebuffer final : public Bindable {
    public:
        Framebuffer(const uint32_t width, const uint32_t height)
            : m_ColorTexture(MakeScope<Texture2D>()), m_Width(width),
              m_Height(height) {
            if (width == 0 || height == 0)
                throw std::invalid_argument("Framebuffer dimensions must be > 0");

            glGenFramebuffers(1, &m_RendererID);
            glGenRenderbuffers(1, &m_DepthAttachment);

            CreateAttachments();
        }

        ~Framebuffer() override {
            if (m_RendererID)
                glDeleteFramebuffers(1, &m_RendererID);
            if (m_DepthAttachment)
                glDeleteRenderbuffers(1, &m_DepthAttachment);
        }

        // Suppression de la copie
        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        // Support du move semantics
        Framebuffer(Framebuffer &&other) noexcept
            : m_RendererID(other.m_RendererID),
              m_ColorTexture(std::move(other.m_ColorTexture)),
              m_DepthAttachment(other.m_DepthAttachment),
              m_Width(other.m_Width),
              m_Height(other.m_Height) {
            other.m_RendererID = 0;
            other.m_DepthAttachment = 0;
        }

        Framebuffer &operator=(Framebuffer &&other) noexcept {
            if (this != &other) {
                if (m_RendererID)
                    glDeleteFramebuffers(1, &m_RendererID);
                if (m_DepthAttachment)
                    glDeleteRenderbuffers(1, &m_DepthAttachment);

                m_RendererID = other.m_RendererID;
                m_ColorTexture = std::move(other.m_ColorTexture);
                m_DepthAttachment = other.m_DepthAttachment;
                m_Width = other.m_Width;
                m_Height = other.m_Height;

                other.m_RendererID = 0;
                other.m_DepthAttachment = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
            RenderCommand::SetViewport(m_Width, m_Height);
        }

        void Unbind() const override {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Resize(const uint32_t width, const uint32_t height) {
            if (width == 0 || height == 0)
                throw std::invalid_argument("Framebuffer dimensions must be > 0");

            m_Width = width;
            m_Height = height;
            CreateAttachments();
        }

        [[nodiscard]] GLuint GetColorTextureID() const {
            return m_ColorTexture->ID();
        }

        [[nodiscard]] const Texture2D &GetColorTexture() const {
            return *m_ColorTexture;
        }

        [[nodiscard]] GLuint GetDepthAttachmentID() const {
            return m_DepthAttachment;
        }

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] GLuint GetID() const { return m_RendererID; }

        [[nodiscard]] bool IsComplete() const {
            Bind();
            const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

    private:
        void CreateAttachments() const {
            Bind();

            // Color attachment - utilise Texture2D
            m_ColorTexture->SetData(0, GL_RGBA8, static_cast<int>(m_Width), static_cast<int>(m_Height),
                                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            m_ColorTexture->SetFilter(GL_LINEAR, GL_LINEAR);
            m_ColorTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, m_ColorTexture->ID(), 0);

            // Depth attachment (renderbuffer)
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                  static_cast<int>(m_Width), static_cast<int>(m_Height));
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, m_DepthAttachment);

            // Vérification
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Framebuffer is not complete!");

            Unbind();
        }

        GLuint m_RendererID = 0;
        std::unique_ptr<Texture2D> m_ColorTexture;
        GLuint m_DepthAttachment = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };
}

#endif //ASHEN_FRAMEBUFFER_H