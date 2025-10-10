#include "Ashen/Graphics/Rendering/DebugRenderer.h"

#include <glad/glad.h>
#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/GraphicsAPI/VertexArray.h"

namespace ash {

    struct DebugRenderer::DebugData {
        std::vector<DebugLine> lines;

        std::shared_ptr<VertexArray> vao;
        std::shared_ptr<VertexBuffer> vbo;
        std::shared_ptr<ShaderProgram> shader;

        Mat4 viewProjection;
        float lineWidth = 2.0f;

        static constexpr size_t MaxLines = 10000;
    };

    std::unique_ptr<DebugRenderer::DebugData> DebugRenderer::s_Data = nullptr;

    void DebugRenderer::Init() {
        s_Data = std::make_unique<DebugData>();
        s_Data->lines.reserve(DebugData::MaxLines);

        // Create shader
        const std::string vertSource = R"(
            #version 410 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;

            uniform mat4 u_ViewProjection;

            out vec3 v_Color;

            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        const std::string fragSource = R"(
            #version 410 core
            layout(location = 0) out vec4 FragColor;

            in vec3 v_Color;

            void main() {
                FragColor = vec4(v_Color, 1.0);
            }
        )";

        s_Data->shader = std::make_shared<ShaderProgram>();
        s_Data->shader->AttachShader(ShaderUnit(ShaderStage::Vertex, vertSource));
        s_Data->shader->AttachShader(ShaderUnit(ShaderStage::Fragment, fragSource));
        s_Data->shader->Link();

        // Create buffers
        s_Data->vbo = std::make_shared<VertexBuffer>(BufferConfig::Dynamic());
        s_Data->vbo->SetEmpty(DebugData::MaxLines * 2, sizeof(Vec3) * 2); // position + color per vertex

        VertexBufferLayout layout;
        layout.AddAttribute(VertexAttributeDescription::Vec3(0, 0));  // position
        layout.AddAttribute(VertexAttributeDescription::Vec3(1, sizeof(Vec3))); // color
        layout.SetStride(sizeof(Vec3) * 2);

        s_Data->vao = std::make_shared<VertexArray>();
        s_Data->vao->AddVertexBuffer(s_Data->vbo, layout);

        Logger::Info("DebugRenderer initialized");
    }

    void DebugRenderer::Shutdown() {
        if (s_Data) {
            s_Data.reset();
        }
        Logger::Info("DebugRenderer shutdown");
    }

    void DebugRenderer::Begin(const Camera& camera) {
        s_Data->viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        s_Data->lines.clear();
    }

    void DebugRenderer::End() {
        Flush();
    }

    void DebugRenderer::DrawLine(const Vec3& from, const Vec3& to, const Vec3& color) {
        AddLine(from, to, color, color);
    }

    void DebugRenderer::DrawRay(const Vec3& origin, const Vec3& direction, float length, const Vec3& color) {
        Vec3 end = origin + glm::normalize(direction) * length;
        DrawLine(origin, end, color);
    }

    void DebugRenderer::DrawBox(const Vec3& min, const Vec3& max, const Vec3& color) {
        // Bottom face
        DrawLine(Vec3(min.x, min.y, min.z), Vec3(max.x, min.y, min.z), color);
        DrawLine(Vec3(max.x, min.y, min.z), Vec3(max.x, min.y, max.z), color);
        DrawLine(Vec3(max.x, min.y, max.z), Vec3(min.x, min.y, max.z), color);
        DrawLine(Vec3(min.x, min.y, max.z), Vec3(min.x, min.y, min.z), color);

        // Top face
        DrawLine(Vec3(min.x, max.y, min.z), Vec3(max.x, max.y, min.z), color);
        DrawLine(Vec3(max.x, max.y, min.z), Vec3(max.x, max.y, max.z), color);
        DrawLine(Vec3(max.x, max.y, max.z), Vec3(min.x, max.y, max.z), color);
        DrawLine(Vec3(min.x, max.y, max.z), Vec3(min.x, max.y, min.z), color);

        // Vertical edges
        DrawLine(Vec3(min.x, min.y, min.z), Vec3(min.x, max.y, min.z), color);
        DrawLine(Vec3(max.x, min.y, min.z), Vec3(max.x, max.y, min.z), color);
        DrawLine(Vec3(max.x, min.y, max.z), Vec3(max.x, max.y, max.z), color);
        DrawLine(Vec3(min.x, min.y, max.z), Vec3(min.x, max.y, max.z), color);
    }

    void DebugRenderer::DrawWireCube(const Vec3& center, const Vec3& size, const Vec3& color) {
        Vec3 halfSize = size * 0.5f;
        DrawBox(center - halfSize, center + halfSize, color);
    }

    void DebugRenderer::DrawWireCube(const Mat4& transform, const Vec3& color) {
        // Define cube corners in local space
        Vec3 corners[8] = {
            Vec3(-0.5f, -0.5f, -0.5f), Vec3( 0.5f, -0.5f, -0.5f),
            Vec3( 0.5f, -0.5f,  0.5f), Vec3(-0.5f, -0.5f,  0.5f),
            Vec3(-0.5f,  0.5f, -0.5f), Vec3( 0.5f,  0.5f, -0.5f),
            Vec3( 0.5f,  0.5f,  0.5f), Vec3(-0.5f,  0.5f,  0.5f)
        };

        // Transform corners to world space
        Vec3 worldCorners[8];
        for (int i = 0; i < 8; ++i) {
            Vec4 worldPos = transform * Vec4(corners[i], 1.0f);
            worldCorners[i] = Vec3(worldPos) / worldPos.w;
        }

        // Draw edges
        // Bottom face
        DrawLine(worldCorners[0], worldCorners[1], color);
        DrawLine(worldCorners[1], worldCorners[2], color);
        DrawLine(worldCorners[2], worldCorners[3], color);
        DrawLine(worldCorners[3], worldCorners[0], color);

        // Top face
        DrawLine(worldCorners[4], worldCorners[5], color);
        DrawLine(worldCorners[5], worldCorners[6], color);
        DrawLine(worldCorners[6], worldCorners[7], color);
        DrawLine(worldCorners[7], worldCorners[4], color);

        // Vertical edges
        DrawLine(worldCorners[0], worldCorners[4], color);
        DrawLine(worldCorners[1], worldCorners[5], color);
        DrawLine(worldCorners[2], worldCorners[6], color);
        DrawLine(worldCorners[3], worldCorners[7], color);
    }

    void DebugRenderer::DrawWireSphere(const Vec3& center, float radius, const Vec3& color, int segments) {
        float angleStep = glm::two_pi<float>() / segments;

        // Draw 3 circles (XY, XZ, YZ planes)
        for (int i = 0; i < segments; ++i) {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;

            // XY plane
            Vec3 p1 = center + Vec3(cos(angle1) * radius, sin(angle1) * radius, 0);
            Vec3 p2 = center + Vec3(cos(angle2) * radius, sin(angle2) * radius, 0);
            DrawLine(p1, p2, color);

            // XZ plane
            p1 = center + Vec3(cos(angle1) * radius, 0, sin(angle1) * radius);
            p2 = center + Vec3(cos(angle2) * radius, 0, sin(angle2) * radius);
            DrawLine(p1, p2, color);

            // YZ plane
            p1 = center + Vec3(0, cos(angle1) * radius, sin(angle1) * radius);
            p2 = center + Vec3(0, cos(angle2) * radius, sin(angle2) * radius);
            DrawLine(p1, p2, color);
        }
    }

    void DebugRenderer::DrawAxes(const Vec3& origin, float length) {
        DrawLine(origin, origin + Vec3(length, 0, 0), Vec3(1, 0, 0)); // X - Red
        DrawLine(origin, origin + Vec3(0, length, 0), Vec3(0, 1, 0)); // Y - Green
        DrawLine(origin, origin + Vec3(0, 0, length), Vec3(0, 0, 1)); // Z - Blue
    }

    void DebugRenderer::DrawGrid(const Vec3& center, float size, int divisions, const Vec3& color) {
        float step = size / divisions;
        float halfSize = size * 0.5f;

        // Lines parallel to X axis
        for (int i = 0; i <= divisions; ++i) {
            float z = -halfSize + i * step;
            DrawLine(
                center + Vec3(-halfSize, 0, z),
                center + Vec3( halfSize, 0, z),
                color
            );
        }

        // Lines parallel to Z axis
        for (int i = 0; i <= divisions; ++i) {
            float x = -halfSize + i * step;
            DrawLine(
                center + Vec3(x, 0, -halfSize),
                center + Vec3(x, 0,  halfSize),
                color
            );
        }
    }

    void DebugRenderer::DrawFrustum(const Mat4& viewProjection, const Vec3& color) {
        // Extract frustum corners from inverse view-projection matrix
        Mat4 invVP = glm::inverse(viewProjection);

        Vec3 corners[8];
        int index = 0;
        for (int z = 0; z < 2; ++z) {
            for (int y = 0; y < 2; ++y) {
                for (int x = 0; x < 2; ++x) {
                    Vec4 point = invVP * Vec4(
                        x * 2.0f - 1.0f,
                        y * 2.0f - 1.0f,
                        z * 2.0f - 1.0f,
                        1.0f
                    );
                    corners[index++] = Vec3(point) / point.w;
                }
            }
        }

        // Draw near plane
        DrawLine(corners[0], corners[1], color);
        DrawLine(corners[1], corners[3], color);
        DrawLine(corners[3], corners[2], color);
        DrawLine(corners[2], corners[0], color);

        // Draw far plane
        DrawLine(corners[4], corners[5], color);
        DrawLine(corners[5], corners[7], color);
        DrawLine(corners[7], corners[6], color);
        DrawLine(corners[6], corners[4], color);

        // Draw connecting lines
        DrawLine(corners[0], corners[4], color);
        DrawLine(corners[1], corners[5], color);
        DrawLine(corners[2], corners[6], color);
        DrawLine(corners[3], corners[7], color);
    }

    void DebugRenderer::SetLineWidth(float width) {
        s_Data->lineWidth = width;
    }

    void DebugRenderer::Clear() {
        s_Data->lines.clear();
    }

    void DebugRenderer::Flush() {
        if (s_Data->lines.empty()) return;

        // Prepare vertex data
        std::vector<float> vertices;
        vertices.reserve(s_Data->lines.size() * 12); // 2 vertices * 6 floats per line

        for (const auto& line : s_Data->lines) {
            // Start vertex
            vertices.push_back(line.start.x);
            vertices.push_back(line.start.y);
            vertices.push_back(line.start.z);
            vertices.push_back(line.color1.x);
            vertices.push_back(line.color1.y);
            vertices.push_back(line.color1.z);

            // End vertex
            vertices.push_back(line.end.x);
            vertices.push_back(line.end.y);
            vertices.push_back(line.end.z);
            vertices.push_back(line.color2.x);
            vertices.push_back(line.color2.y);
            vertices.push_back(line.color2.z);
        }

        // Upload to GPU
        s_Data->vbo->Update<float>(std::span(vertices.data(), vertices.size()));

        // Draw
        s_Data->shader->Bind();
        s_Data->shader->SetMat4("u_ViewProjection", s_Data->viewProjection);

        glLineWidth(s_Data->lineWidth);
        s_Data->vao->Bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(s_Data->lines.size() * 2));

        s_Data->lines.clear();
    }

    void DebugRenderer::AddLine(const Vec3& start, const Vec3& end, const Vec3& color1, const Vec3& color2) {
        if (s_Data->lines.size() >= DebugData::MaxLines) {
            Logger::Warn("DebugRenderer: Maximum line count reached!");
            return;
        }

        DebugLine line;
        line.start = start;
        line.color1 = color1;
        line.end = end;
        line.color2 = color2;
        s_Data->lines.push_back(line);
    }
}