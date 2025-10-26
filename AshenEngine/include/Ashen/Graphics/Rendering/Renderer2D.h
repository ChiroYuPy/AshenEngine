#ifndef ASHEN_RENDERER2D_H
#define ASHEN_RENDERER2D_H

#include <array>

#include <glm/glm.hpp>

#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    struct QuadVertex {
        Vec3 Position;
        Vec4 Color;
        Vec2 TexCoord;
        float TexIndex;
    };

    struct LineVertex {
        Vec3 Position;
        Vec4 Color;
    };

    struct CircleVertex {
        Vec3 WorldPosition;
        Vec3 LocalPosition;
        Vec4 Color;
        float Thickness;
        float Fade;
    };

    class Camera;
    class Texture2D;
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class ShaderProgram;

    class Renderer2D {
    public:
        static void Init();

        static void Shutdown();

        static void BeginScene(const Camera &camera);

        static void EndScene();

        static void DrawQuad(const Vec2 &position, const Vec2 &size, const Vec4 &color);

        static void DrawQuad(const Vec3 &position, const Vec2 &size, const Vec4 &color);

        static void DrawQuad(const Mat4 &transform, const Vec4 &color);

        static void DrawQuad(const Vec2 &position, const Vec2 &size,
                             const Ref<Texture2D> &texture,
                             const Vec4 &tintColor = Vec4(1.0f));

        static void DrawQuad(const Vec3 &position, const Vec2 &size,
                             const Ref<Texture2D> &texture,
                             const Vec4 &tintColor = Vec4(1.0f));

        static void DrawQuad(const Mat4 &transform,
                             const Ref<Texture2D> &texture,
                             const Vec4 &tintColor = Vec4(1.0f));

        static void DrawRotatedQuad(const Vec2 &position, const Vec2 &size,
                                    float rotation, const Vec4 &color);

        static void DrawRotatedQuad(const Vec3 &position, const Vec2 &size,
                                    float rotation, const Vec4 &color);

        static void DrawRotatedQuad(const Vec2 &position, const Vec2 &size,
                                    float rotation,
                                    const Ref<Texture2D> &texture,
                                    const Vec4 &tintColor = Vec4(1.0f));

        static void DrawRotatedQuad(const Vec3 &position, const Vec2 &size,
                                    float rotation,
                                    const Ref<Texture2D> &texture,
                                    const Vec4 &tintColor = Vec4(1.0f));

        static void DrawLine(const Vec2 &p0, const Vec2 &p1, const Vec4 &color);

        static void DrawLine(const Vec3 &p0, const Vec3 &p1, const Vec4 &color);

        static void DrawRect(const Vec2 &position, const Vec2 &size, const Vec4 &color);

        static void DrawRect(const Vec3 &position, const Vec2 &size, const Vec4 &color);

        static void DrawRect(const Mat4 &transform, const Vec4 &color);

        static void DrawCircle(const Vec2 &center, float radius, const Vec4 &color,
                               float thickness = 1.0f, float fade = 0.005f);

        static void DrawCircle(const Vec3 &center, float radius, const Vec4 &color,
                               float thickness = 1.0f, float fade = 0.005f);

        static void DrawCircle(const Mat4 &transform, const Vec4 &color,
                               float thickness = 1.0f, float fade = 0.005f);

        static void DrawFilledCircle(const Vec2 &center, float radius, const Vec4 &color);

        static void DrawFilledCircle(const Vec3 &center, float radius, const Vec4 &color);

        static void DrawPolygon(const Vector<Vec2> &points, const Vec4 &color);

        static void DrawPolygon(const Vector<Vec3> &points, const Vec4 &color);

        static void DrawPolygonOutline(const Vector<Vec2> &points, const Vec4 &color);

        static void DrawPolygonOutline(const Vector<Vec3> &points, const Vec4 &color);

        static void SetLineWidth(float width);

    private:
        struct QuadData {
            static constexpr uint32_t MaxQuads = 10000;
            static constexpr uint32_t MaxVertices = MaxQuads * 4;
            static constexpr uint32_t MaxIndices = MaxQuads * 6;
            static constexpr uint32_t MaxTextureSlots = 32;

            Ref<VertexArray> VertexArray2D;
            Ref<VertexBuffer> VertexBuffer2D;
            Ref<IndexBuffer> IndexBuffer2D;

            QuadVertex *VertexBufferBase = nullptr;
            QuadVertex *VertexBufferPtr = nullptr;
            uint32_t IndexCount = 0;

            Ref<Texture2D> WhiteTexture;
            std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots{};
            uint32_t TextureSlotIndex = 1;

            Ref<ShaderProgram> Shader;

            std::array<Vec4, 4> VertexPositions;
            std::array<Vec2, 4> TexCoords;
        };

        struct LineData {
            static constexpr uint32_t MaxLines = 10000;
            static constexpr uint32_t MaxVertices = MaxLines * 2;

            Ref<VertexArray> VertexArray2D;
            Ref<VertexBuffer> VertexBuffer2D;

            LineVertex *VertexBufferBase = nullptr;
            LineVertex *VertexBufferPtr = nullptr;
            uint32_t VertexCount = 0;

            Ref<ShaderProgram> Shader;
        };

        struct CircleData {
            static constexpr uint32_t MaxCircles = 5000;
            static constexpr uint32_t MaxVertices = MaxCircles * 4;
            static constexpr uint32_t MaxIndices = MaxCircles * 6;

            Ref<VertexArray> VertexArray2D;
            Ref<VertexBuffer> VertexBuffer2D;
            Ref<IndexBuffer> IndexBuffer2D;

            CircleVertex *VertexBufferBase = nullptr;
            CircleVertex *VertexBufferPtr = nullptr;
            uint32_t IndexCount = 0;

            Ref<ShaderProgram> Shader;

            std::array<Vec4, 4> VertexPositions;
        };

        struct RenderData {
            QuadData Quads;
            LineData Lines;
            CircleData Circles;
            Mat4 ViewProjectionMatrix{};
            float LineWidth = 2.0f;
        };

        static void StartBatch();

        static void NextBatch();

        static void FlushQuads();

        static void FlushLines();

        static void FlushCircles();

        static RenderData s_Data;
    };
}

#endif //ASHEN_RENDERER2D_H