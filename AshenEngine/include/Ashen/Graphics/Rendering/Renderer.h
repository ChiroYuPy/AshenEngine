#ifndef ASHEN_RENDERER_H
#define ASHEN_RENDERER_H

#include "Ashen/Math/Math.h"

namespace ash {
    class VertexArray;
    class IndexBuffer;
    class ShaderProgram;
    class Camera;

    class Renderer {
    public:
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t Vertices = 0;
            uint32_t Indices = 0;
            uint32_t Triangles = 0;

            void Reset() {
                DrawCalls = 0;
                Vertices = 0;
                Indices = 0;
                Triangles = 0;
            }
        };

        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void Draw(const VertexArray &vao);

        static void DrawIndexed(const VertexArray &vao, uint32_t indexCount, uint32_t indexOffset = 0);

        static void DrawArrays(const VertexArray &vao, uint32_t vertexCount, uint32_t vertexOffset = 0);

        static void DrawInstanced(const VertexArray &vao, uint32_t instanceCount);

        static void DrawIndexedInstanced(const VertexArray &vao, uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset = 0);

        static const Statistics &GetStats() { return s_Stats; }
        static void ResetStats() { s_Stats.Reset(); }

    private:
        static Statistics s_Stats;
    };
}

#endif //ASHEN_RENDERER_H
