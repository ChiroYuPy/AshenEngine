#ifndef ASHEN_BUFFER_H
#define ASHEN_BUFFER_H

#include <glad/glad.h>

#include "Ashen/Core/Types.h"
#include "Ashen/GraphicsAPI/GLEnums.h"
#include "Ashen/GraphicsAPI/GLObject.h"

namespace ash {
    struct BufferConfig {
        BufferUsage usage = BufferUsage::StaticDraw;

        static BufferConfig Static() {
            BufferConfig config;
            config.usage = BufferUsage::StaticDraw;
            return config;
        }

        static BufferConfig Dynamic() {
            BufferConfig config;
            config.usage = BufferUsage::DynamicDraw;
            return config;
        }

        static BufferConfig Stream() {
            BufferConfig config;
            config.usage = BufferUsage::StreamDraw;
            return config;
        }
    };

    class Buffer : public Bindable {
    public:
        explicit Buffer(const BufferTarget target, const BufferConfig &config = BufferConfig::Static())
            : m_Target(target), m_Config(config) {
            glGenBuffers(1, &m_ID);
        }

        ~Buffer() override {
            if (m_ID)
                glDeleteBuffers(1, &m_ID);
        }

        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;

        Buffer(Buffer &&other) noexcept
            : m_Target(other.m_Target), m_Size(other.m_Size), m_Config(other.m_Config) {
            m_ID = other.m_ID;
            other.m_ID = 0;
            other.m_Size = 0;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteBuffers(1, &m_ID);
                m_ID = other.m_ID;
                m_Target = other.m_Target;
                m_Size = other.m_Size;
                m_Config = other.m_Config;
                other.m_ID = 0;
                other.m_Size = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindBuffer(static_cast<GLenum>(m_Target), m_ID);
        }

        void Unbind() const override {
            glBindBuffer(static_cast<GLenum>(m_Target), 0);
        }

        void BindBase(const GLuint index) const {
            glBindBufferBase(static_cast<GLenum>(m_Target), index, m_ID);
        }

        void BindRange(const GLuint index, const GLintptr offset, const GLsizeiptr size) const {
            glBindBufferRange(static_cast<GLenum>(m_Target), index, m_ID, offset, size);
        }

        [[nodiscard]] size_t Size() const { return m_Size; }
        [[nodiscard]] BufferTarget Target() const { return m_Target; }
        [[nodiscard]] const BufferConfig &GetConfig() const { return m_Config; }

        [[nodiscard]] void *Map(BufferAccess access) const {
            Bind();
            return glMapBuffer(static_cast<GLenum>(m_Target), static_cast<GLenum>(access));
        }

        void Unmap() const {
            Bind();
            glUnmapBuffer(static_cast<GLenum>(m_Target));
        }

    protected:
        BufferTarget m_Target;
        size_t m_Size = 0;
        BufferConfig m_Config;

        void AllocateEmpty(const size_t sizeInBytes) {
            Bind();
            glBufferData(static_cast<GLenum>(m_Target), static_cast<GLsizeiptr>(sizeInBytes),
                         nullptr, static_cast<GLenum>(m_Config.usage));
            m_Size = sizeInBytes;
        }

        template<typename T>
        void UploadData(const Span<const T> &data) {
            Bind();
            glBufferData(static_cast<GLenum>(m_Target), data.size_bytes(),
                         data.data(), static_cast<GLenum>(m_Config.usage));
            m_Size = data.size_bytes();
        }

        template<typename T>
        void UpdateData(const Span<const T> &data, const size_t offset = 0) {
            assert(offset + data.size_bytes() <= m_Size && "Buffer overflow in UpdateData!");
            Bind();
            glBufferSubData(static_cast<GLenum>(m_Target), static_cast<GLintptr>(offset),
                            data.size_bytes(), data.data());
        }
    };

    class VertexBuffer final : public Buffer {
    public:
        explicit VertexBuffer(const BufferConfig &config = BufferConfig::Static())
            : Buffer(BufferTarget::Array, config) {
        }

        template<typename T>
        void SetData(const Span<const T> &data) {
            UploadData(data);
            m_Count = data.size();
        }

        void SetEmpty(const size_t count, const size_t elementSize) {
            AllocateEmpty(count * elementSize);
            m_Count = count;
        }

        template<typename T>
        void Update(const Span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        [[nodiscard]] size_t GetCount() const { return m_Count; }

        template<typename T>
        static Ref<VertexBuffer> Create(const Span<const T> &data,
                                        const BufferConfig &config = BufferConfig::Static()) {
            auto vbo = MakeRef<VertexBuffer>(config);
            vbo->SetData(data);
            return vbo;
        }

        static Ref<VertexBuffer> CreateEmpty(size_t count, size_t elementSize,
                                             const BufferConfig &config = BufferConfig::Dynamic()) {
            auto vbo = MakeRef<VertexBuffer>(config);
            vbo->SetEmpty(count, elementSize);
            return vbo;
        }

        template<typename T>
        static Ref<VertexBuffer> CreateDynamic(const Span<const T> &data) {
            return Create(data, BufferConfig::Dynamic());
        }

        template<typename T>
        static Ref<VertexBuffer> CreateStream(const Span<const T> &data) {
            return Create(data, BufferConfig::Stream());
        }

    private:
        size_t m_Count = 0;
    };

    class IndexBuffer final : public Buffer {
    public:
        explicit IndexBuffer(const IndexType type = IndexType::UnsignedInt,
                             const BufferConfig &config = BufferConfig::Static())
            : Buffer(BufferTarget::ElementArray, config), m_IndexType(type) {
        }

        template<typename T = uint32_t>
        void SetData(const Span<const T> &data) {
            static_assert(std::is_same_v<T, uint8_t> ||
                          std::is_same_v<T, uint16_t> ||
                          std::is_same_v<T, uint32_t>,
                          "Index type must be uint8_t, uint16_t, or uint32_t");

            if constexpr (std::is_same_v<T, uint8_t>)
                m_IndexType = IndexType::UnsignedByte;
            else if constexpr (std::is_same_v<T, uint16_t>)
                m_IndexType = IndexType::UnsignedShort;
            else
                m_IndexType = IndexType::UnsignedInt;

            UploadData(data);
            m_Count = data.size();
        }

        void SetEmpty(const size_t count, const size_t indexSize) {
            AllocateEmpty(count * indexSize);
            m_Count = count;
        }

        template<typename T = uint32_t>
        void Update(const Span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        [[nodiscard]] size_t GetCount() const { return m_Count; }
        [[nodiscard]] IndexType GetIndexType() const { return m_IndexType; }

        template<typename T = uint32_t>
        static Ref<IndexBuffer> Create(const Span<const T> &data,
                                       const BufferConfig &config = BufferConfig::Static()) {
            static_assert(std::is_same_v<T, uint8_t> ||
                          std::is_same_v<T, uint16_t> ||
                          std::is_same_v<T, uint32_t>,
                          "Index type must be uint8_t, uint16_t, or uint32_t");

            IndexType type;
            if constexpr (std::is_same_v<T, uint8_t>)
                type = IndexType::UnsignedByte;
            else if constexpr (std::is_same_v<T, uint16_t>)
                type = IndexType::UnsignedShort;
            else
                type = IndexType::UnsignedInt;

            auto ibo = MakeRef<IndexBuffer>(type, config);
            ibo->SetData(data);
            return ibo;
        }

        static Ref<IndexBuffer> CreateEmpty(size_t count, size_t indexSize,
                                            IndexType type = IndexType::UnsignedInt,
                                            const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ibo = MakeRef<IndexBuffer>(type, config);
            ibo->SetEmpty(count, indexSize);
            return ibo;
        }

        template<typename T = uint32_t>
        static Ref<IndexBuffer> CreateDynamic(const Span<const T> &data) {
            return Create(data, BufferConfig::Dynamic());
        }

    private:
        size_t m_Count = 0;
        IndexType m_IndexType;
    };

    class UniformBuffer final : public Buffer {
    public:
        explicit UniformBuffer(const BufferConfig &config = BufferConfig::Dynamic())
            : Buffer(BufferTarget::Uniform, config) {
        }

        template<typename T>
        void SetData(const Span<const T> &data) {
            UploadData(data);
        }

        template<typename T>
        void SetData(const T &data) {
            SetData(Span<const T>(&data, 1));
        }

        template<typename T>
        void Update(const Span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        template<typename T>
        void Update(const T &data, size_t offset = 0) {
            Update(Span<const T>(&data, 1), offset);
        }

        void Allocate(const size_t size) {
            AllocateEmpty(size);
        }

        template<typename T>
        static Ref<UniformBuffer> Create(const T &data,
                                         const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ubo = MakeRef<UniformBuffer>(config);
            ubo->SetData(data);
            return ubo;
        }

        template<typename T>
        static Ref<UniformBuffer> Create(const Span<const T> &data,
                                         const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ubo = MakeRef<UniformBuffer>(config);
            ubo->SetData(data);
            return ubo;
        }

        static Ref<UniformBuffer> CreateEmpty(size_t size,
                                              const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ubo = MakeRef<UniformBuffer>(config);
            ubo->Allocate(size);
            return ubo;
        }
    };

    class ShaderStorageBuffer final : public Buffer {
    public:
        explicit ShaderStorageBuffer(const BufferConfig &config = BufferConfig::Dynamic())
            : Buffer(BufferTarget::ShaderStorage, config) {
        }

        template<typename T>
        void SetData(const Span<const T> &data) {
            UploadData(data);
        }

        template<typename T>
        void Update(const Span<const T> &data, size_t offset = 0) {
            UpdateData(data, offset);
        }

        void Allocate(const size_t size) {
            AllocateEmpty(size);
        }

        template<typename T>
        static Ref<ShaderStorageBuffer> Create(const Span<const T> &data,
                                               const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ssbo = MakeRef<ShaderStorageBuffer>(config);
            ssbo->SetData(data);
            return ssbo;
        }

        static Ref<ShaderStorageBuffer> CreateEmpty(size_t size,
                                                    const BufferConfig &config = BufferConfig::Dynamic()) {
            auto ssbo = MakeRef<ShaderStorageBuffer>(config);
            ssbo->Allocate(size);
            return ssbo;
        }
    };
}

#endif // ASHEN_BUFFER_H