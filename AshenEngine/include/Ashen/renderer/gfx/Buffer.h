#ifndef ASHEN_BUFFER_H
#define ASHEN_BUFFER_H

#include <span>
#include <glad/glad.h>

#include "../Bindable.h"

namespace ash {
    enum class BufferType : GLenum {
        Vertex = GL_ARRAY_BUFFER,
        Index = GL_ELEMENT_ARRAY_BUFFER,
        Uniform = GL_UNIFORM_BUFFER
    };

    enum class BufferUsage : GLenum {
        Static = GL_STATIC_DRAW,
        Dynamic = GL_DYNAMIC_DRAW,
        Stream = GL_STREAM_DRAW
    };

    class Buffer : public Bindable {
    public:
        explicit Buffer(BufferType type)
            : m_Type(static_cast<GLenum>(type)) {
            glGenBuffers(1, &m_ID);
        }

        ~Buffer() override {
            if (m_ID) glDeleteBuffers(1, &m_ID);
        }

        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;

        Buffer(Buffer &&other) noexcept
            : m_ID(other.m_ID), m_Type(other.m_Type), m_Size(other.m_Size) {
            other.m_ID = 0;
            other.m_Size = 0;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (this != &other) {
                if (m_ID) glDeleteBuffers(1, &m_ID);
                m_ID = other.m_ID;
                m_Type = other.m_Type;
                m_Size = other.m_Size;
                other.m_ID = 0;
                other.m_Size = 0;
            }
            return *this;
        }

        void Bind() const override { glBindBuffer(m_Type, m_ID); }
        void Unbind() const override { glBindBuffer(m_Type, 0); }

        size_t Size() const { return m_Size; }

    protected:
        void AllocateEmpty(const size_t sizeInBytes, BufferUsage usage = BufferUsage::Dynamic) {
            Bind();
            glBufferData(m_Type, static_cast<GLsizeiptr>(sizeInBytes), nullptr, static_cast<GLenum>(usage));
            m_Size = sizeInBytes;
        }

        template<typename T>
        void UploadData(const std::span<const T> &data, BufferUsage usage = BufferUsage::Static) {
            Bind();
            glBufferData(m_Type, data.size_bytes(), data.data(), static_cast<GLenum>(usage));
            m_Size = data.size_bytes();
        }

        template<typename T>
        void UpdateData(const std::span<const T> &data, const size_t offset = 0) {
            assert(offset + data.size_bytes() <= m_Size && "Buffer overflow in UpdateData!");
            Bind();
            glBufferSubData(m_Type, static_cast<GLintptr>(offset), data.size_bytes(), data.data());
        }

        GLuint m_ID = 0;
        GLenum m_Type;
        size_t m_Size = 0;
    };

    class VertexBuffer final : public Buffer {
    public:
        VertexBuffer() : Buffer(BufferType::Vertex) {
        }

        template<typename T>
        void SetData(const std::span<const T> &data, BufferUsage usage = BufferUsage::Static) {
            UploadData(data, usage);
            m_Count = data.size();
        }

        template<typename T>
        void SetEmpty(const size_t count, const BufferUsage usage = BufferUsage::Dynamic) {
            AllocateEmpty(count * sizeof(T), usage);
            m_Count = count;
        }

        template<typename T>
        void Update(const std::span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        size_t Count() const { return m_Count; }

    private:
        size_t m_Count = 0;
    };

    class IndexBuffer final : public Buffer {
    public:
        IndexBuffer() : Buffer(BufferType::Index) {
        }

        template<typename T = uint32_t>
        void SetData(const std::span<const T> &data, BufferUsage usage = BufferUsage::Static) {
            UploadData(data, usage);
            m_Count = data.size();
        }

        template<typename T = uint32_t>
        void SetEmpty(const size_t count, const BufferUsage usage = BufferUsage::Dynamic) {
            AllocateEmpty(count * sizeof(T), usage);
            m_Count = count;
        }

        template<typename T = uint32_t>
        void Update(const std::span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        size_t Count() const { return m_Count; }

    private:
        size_t m_Count = 0;
    };

    class UniformBuffer final : public Buffer {
    public:
        UniformBuffer() : Buffer(BufferType::Uniform) {
        }

        void BindBase(const GLuint bindingPoint) const {
            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
        }

        template<typename T>
        void SetData(const std::span<const T> &data, BufferUsage usage = BufferUsage::Static) {
            UploadData(data, usage);
        }

        template<typename T>
        void Update(const std::span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }
    };
}

#endif // ASHEN_BUFFER_H