#ifndef ASHEN_VERTEXARRAY_H
#define ASHEN_VERTEXARRAY_H

#include <bitset>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Buffer.h"
#include "Ashen/math/Math.h"
#include "Ashen/renderer/Bindable.h"

namespace pixl {
    template<typename T>
    struct VertexAttrib {
        static constexpr GLint count = 1;
        static constexpr GLenum type = GL_FLOAT;
        static constexpr bool integral = std::is_integral_v<T>;
    };

    // scalars
    template<>
    struct VertexAttrib<float> {
        static constexpr GLint count = 1;
        static constexpr GLenum type = GL_FLOAT;
        static constexpr bool integral = false;
    };

    template<>
    struct VertexAttrib<double> {
        static constexpr GLint count = 1;
        static constexpr GLenum type = GL_DOUBLE;
        static constexpr bool integral = false;
    };

    template<>
    struct VertexAttrib<int> {
        static constexpr GLint count = 1;
        static constexpr GLenum type = GL_INT;
        static constexpr bool integral = true;
    };

    template<>
    struct VertexAttrib<unsigned int> {
        static constexpr GLint count = 1;
        static constexpr GLenum type = GL_UNSIGNED_INT;
        static constexpr bool integral = true;
    };

    // vectors
    template<>
    struct VertexAttrib<Vec2> {
        static constexpr GLint count = 2;
        static constexpr GLenum type = GL_FLOAT;
        static constexpr bool integral = false;
    };

    template<>
    struct VertexAttrib<Vec3> {
        static constexpr GLint count = 3;
        static constexpr GLenum type = GL_FLOAT;
        static constexpr bool integral = false;
    };

    template<>
    struct VertexAttrib<Vec4> {
        static constexpr GLint count = 4;
        static constexpr GLenum type = GL_FLOAT;
        static constexpr bool integral = false;
    };

    template<>
    struct VertexAttrib<IVec2> {
        static constexpr GLint count = 2;
        static constexpr GLenum type = GL_INT;
        static constexpr bool integral = true;
    };

    template<>
    struct VertexAttrib<IVec3> {
        static constexpr GLint count = 3;
        static constexpr GLenum type = GL_INT;
        static constexpr bool integral = true;
    };

    template<>
    struct VertexAttrib<IVec4> {
        static constexpr GLint count = 4;
        static constexpr GLenum type = GL_INT;
        static constexpr bool integral = true;
    };

    struct VertexBufferElement {
        GLuint index;
        GLuint size;
        GLenum type;
        bool integral;
        size_t offset;
        GLuint divisor = 0;
    };

    class VertexBufferLayout {
    public:
        explicit VertexBufferLayout(const size_t stride = 0) : m_Stride(stride) {
        }

        template<typename T>
        void AddAttribute(const GLuint index, const size_t offset = 0, const GLuint divisor = 0) {
            m_Elements.push_back({
                index,
                VertexAttrib<T>::count,
                VertexAttrib<T>::type,
                VertexAttrib<T>::integral,
                offset,
                divisor
            });
            m_Stride = std::max(m_Stride, offset + sizeof(T));
        }

        const std::vector<VertexBufferElement> &GetElements() const { return m_Elements; }
        size_t GetStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        size_t m_Stride = 0;
    };

    class VertexArray final : public Bindable {
    public:
        VertexArray() { glGenVertexArrays(1, &m_ID); }
        ~VertexArray() override { if (m_ID) glDeleteVertexArrays(1, &m_ID); }

        VertexArray(const VertexArray &) = delete;

        VertexArray &operator=(const VertexArray &) = delete;

        VertexArray(VertexArray &&other) noexcept
            : m_ID(other.m_ID), m_VertexBuffers(std::move(other.m_VertexBuffers)),
              m_IndexBuffer(std::move(other.m_IndexBuffer)) {
            other.m_ID = 0;
        }

        VertexArray &operator=(VertexArray &&other) noexcept {
            if (this != &other) {
                if (m_ID) glDeleteVertexArrays(1, &m_ID);
                m_ID = other.m_ID;
                m_VertexBuffers = std::move(other.m_VertexBuffers);
                m_IndexBuffer = std::move(other.m_IndexBuffer);
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override { glBindVertexArray(m_ID); }
        void Unbind() const override { glBindVertexArray(0); }

        // Ajoute un VBO avec son layout complet
        void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vbo, const VertexBufferLayout &layout) {
            Bind();
            vbo->Bind();
            for (const auto &elem: layout.GetElements()) {
                EnableAttrib(elem.index);
                if (elem.integral) {
                    glVertexAttribIPointer(elem.index, elem.size, elem.type, static_cast<GLsizei>(layout.GetStride()),
                                           reinterpret_cast<void *>(elem.offset));
                } else {
                    glVertexAttribPointer(elem.index, elem.size, elem.type, GL_FALSE,
                                          static_cast<GLsizei>(layout.GetStride()),
                                          reinterpret_cast<void *>(elem.offset));
                }
                if (elem.divisor > 0) glVertexAttribDivisor(elem.index, elem.divisor);
            }
            m_VertexBuffers.push_back(vbo);
        }

        void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &ibo) {
            Bind();
            ibo->Bind();
            m_IndexBuffer = ibo;
        }

        GLuint ID() const { return m_ID; }

        const std::vector<std::shared_ptr<VertexBuffer> > &GetVertexBuffers() const { return m_VertexBuffers; }
        const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const { return m_IndexBuffer; }

        // Draw automatique
        void Draw(const GLenum mode = GL_TRIANGLES) const {
            Bind();
            if (m_IndexBuffer)
                glDrawElements(mode, static_cast<GLsizei>(m_IndexBuffer->Count()), GL_UNSIGNED_INT, nullptr);
            else if (!m_VertexBuffers.empty())
                glDrawArrays(mode, 0, static_cast<GLsizei>(m_VertexBuffers[0]->Count()));
        }

    private:
        GLuint m_ID = 0;
        mutable std::bitset<16> m_EnabledAttribs{};
        std::vector<std::shared_ptr<VertexBuffer> > m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

        void EnableAttrib(const GLuint index) const {
            if (!m_EnabledAttribs.test(index)) {
                glEnableVertexAttribArray(index);
                m_EnabledAttribs.set(index);
            }
        }
    };
}

#endif //ASHEN_VERTEXARRAY_H