#ifndef ASHEN_VERTEXARRAY_H
#define ASHEN_VERTEXARRAY_H

#include <bitset>
#include <initializer_list>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "Ashen/GraphicsAPI/GLObject.h"

namespace ash {
    struct VertexAttributeDescription {
        uint32_t location;
        uint32_t componentCount;
        VertexAttribType type;
        bool normalized;
        size_t offset;
        uint32_t divisor = 0;

        static VertexAttributeDescription Float(const uint32_t location, const size_t offset = 0,
                                                const uint32_t divisor = 0) {
            return {location, 1, VertexAttribType::Float, false, offset, divisor};
        }

        static VertexAttributeDescription Vec2(const uint32_t location, const size_t offset = 0,
                                               const uint32_t divisor = 0) {
            return {location, 2, VertexAttribType::Float, false, offset, divisor};
        }

        static VertexAttributeDescription Vec3(const uint32_t location, const size_t offset = 0,
                                               const uint32_t divisor = 0) {
            return {location, 3, VertexAttribType::Float, false, offset, divisor};
        }

        static VertexAttributeDescription Vec4(const uint32_t location, const size_t offset = 0,
                                               const uint32_t divisor = 0) {
            return {location, 4, VertexAttribType::Float, false, offset, divisor};
        }

        static VertexAttributeDescription Int(const uint32_t location, const size_t offset = 0,
                                              const uint32_t divisor = 0) {
            return {location, 1, VertexAttribType::Int, false, offset, divisor};
        }

        static VertexAttributeDescription IVec2(const uint32_t location, const size_t offset = 0,
                                                const uint32_t divisor = 0) {
            return {location, 2, VertexAttribType::Int, false, offset, divisor};
        }

        static VertexAttributeDescription IVec3(const uint32_t location, const size_t offset = 0,
                                                const uint32_t divisor = 0) {
            return {location, 3, VertexAttribType::Int, false, offset, divisor};
        }

        static VertexAttributeDescription IVec4(const uint32_t location, const size_t offset = 0,
                                                const uint32_t divisor = 0) {
            return {location, 4, VertexAttribType::Int, false, offset, divisor};
        }

        static VertexAttributeDescription UInt(const uint32_t loc, const size_t off = 0, const uint32_t div = 0) {
            return {loc, 1, VertexAttribType::UnsignedInt, false, off, div};
        }

        static VertexAttributeDescription UVec2(const uint32_t loc, const size_t off = 0, const uint32_t div = 0) {
            return {loc, 2, VertexAttribType::UnsignedInt, false, off, div};
        }

        static VertexAttributeDescription UVec3(const uint32_t loc, const size_t off = 0, const uint32_t div = 0) {
            return {loc, 3, VertexAttribType::UnsignedInt, false, off, div};
        }

        static VertexAttributeDescription UVec4(const uint32_t loc, const size_t off = 0, const uint32_t div = 0) {
            return {loc, 4, VertexAttribType::UnsignedInt, false, off, div};
        }

        static VertexAttributeDescription Byte(const uint32_t loc, const size_t off = 0, const bool norm = true,
                                               const uint32_t div = 0) {
            return {loc, 1, VertexAttribType::Byte, norm, off, div};
        }

        static VertexAttributeDescription UByte(const uint32_t loc, const size_t off = 0, const bool norm = true,
                                                const uint32_t div = 0) {
            return {loc, 1, VertexAttribType::UnsignedByte, norm, off, div};
        }

        static VertexAttributeDescription Short(const uint32_t loc, const size_t off = 0, const bool norm = true,
                                                const uint32_t div = 0) {
            return {loc, 1, VertexAttribType::Short, norm, off, div};
        }

        static VertexAttributeDescription UShort(const uint32_t loc, const size_t off = 0, const bool norm = true,
                                                 const uint32_t div = 0) {
            return {loc, 1, VertexAttribType::UnsignedShort, norm, off, div};
        }
    };

    class VertexBufferLayout {
    public:
        VertexBufferLayout() = default;

        explicit VertexBufferLayout(const std::initializer_list<VertexAttributeDescription> attributes,
                                    const size_t stride = 0)
            : m_Attributes(attributes), m_Stride(stride) {
            if (m_Stride == 0)
                CalculateStride();
        }

        void AddAttribute(const VertexAttributeDescription &attrib) {
            m_Attributes.push_back(attrib);
            CalculateStride();
        }

        void SetStride(const size_t stride) { m_Stride = stride; }

        [[nodiscard]] const Vector<VertexAttributeDescription> &GetAttributes() const {
            return m_Attributes;
        }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

        //TODO: abstracted types predefined layouts
        static VertexBufferLayout Position2D() {
            return VertexBufferLayout({
                                          VertexAttributeDescription::Vec2(0, 0)
                                      }, sizeof(Vec2));
        }

    private:
        void CalculateStride() {
            size_t maxEnd = 0;
            for (const auto &attrib: m_Attributes) {
                const size_t size = attrib.componentCount * GetTypeSize(attrib.type);
                maxEnd = std::max(maxEnd, attrib.offset + size);
            }
            m_Stride = maxEnd;
        }

        static size_t GetTypeSize(const VertexAttribType type) {
            switch (type) {
                case VertexAttribType::Byte:
                case VertexAttribType::UnsignedByte:
                    return 1;
                case VertexAttribType::Short:
                case VertexAttribType::UnsignedShort:
                case VertexAttribType::HalfFloat:
                    return 2;
                case VertexAttribType::Int:
                case VertexAttribType::UnsignedInt:
                case VertexAttribType::Float:
                case VertexAttribType::Fixed:
                case VertexAttribType::Int2101010Rev:
                case VertexAttribType::UnsignedInt2101010Rev:
                case VertexAttribType::UnsignedInt10F11F11FRev:
                    return 4;
                case VertexAttribType::Double:
                    return 8;
                default:
                    return 4;
            }
        }

        Vector<VertexAttributeDescription> m_Attributes;
        size_t m_Stride = 0;
    };

    struct VertexArrayConfig {
        PrimitiveType primitiveMode = PrimitiveType::Triangles;
        bool enablePrimitiveRestart = false;
        uint32_t primitiveRestartIndex = 0xFFFFFFFF;

        static VertexArrayConfig Default() {
            return {};
        }

        static VertexArrayConfig Lines() {
            VertexArrayConfig config;
            config.primitiveMode = PrimitiveType::Lines;
            return config;
        }

        static VertexArrayConfig Points() {
            VertexArrayConfig config;
            config.primitiveMode = PrimitiveType::Points;
            return config;
        }

        static VertexArrayConfig TriangleStrip() {
            VertexArrayConfig config;
            config.primitiveMode = PrimitiveType::TriangleStrip;
            config.enablePrimitiveRestart = true;
            return config;
        }
    };

    class VertexArray final : public Bindable {
    public:
        explicit VertexArray(const VertexArrayConfig &config = VertexArrayConfig::Default())
            : m_Config(config) {
            glGenVertexArrays(1, &m_ID);

            if (config.enablePrimitiveRestart) {
                glEnable(GL_PRIMITIVE_RESTART);
                glPrimitiveRestartIndex(config.primitiveRestartIndex);
            }
        }

        ~VertexArray() override {
            if (m_ID)
                glDeleteVertexArrays(1, &m_ID);
        }

        VertexArray(const VertexArray &) = delete;

        VertexArray &operator=(const VertexArray &) = delete;

        VertexArray(VertexArray &&other) noexcept
            : m_EnabledAttribs(other.m_EnabledAttribs),
              m_VertexBuffers(std::move(other.m_VertexBuffers)),
              m_IndexBuffer(std::move(other.m_IndexBuffer)),
              m_Config(other.m_Config) {
            m_ID = other.m_ID;
            other.m_ID = 0;
        }

        VertexArray &operator=(VertexArray &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteVertexArrays(1, &m_ID);
                m_ID = other.m_ID;
                m_EnabledAttribs = other.m_EnabledAttribs;
                m_VertexBuffers = std::move(other.m_VertexBuffers);
                m_IndexBuffer = std::move(other.m_IndexBuffer);
                m_Config = other.m_Config;
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override {
            glBindVertexArray(m_ID);
        }

        void Unbind() const override {
            glBindVertexArray(0);
        }

        void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vbo,
                             const VertexBufferLayout &layout) {
            Bind();
            vbo->Bind();

            for (const auto &attrib: layout.GetAttributes()) {
                EnableAttrib(attrib.location);

                const bool isInteger =
                        attrib.type == VertexAttribType::Byte ||
                        attrib.type == VertexAttribType::UnsignedByte ||
                        attrib.type == VertexAttribType::Short ||
                        attrib.type == VertexAttribType::UnsignedShort ||
                        attrib.type == VertexAttribType::Int ||
                        attrib.type == VertexAttribType::UnsignedInt;

                if (isInteger && !attrib.normalized) {
                    glVertexAttribIPointer(
                        attrib.location,
                        attrib.componentCount,
                        static_cast<GLenum>(attrib.type),
                        static_cast<GLsizei>(layout.GetStride()),
                        reinterpret_cast<void *>(attrib.offset)
                    );
                } else {
                    glVertexAttribPointer(
                        attrib.location,
                        attrib.componentCount,
                        static_cast<GLenum>(attrib.type),
                        attrib.normalized ? GL_TRUE : GL_FALSE,
                        static_cast<GLsizei>(layout.GetStride()),
                        reinterpret_cast<void *>(attrib.offset)
                    );
                }

                if (attrib.divisor > 0) {
                    glVertexAttribDivisor(attrib.location, attrib.divisor);
                }
            }

            m_VertexBuffers.push_back(vbo);
        }

        void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &ibo) {
            Bind();
            ibo->Bind();
            m_IndexBuffer = ibo;
        }

        bool HasIndexBuffer() const { return m_IndexBuffer != nullptr; }
        bool HasVertices() const { return !m_VertexBuffers.empty(); }

        uint32_t GetIndexCount() const {
            return m_IndexBuffer ? m_IndexBuffer->GetCount() : 0;
        }

        uint32_t GetVertexCount() const {
            if (m_VertexBuffers.empty()) return 0;
            return m_VertexBuffers[0]->GetCount();
        }

        [[nodiscard]] const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
        [[nodiscard]] const Vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

        [[nodiscard]] const VertexArrayConfig &GetConfig() const { return m_Config; }

        static Ref<VertexArray> Create(const VertexArrayConfig &config = VertexArrayConfig::Default()) {
            return MakeRef<VertexArray>(config);
        }

        static Ref<VertexArray> CreateWithBuffer(const Ref<VertexBuffer> &vbo,
                                                 const VertexBufferLayout &layout,
                                                 const VertexArrayConfig &config = VertexArrayConfig::Default()) {
            auto vao = MakeRef<VertexArray>(config);
            vao->AddVertexBuffer(vbo, layout);
            return vao;
        }

        static Ref<VertexArray> CreateIndexed(const Ref<VertexBuffer> &vbo,
                                              const VertexBufferLayout &layout,
                                              const Ref<IndexBuffer> &ibo,
                                              const VertexArrayConfig &config = VertexArrayConfig::Default()) {
            auto vao = MakeRef<VertexArray>(config);
            vao->AddVertexBuffer(vbo, layout);
            vao->SetIndexBuffer(ibo);
            return vao;
        }

        // Helper for common case: create everything at once
        template<typename T>
        static Ref<VertexArray> CreateFromData(const std::span<const T> &vertices,
                                               const VertexBufferLayout &layout,
                                               const VertexArrayConfig &config = VertexArrayConfig::Default()) {
            auto vbo = VertexBuffer::Create(vertices);
            return CreateWithBuffer(vbo, layout, config);
        }

        template<typename VertexT, typename IndexT = uint32_t>
        static Ref<VertexArray> CreateIndexedFromData(const std::span<const VertexT> &vertices,
                                                      const std::span<const IndexT> &indices,
                                                      const VertexBufferLayout &layout,
                                                      const VertexArrayConfig &config = VertexArrayConfig::Default()) {
            auto vbo = VertexBuffer::Create(vertices);
            auto ibo = IndexBuffer::Create(indices);
            return CreateIndexed(vbo, layout, ibo, config);
        }

    private:
        mutable std::bitset<32> m_EnabledAttribs{};
        Vector<std::shared_ptr<VertexBuffer> > m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        VertexArrayConfig m_Config;

        void EnableAttrib(const uint32_t location) const {
            if (location >= 32) return;
            if (!m_EnabledAttribs.test(location)) {
                glEnableVertexAttribArray(location);
                m_EnabledAttribs.set(location);
            }
        }

        [[nodiscard]] size_t GetIndexSize() const {
            if (!m_IndexBuffer) return 0;

            switch (m_IndexBuffer->GetIndexType()) {
                case IndexType::UnsignedByte: return 1;
                case IndexType::UnsignedShort: return 2;
                case IndexType::UnsignedInt: return 4;
                default: return 4;
            }
        }
    };
}

#endif //ASHEN_VERTEXARRAY_H