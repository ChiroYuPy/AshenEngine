#ifndef ASHEN_FRAMEBUFFER_H
#define ASHEN_FRAMEBUFFER_H

#include <iostream>
#include <stdexcept>
#include <optional>

#include <../../../vendor/include/glad.h>

#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Core/Types.h"
#include "Ashen/GraphicsAPI/RenderCommand.h"

namespace ash {
    struct FramebufferAttachmentSpec {
        TextureFormat format = TextureFormat::RGBA8;
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        TextureWrap wrap = TextureWrap::ClampToEdge;

        static FramebufferAttachmentSpec Color() {
            return {TextureFormat::RGBA8};
        }

        static FramebufferAttachmentSpec ColorHDR() {
            return {TextureFormat::RGBA16F};
        }

        static FramebufferAttachmentSpec Depth() {
            return {TextureFormat::Depth24};
        }

        static FramebufferAttachmentSpec DepthStencil() {
            return {TextureFormat::Depth24Stencil8};
        }
    };

    struct FramebufferConfig {
        uint32_t width = 1280;
        uint32_t height = 720;
        uint32_t samples = 1;
        Vector<FramebufferAttachmentSpec> colorAttachments;
        std::optional<FramebufferAttachmentSpec> depthAttachment;
        bool swapChainTarget = false;

        static FramebufferConfig Default(const uint32_t w = 1280, const uint32_t h = 720) {
            FramebufferConfig config;
            config.width = w;
            config.height = h;
            config.colorAttachments.push_back(FramebufferAttachmentSpec::Color());
            config.depthAttachment = FramebufferAttachmentSpec::DepthStencil();
            return config;
        }

        static FramebufferConfig HDR(const uint32_t w = 1280, const uint32_t h = 720) {
            FramebufferConfig config;
            config.width = w;
            config.height = h;
            config.colorAttachments.push_back(FramebufferAttachmentSpec::ColorHDR());
            config.depthAttachment = FramebufferAttachmentSpec::DepthStencil();
            return config;
        }

        static FramebufferConfig ColorOnly(const uint32_t w = 1280, const uint32_t h = 720) {
            FramebufferConfig config;
            config.width = w;
            config.height = h;
            config.colorAttachments.push_back(FramebufferAttachmentSpec::Color());
            return config;
        }

        static FramebufferConfig Multisampled(const uint32_t samples, const uint32_t w = 1280, const uint32_t h = 720) {
            FramebufferConfig config = Default(w, h);
            config.samples = samples;
            return config;
        }
    };

    class Framebuffer final : public Bindable {
    public:
        explicit Framebuffer(const FramebufferConfig &config = FramebufferConfig::Default())
            : m_Config(config) {
            if (config.width == 0 || config.height == 0)
                throw std::invalid_argument("Framebuffer dimensions must be > 0");

            if (config.colorAttachments.empty() && !config.depthAttachment.has_value())
                throw std::invalid_argument("Framebuffer must have at least one attachment");

            glGenFramebuffers(1, &m_ID);
            CreateAttachments();
        }

        ~Framebuffer() override {
            if (m_ID)
                glDeleteFramebuffers(1, &m_ID);
            if (m_DepthRenderbuffer)
                glDeleteRenderbuffers(1, &m_DepthRenderbuffer);
        }

        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&other) noexcept
            : m_ColorTextures(std::move(other.m_ColorTextures)),
              m_DepthTexture(std::move(other.m_DepthTexture)),
              m_DepthRenderbuffer(other.m_DepthRenderbuffer),
              m_Config(other.m_Config) {
            m_ID = other.m_ID;
            other.m_ID = 0;
            other.m_DepthRenderbuffer = 0;
        }

        Framebuffer &operator=(Framebuffer &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteFramebuffers(1, &m_ID);
                if (m_DepthRenderbuffer)
                    glDeleteRenderbuffers(1, &m_DepthRenderbuffer);

                m_ID = other.m_ID;
                m_ColorTextures = std::move(other.m_ColorTextures);
                m_DepthTexture = std::move(other.m_DepthTexture);
                m_DepthRenderbuffer = other.m_DepthRenderbuffer;
                m_Config = other.m_Config;

                other.m_ID = 0;
                other.m_DepthRenderbuffer = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
            RenderCommand::SetViewport(m_Config.width, m_Config.height);
        }

        void Unbind() const override {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void BindRead() const {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID);
        }

        void BindDraw() const {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
        }

        void Resize(const uint32_t width, const uint32_t height) {
            if (width == 0 || height == 0)
                throw std::invalid_argument("Framebuffer dimensions must be > 0");

            m_Config.width = width;
            m_Config.height = height;

            m_ColorTextures.clear();
            if (m_DepthRenderbuffer) {
                glDeleteRenderbuffers(1, &m_DepthRenderbuffer);
                m_DepthRenderbuffer = 0;
            }
            m_DepthTexture.reset();

            CreateAttachments();
        }

        void AttachColorTexture(const Texture2D &texture, const uint32_t index = 0) {
            Bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                                   GL_TEXTURE_2D, texture.ID(), 0);
        }

        void AttachDepthTexture(const Texture2D &texture) {
            Bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D, texture.ID(), 0);
        }

        [[nodiscard]] const Texture2D *GetColorTexture(const size_t index = 0) const {
            if (index >= m_ColorTextures.size())
                return nullptr;
            return m_ColorTextures[index].get();
        }

        [[nodiscard]] const Texture2D *GetDepthTexture() const {
            return m_DepthTexture.get();
        }

        [[nodiscard]] size_t GetColorAttachmentCount() const {
            return m_ColorTextures.size();
        }

        [[nodiscard]] uint32_t GetWidth() const { return m_Config.width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Config.height; }
        [[nodiscard]] const FramebufferConfig &GetConfig() const { return m_Config; }

        [[nodiscard]] bool IsComplete() const {
            Bind();
            const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer incomplete! Status: " << status << std::endl;
            }
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        void BlitTo(const Framebuffer &target, ClearBuffer bufferMask = ClearBuffer::ColorDepth) const {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.ID());

            glBlitFramebuffer(
                0, 0, m_Config.width, m_Config.height,
                0, 0, target.m_Config.width, target.m_Config.height,
                static_cast<GLbitfield>(bufferMask), GL_NEAREST
            );

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Clear(const Vec4 &color = Vec4(0.0f), const float depth = 1.0f) const {
            Bind();
            glClearColor(color.r, color.g, color.b, color.a);
            glClearDepth(depth);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

    private:
        void CreateAttachments() {
            Bind();

            for (size_t i = 0; i < m_Config.colorAttachments.size(); ++i) {
                const auto &spec = m_Config.colorAttachments[i];

                auto colorTex = MakeOwn<Texture2D>();
                colorTex->SetData(
                    spec.format,
                    static_cast<GLsizei>(m_Config.width),
                    static_cast<GLsizei>(m_Config.height),
                    spec.format,
                    PixelDataType::UnsignedByte,
                    nullptr
                );

                TextureConfig texConfig;
                texConfig.minFilter = spec.minFilter;
                texConfig.magFilter = spec.magFilter;
                texConfig.wrapS = spec.wrap;
                texConfig.wrapT = spec.wrap;
                colorTex->ApplyConfig(texConfig);

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
                    GL_TEXTURE_2D,
                    colorTex->ID(),
                    0
                );

                m_ColorTextures.push_back(std::move(colorTex));
            }

            if (!m_ColorTextures.empty()) {
                Vector<GLenum> attachments;
                for (size_t i = 0; i < m_ColorTextures.size(); ++i) {
                    attachments.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i));
                }
                glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
            } else {
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
            }

            if (m_Config.depthAttachment.has_value()) {
                const auto &spec = m_Config.depthAttachment.value();

                if (m_Config.samples > 1) {
                    glGenRenderbuffers(1, &m_DepthRenderbuffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderbuffer);
                    glRenderbufferStorageMultisample(
                        GL_RENDERBUFFER,
                        m_Config.samples,
                        static_cast<GLenum>(spec.format),
                        m_Config.width,
                        m_Config.height
                    );
                    glFramebufferRenderbuffer(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_RENDERBUFFER,
                        m_DepthRenderbuffer
                    );
                } else {
                    m_DepthTexture = MakeOwn<Texture2D>();
                    m_DepthTexture->SetData(
                        spec.format,
                        static_cast<GLsizei>(m_Config.width),
                        static_cast<GLsizei>(m_Config.height),
                        TextureFormat::DepthStencil,
                        PixelDataType::UnsignedInt,
                        nullptr
                    );

                    TextureConfig texConfig;
                    texConfig.minFilter = spec.minFilter;
                    texConfig.magFilter = spec.magFilter;
                    texConfig.wrapS = spec.wrap;
                    texConfig.wrapT = spec.wrap;
                    m_DepthTexture->ApplyConfig(texConfig);

                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_TEXTURE_2D,
                        m_DepthTexture->ID(),
                        0
                    );
                }
            }

            if (!IsComplete())
                throw std::runtime_error("Framebuffer is not complete!");

            Unbind();
        }

        Vector<Own<Texture2D> > m_ColorTextures;
        Own<Texture2D> m_DepthTexture;
        GLuint m_DepthRenderbuffer = 0;
        FramebufferConfig m_Config;
    };
}

#endif //ASHEN_FRAMEBUFFER_H
