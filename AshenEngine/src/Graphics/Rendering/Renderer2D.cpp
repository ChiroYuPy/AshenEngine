#include "Ashen/Graphics/Rendering/Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Ashen/Graphics/Rendering/Renderer.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/GraphicsAPI/VertexArray.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Renderer2D::RenderData Renderer2D::s_Data{};

    void Renderer2D::Init() {
        // =============== QUADS ===============
        {
            auto &quads = s_Data.Quads;

            quads.VertexArray2D = MakeRef<VertexArray>();
            quads.VertexBuffer2D = MakeRef<VertexBuffer>(BufferConfig::Dynamic());
            quads.VertexBuffer2D->SetEmpty(QuadData::MaxVertices, sizeof(QuadVertex));

            const VertexBufferLayout layout({
                VertexAttributeDescription::Vec3(0, offsetof(QuadVertex, Position)),
                VertexAttributeDescription::Vec4(1, offsetof(QuadVertex, Color)),
                VertexAttributeDescription::Vec2(2, offsetof(QuadVertex, TexCoord)),
                VertexAttributeDescription::Float(3, offsetof(QuadVertex, TexIndex)),
            });

            quads.VertexArray2D->AddVertexBuffer(quads.VertexBuffer2D, layout);

            quads.IndexBuffer2D = MakeRef<IndexBuffer>();
            std::vector<uint32_t> indices(QuadData::MaxIndices);
            uint32_t offset = 0;
            for (size_t i = 0; i < QuadData::MaxIndices; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;
                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;
                offset += 4;
            }
            quads.IndexBuffer2D->SetData<uint32_t>(indices);
            quads.VertexArray2D->SetIndexBuffer(quads.IndexBuffer2D);

            quads.WhiteTexture = MakeRef<Texture2D>();
            constexpr uint32_t white = 0xFFFFFFFF;
            quads.WhiteTexture->SetData(TextureFormat::RGBA, 1, 1, TextureFormat::RGBA, PixelDataType::UnsignedByte,
                                        &white);
            quads.WhiteTexture->SetFilter(TextureFilter::Linear, TextureFilter::Linear);
            quads.WhiteTexture->SetWrap(TextureWrap::Repeat, TextureWrap::Repeat);
            quads.TextureSlots[0] = quads.WhiteTexture;

            const std::string quadVertSrc = R"(
                #version 410 core
                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;
                layout(location = 2) in vec2 a_TexCoord;
                layout(location = 3) in float a_TexIndex;
                uniform mat4 u_ViewProjection;
                out vec4 v_Color;
                out vec2 v_TexCoord;
                out float v_TexIndex;
                void main() {
                    v_Color = a_Color;
                    v_TexCoord = a_TexCoord;
                    v_TexIndex = a_TexIndex;
                    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
                }
            )";

            const std::string quadFragSrc = R"(
                #version 410 core
                layout(location = 0) out vec4 color;
                in vec4 v_Color;
                in vec2 v_TexCoord;
                in float v_TexIndex;
                uniform sampler2D u_Textures[32];
                void main() {
                    int index = int(v_TexIndex);
                    vec4 texColor = texture(u_Textures[index], v_TexCoord);
                    color = texColor * v_Color;
                }
            )";

            quads.Shader = MakeRef<ShaderProgram>();
            quads.Shader->AttachShader(ShaderUnit(ShaderStage::Vertex, quadVertSrc));
            quads.Shader->AttachShader(ShaderUnit(ShaderStage::Fragment, quadFragSrc));
            quads.Shader->Link();

            quads.Shader->Bind();
            int samplers[32];
            for (int i = 0; i < 32; i++) samplers[i] = i;
            glUniform1iv(glGetUniformLocation(quads.Shader->ID(), "u_Textures"), 32, samplers);

            quads.VertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
            quads.VertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
            quads.VertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
            quads.VertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

            quads.TexCoords[0] = {0.0f, 0.0f};
            quads.TexCoords[1] = {1.0f, 0.0f};
            quads.TexCoords[2] = {1.0f, 1.0f};
            quads.TexCoords[3] = {0.0f, 1.0f};

            quads.VertexBufferBase = new QuadVertex[QuadData::MaxVertices];
        }

        // =============== LINES ===============
        {
            auto &lines = s_Data.Lines;

            lines.VertexArray2D = MakeRef<VertexArray>();
            lines.VertexBuffer2D = MakeRef<VertexBuffer>(BufferConfig::Dynamic());
            lines.VertexBuffer2D->SetEmpty(LineData::MaxVertices, sizeof(LineVertex));

            const VertexBufferLayout layout({
                VertexAttributeDescription::Vec3(0, offsetof(LineVertex, Position)),
                VertexAttributeDescription::Vec4(1, offsetof(LineVertex, Color)),
            });

            lines.VertexArray2D->AddVertexBuffer(lines.VertexBuffer2D, layout);

            const std::string lineVertSrc = R"(
                #version 410 core
                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;
                uniform mat4 u_ViewProjection;
                out vec4 v_Color;
                void main() {
                    v_Color = a_Color;
                    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
                }
            )";

            const std::string lineFragSrc = R"(
                #version 410 core
                layout(location = 0) out vec4 color;
                in vec4 v_Color;
                void main() {
                    color = v_Color;
                }
            )";

            lines.Shader = MakeRef<ShaderProgram>();
            lines.Shader->AttachShader(ShaderUnit(ShaderStage::Vertex, lineVertSrc));
            lines.Shader->AttachShader(ShaderUnit(ShaderStage::Fragment, lineFragSrc));
            lines.Shader->Link();

            lines.VertexBufferBase = new LineVertex[LineData::MaxVertices];
        }

        // =============== CIRCLES ===============
        {
            auto &circles = s_Data.Circles;

            circles.VertexArray2D = MakeRef<VertexArray>();
            circles.VertexBuffer2D = MakeRef<VertexBuffer>(BufferConfig::Dynamic());
            circles.VertexBuffer2D->SetEmpty(CircleData::MaxVertices, sizeof(CircleVertex));

            const VertexBufferLayout layout({
                VertexAttributeDescription::Vec3(0, offsetof(CircleVertex, WorldPosition)),
                VertexAttributeDescription::Vec3(1, offsetof(CircleVertex, LocalPosition)),
                VertexAttributeDescription::Vec4(1, offsetof(CircleVertex, Color)),
                VertexAttributeDescription::Float(1, offsetof(CircleVertex, Thickness)),
                VertexAttributeDescription::Float(1, offsetof(CircleVertex, Fade)),
            });

            circles.VertexArray2D->AddVertexBuffer(circles.VertexBuffer2D, layout);

            circles.IndexBuffer2D = MakeRef<IndexBuffer>();
            std::vector<uint32_t> indices(CircleData::MaxIndices);
            uint32_t offset = 0;
            for (size_t i = 0; i < CircleData::MaxIndices; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;
                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;
                offset += 4;
            }
            circles.IndexBuffer2D->SetData<uint32_t>(indices);
            circles.VertexArray2D->SetIndexBuffer(circles.IndexBuffer2D);

            const std::string circleVertSrc = R"(
                #version 410 core
                layout(location = 0) in vec3 a_WorldPosition;
                layout(location = 1) in vec3 a_LocalPosition;
                layout(location = 2) in vec4 a_Color;
                layout(location = 3) in float a_Thickness;
                layout(location = 4) in float a_Fade;
                uniform mat4 u_ViewProjection;
                out vec3 v_LocalPosition;
                out vec4 v_Color;
                out float v_Thickness;
                out float v_Fade;
                void main() {
                    v_LocalPosition = a_LocalPosition;
                    v_Color = a_Color;
                    v_Thickness = a_Thickness;
                    v_Fade = a_Fade;
                    gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
                }
            )";

            const std::string circleFragSrc = R"(
                #version 410 core
                layout(location = 0) out vec4 color;
                in vec3 v_LocalPosition;
                in vec4 v_Color;
                in float v_Thickness;
                in float v_Fade;
                void main() {
                    float dist = 1.0 - length(v_LocalPosition);
                    float circle = smoothstep(0.0, v_Fade, dist);
                    circle *= smoothstep(v_Thickness + v_Fade, v_Thickness, dist);

                    if (circle == 0.0)
                        discard;

                    color = v_Color;
                    color.a *= circle;
                }
            )";

            circles.Shader = MakeRef<ShaderProgram>();
            circles.Shader->AttachShader(ShaderUnit(ShaderStage::Vertex, circleVertSrc));
            circles.Shader->AttachShader(ShaderUnit(ShaderStage::Fragment, circleFragSrc));
            circles.Shader->Link();

            circles.VertexPositions[0] = {-1.0f, -1.0f, 0.0f, 1.0f};
            circles.VertexPositions[1] = {1.0f, -1.0f, 0.0f, 1.0f};
            circles.VertexPositions[2] = {1.0f, 1.0f, 0.0f, 1.0f};
            circles.VertexPositions[3] = {-1.0f, 1.0f, 0.0f, 1.0f};

            circles.VertexBufferBase = new CircleVertex[CircleData::MaxVertices];
        }
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.Quads.VertexBufferBase;
        delete[] s_Data.Lines.VertexBufferBase;
        delete[] s_Data.Circles.VertexBufferBase;

        s_Data.Quads.VertexArray2D.reset();
        s_Data.Quads.VertexBuffer2D.reset();
        s_Data.Quads.IndexBuffer2D.reset();
        s_Data.Quads.WhiteTexture.reset();
        s_Data.Quads.TextureSlots.fill(nullptr);
        s_Data.Quads.Shader.reset();

        s_Data.Lines.VertexArray2D.reset();
        s_Data.Lines.VertexBuffer2D.reset();
        s_Data.Lines.Shader.reset();

        s_Data.Circles.VertexArray2D.reset();
        s_Data.Circles.VertexBuffer2D.reset();
        s_Data.Circles.IndexBuffer2D.reset();
        s_Data.Circles.Shader.reset();
    }

    void Renderer2D::BeginScene(const Camera &camera) {
        s_Data.ViewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        StartBatch();
    }

    void Renderer2D::EndScene() {
        FlushQuads();
        FlushLines();
        FlushCircles();
    }

    void Renderer2D::StartBatch() {
        // Quads
        s_Data.Quads.IndexCount = 0;
        s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
        s_Data.Quads.TextureSlotIndex = 1;

        // Lines
        s_Data.Lines.VertexCount = 0;
        s_Data.Lines.VertexBufferPtr = s_Data.Lines.VertexBufferBase;

        // Circles
        s_Data.Circles.IndexCount = 0;
        s_Data.Circles.VertexBufferPtr = s_Data.Circles.VertexBufferBase;
    }

    void Renderer2D::NextBatch() {
        FlushQuads();
        FlushLines();
        FlushCircles();
        StartBatch();
    }

    void Renderer2D::FlushQuads() {
        if (s_Data.Quads.IndexCount == 0)
            return;

        const auto dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t *>(s_Data.Quads.VertexBufferPtr) -
            reinterpret_cast<uint8_t *>(s_Data.Quads.VertexBufferBase));
        s_Data.Quads.VertexBuffer2D->Update<QuadVertex>(
            std::span(s_Data.Quads.VertexBufferBase, dataSize / sizeof(QuadVertex))
        );

        for (uint32_t i = 0; i < s_Data.Quads.TextureSlotIndex; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            s_Data.Quads.TextureSlots[i]->Bind();
        }

        s_Data.Quads.Shader->Bind();
        s_Data.Quads.Shader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        Renderer::DrawIndexed(*s_Data.Quads.VertexArray2D, s_Data.Quads.IndexCount);

        s_Data.Quads.IndexCount = 0;
        s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
        s_Data.Quads.TextureSlotIndex = 1;
    }

    void Renderer2D::FlushLines() {
        if (s_Data.Lines.VertexCount == 0)
            return;

        const auto dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t *>(s_Data.Lines.VertexBufferPtr) -
            reinterpret_cast<uint8_t *>(s_Data.Lines.VertexBufferBase));
        s_Data.Lines.VertexBuffer2D->Update<LineVertex>(
            std::span(s_Data.Lines.VertexBufferBase, dataSize / sizeof(LineVertex))
        );

        s_Data.Lines.Shader->Bind();
        s_Data.Lines.Shader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

        glLineWidth(s_Data.LineWidth);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(s_Data.Lines.VertexCount));

        s_Data.Lines.VertexCount = 0;
        s_Data.Lines.VertexBufferPtr = s_Data.Lines.VertexBufferBase;
    }

    void Renderer2D::FlushCircles() {
        if (s_Data.Circles.IndexCount == 0)
            return;

        const auto dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t *>(s_Data.Circles.VertexBufferPtr) -
            reinterpret_cast<uint8_t *>(s_Data.Circles.VertexBufferBase));
        s_Data.Circles.VertexBuffer2D->Update<CircleVertex>(
            std::span(s_Data.Circles.VertexBufferBase, dataSize / sizeof(CircleVertex))
        );

        s_Data.Circles.Shader->Bind();
        s_Data.Circles.Shader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        Renderer::DrawIndexed(*s_Data.Circles.VertexArray2D, s_Data.Circles.IndexCount);

        s_Data.Circles.IndexCount = 0;
        s_Data.Circles.VertexBufferPtr = s_Data.Circles.VertexBufferBase;
    }

    // =============== QUADS ===============

    void Renderer2D::DrawQuad(const Vec2 &position, const Vec2 &size, const Vec4 &color) {
        DrawQuad(Vec3(position.x, position.y, 0.0f), size, color);
    }

    void Renderer2D::DrawQuad(const Vec3 &position, const Vec2 &size, const Vec4 &color) {
        const Mat4 transform = glm::translate(Mat4(1.0f), position)
                               * glm::scale(Mat4(1.0f), Vec3(size.x, size.y, 1.0f));
        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const Mat4 &transform, const Vec4 &color) {
        constexpr size_t quadVertexCount = 4;
        constexpr float textureIndex = 0.0f;

        if (s_Data.Quads.IndexCount >= QuadData::MaxIndices) {
            FlushQuads();
            s_Data.Quads.IndexCount = 0;
            s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
            s_Data.Quads.TextureSlotIndex = 1;
        }

        for (size_t i = 0; i < quadVertexCount; i++) {
            s_Data.Quads.VertexBufferPtr->Position = transform * s_Data.Quads.VertexPositions[i];
            s_Data.Quads.VertexBufferPtr->Color = color;
            s_Data.Quads.VertexBufferPtr->TexCoord = s_Data.Quads.TexCoords[i];
            s_Data.Quads.VertexBufferPtr->TexIndex = textureIndex;
            s_Data.Quads.VertexBufferPtr++;
        }

        s_Data.Quads.IndexCount += 6;
    }

    void Renderer2D::DrawQuad(const Vec2 &position, const Vec2 &size,
                              const Ref<Texture2D> &texture,
                              const Vec4 &tintColor) {
        DrawQuad(Vec3(position.x, position.y, 0.0f), size, texture, tintColor);
    }

    void Renderer2D::DrawQuad(const Vec3 &position, const Vec2 &size,
                              const Ref<Texture2D> &texture,
                              const Vec4 &tintColor) {
        const Mat4 transform = glm::translate(Mat4(1.0f), position)
                               * glm::scale(Mat4(1.0f), Vec3(size.x, size.y, 1.0f));
        DrawQuad(transform, texture, tintColor);
    }

    void Renderer2D::DrawQuad(const Mat4 &transform,
                              const Ref<Texture2D> &texture,
                              const Vec4 &tintColor) {
        constexpr size_t quadVertexCount = 4;

        if (s_Data.Quads.IndexCount >= QuadData::MaxIndices) {
            FlushQuads();
            s_Data.Quads.IndexCount = 0;
            s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
            s_Data.Quads.TextureSlotIndex = 1;
        }

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.Quads.TextureSlotIndex; i++) {
            if (s_Data.Quads.TextureSlots[i]->ID() == texture->ID()) {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f) {
            if (s_Data.Quads.TextureSlotIndex >= QuadData::MaxTextureSlots) {
                FlushQuads();
                s_Data.Quads.IndexCount = 0;
                s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
                s_Data.Quads.TextureSlotIndex = 1;
            }

            textureIndex = static_cast<float>(s_Data.Quads.TextureSlotIndex);
            s_Data.Quads.TextureSlots[s_Data.Quads.TextureSlotIndex] = texture;
            s_Data.Quads.TextureSlotIndex++;
        }

        for (size_t i = 0; i < quadVertexCount; i++) {
            const Vec4 pos = transform * s_Data.Quads.VertexPositions[i];
            s_Data.Quads.VertexBufferPtr->Position = Vec3(pos.x, pos.y, pos.z);
            s_Data.Quads.VertexBufferPtr->Color = tintColor;
            s_Data.Quads.VertexBufferPtr->TexCoord = s_Data.Quads.TexCoords[i];
            s_Data.Quads.VertexBufferPtr->TexIndex = textureIndex;
            s_Data.Quads.VertexBufferPtr++;
        }

        s_Data.Quads.IndexCount += 6;
    }

    void Renderer2D::DrawRotatedQuad(const Vec2 &position, const Vec2 &size,
                                     const float rotation, const Vec4 &color) {
        DrawRotatedQuad(Vec3(position.x, position.y, 0.0f), size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const Vec3 &position, const Vec2 &size,
                                     const float rotation, const Vec4 &color) {
        const Mat4 transform = glm::translate(Mat4(1.0f), position)
                               * glm::rotate(Mat4(1.0f), rotation, Vec3(0.0f, 0.0f, 1.0f))
                               * glm::scale(Mat4(1.0f), Vec3(size.x, size.y, 1.0f));
        DrawQuad(transform, color);
    }

    void Renderer2D::DrawRotatedQuad(const Vec2 &position, const Vec2 &size,
                                     const float rotation, const Ref<Texture2D> &texture,
                                     const Vec4 &tintColor) {
        DrawRotatedQuad(Vec3(position.x, position.y, 0.0f), size, rotation, texture, tintColor);
    }

    void Renderer2D::DrawRotatedQuad(const Vec3 &position, const Vec2 &size,
                                     const float rotation, const Ref<Texture2D> &texture,
                                     const Vec4 &tintColor) {
        const Mat4 transform = glm::translate(Mat4(1.0f), position)
                         * glm::rotate(Mat4(1.0f), rotation, Vec3(0.0f, 0.0f, 1.0f))
                         * glm::scale(Mat4(1.0f), Vec3(size.x, size.y, 1.0f));
        DrawQuad(transform, texture, tintColor);
    }

    // =============== LINES ===============

    void Renderer2D::DrawLine(const Vec2 &p0, const Vec2 &p1, const Vec4 &color) {
        DrawLine(Vec3(p0.x, p0.y, 0.0f), Vec3(p1.x, p1.y, 0.0f), color);
    }

    void Renderer2D::DrawLine(const Vec3 &p0, const Vec3 &p1, const Vec4 &color) {
        if (s_Data.Lines.VertexCount >= LineData::MaxVertices) {
            FlushLines();
            s_Data.Lines.VertexCount = 0;
            s_Data.Lines.VertexBufferPtr = s_Data.Lines.VertexBufferBase;
        }

        s_Data.Lines.VertexBufferPtr->Position = p0;
        s_Data.Lines.VertexBufferPtr->Color = color;
        s_Data.Lines.VertexBufferPtr++;

        s_Data.Lines.VertexBufferPtr->Position = p1;
        s_Data.Lines.VertexBufferPtr->Color = color;
        s_Data.Lines.VertexBufferPtr++;

        s_Data.Lines.VertexCount += 2;
    }

    void Renderer2D::DrawRect(const Vec2 &position, const Vec2 &size, const Vec4 &color) {
        DrawRect(Vec3(position.x, position.y, 0.0f), size, color);
    }

    void Renderer2D::DrawRect(const Vec3 &position, const Vec2 &size, const Vec4 &color) {
        const Mat4 transform = glm::translate(Mat4(1.0f), position)
                               * glm::scale(Mat4(1.0f), Vec3(size.x, size.y, 1.0f));
        DrawRect(transform, color);
    }

    void Renderer2D::DrawRect(const Mat4 &transform, const Vec4 &color) {
        Vec3 corners[4];
        for (int i = 0; i < 4; i++) {
            const Vec4 pos = transform * s_Data.Quads.VertexPositions[i];
            corners[i] = Vec3(pos.x, pos.y, pos.z);
        }

        DrawLine(corners[0], corners[1], color);
        DrawLine(corners[1], corners[2], color);
        DrawLine(corners[2], corners[3], color);
        DrawLine(corners[3], corners[0], color);
    }

    // =============== CIRCLES ===============

    void Renderer2D::DrawCircle(const Vec2 &center, const float radius, const Vec4 &color,
                                const float thickness, const float fade) {
        DrawCircle(Vec3(center.x, center.y, 0.0f), radius, color, thickness, fade);
    }

    void Renderer2D::DrawCircle(const Vec3 &center, const float radius, const Vec4 &color,
                                const float thickness, const float fade) {
        const Mat4 transform = glm::translate(Mat4(1.0f), center)
                               * glm::scale(Mat4(1.0f), Vec3(radius * 2.0f));
        DrawCircle(transform, color, thickness, fade);
    }

    void Renderer2D::DrawCircle(const Mat4 &transform, const Vec4 &color,
                                const float thickness, const float fade) {
        constexpr size_t circleVertexCount = 4;

        if (s_Data.Circles.IndexCount >= CircleData::MaxIndices) {
            FlushCircles();
            s_Data.Circles.IndexCount = 0;
            s_Data.Circles.VertexBufferPtr = s_Data.Circles.VertexBufferBase;
        }

        for (size_t i = 0; i < circleVertexCount; i++) {
            const Vec4 worldPos = transform * s_Data.Circles.VertexPositions[i];
            s_Data.Circles.VertexBufferPtr->WorldPosition = Vec3(worldPos.x, worldPos.y, worldPos.z);
            s_Data.Circles.VertexBufferPtr->LocalPosition = Vec3(s_Data.Circles.VertexPositions[i]);
            s_Data.Circles.VertexBufferPtr->Color = color;
            s_Data.Circles.VertexBufferPtr->Thickness = thickness;
            s_Data.Circles.VertexBufferPtr->Fade = fade;
            s_Data.Circles.VertexBufferPtr++;
        }

        s_Data.Circles.IndexCount += 6;
    }

    void Renderer2D::DrawFilledCircle(const Vec2 &center, const float radius, const Vec4 &color) {
        DrawFilledCircle(Vec3(center.x, center.y, 0.0f), radius, color);
    }

    void Renderer2D::DrawFilledCircle(const Vec3 &center, const float radius, const Vec4 &color) {
        // Cercle rempli = cercle avec thickness = 1.0
        DrawCircle(center, radius, color, 1.0f, 0.005f);
    }

    // =============== POLYGONS ===============

    void Renderer2D::DrawPolygon(const std::vector<Vec2> &points, const Vec4 &color) {
        std::vector<Vec3> points3d;
        points3d.reserve(points.size());
        for (const auto &p: points)
            points3d.emplace_back(p.x, p.y, 0.0f);

        DrawPolygon(points3d, color);
    }

    void Renderer2D::DrawPolygon(const std::vector<Vec3> &points, const Vec4 &color) {
        if (points.size() < 3) return;

        // Triangulation simple en éventail depuis le premier point
        for (size_t i = 1; i < points.size() - 1; i++) {
            // Créer un quad dégénéré pour chaque triangle
            if (s_Data.Quads.IndexCount >= QuadData::MaxIndices) {
                FlushQuads();
                s_Data.Quads.IndexCount = 0;
                s_Data.Quads.VertexBufferPtr = s_Data.Quads.VertexBufferBase;
                s_Data.Quads.TextureSlotIndex = 1;
            }

            // Triangle: points[0], points[i], points[i+1]
            s_Data.Quads.VertexBufferPtr->Position = points[0];
            s_Data.Quads.VertexBufferPtr->Color = color;
            s_Data.Quads.VertexBufferPtr->TexCoord = {0.0f, 0.0f};
            s_Data.Quads.VertexBufferPtr->TexIndex = 0.0f;
            s_Data.Quads.VertexBufferPtr++;

            s_Data.Quads.VertexBufferPtr->Position = points[i];
            s_Data.Quads.VertexBufferPtr->Color = color;
            s_Data.Quads.VertexBufferPtr->TexCoord = {0.0f, 0.0f};
            s_Data.Quads.VertexBufferPtr->TexIndex = 0.0f;
            s_Data.Quads.VertexBufferPtr++;

            s_Data.Quads.VertexBufferPtr->Position = points[i + 1];
            s_Data.Quads.VertexBufferPtr->Color = color;
            s_Data.Quads.VertexBufferPtr->TexCoord = {0.0f, 0.0f};
            s_Data.Quads.VertexBufferPtr->TexIndex = 0.0f;
            s_Data.Quads.VertexBufferPtr++;

            // Point dupliqué pour compléter le quad
            s_Data.Quads.VertexBufferPtr->Position = points[i + 1];
            s_Data.Quads.VertexBufferPtr->Color = color;
            s_Data.Quads.VertexBufferPtr->TexCoord = {0.0f, 0.0f};
            s_Data.Quads.VertexBufferPtr->TexIndex = 0.0f;
            s_Data.Quads.VertexBufferPtr++;

            s_Data.Quads.IndexCount += 6;
        }
    }

    void Renderer2D::DrawPolygonOutline(const std::vector<Vec2> &points, const Vec4 &color) {
        std::vector<Vec3> points3d;
        points3d.reserve(points.size());
        for (const auto &p: points)
            points3d.emplace_back(p.x, p.y, 0.0f);

        DrawPolygonOutline(points3d, color);
    }

    void Renderer2D::DrawPolygonOutline(const std::vector<Vec3> &points, const Vec4 &color) {
        if (points.size() < 2) return;

        for (size_t i = 0; i < points.size(); i++) {
            const size_t next = (i + 1) % points.size();
            DrawLine(points[i], points[next], color);
        }
    }

    // =============== UTILITY ===============

    void Renderer2D::SetLineWidth(const float width) {
        s_Data.LineWidth = width;
    }
}
