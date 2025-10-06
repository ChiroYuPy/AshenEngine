#ifndef ASHEN_RENDERER_H
#define ASHEN_RENDERER_H

#include "Ashen/math/Math.h"

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

        static void DrawIndexed(const VertexArray &vao, uint32_t indexCount);

        static void DrawArrays(const VertexArray &vao, uint32_t vertexCount, uint32_t first = 0);

        static void DrawIndexedInstanced(const VertexArray &vao, uint32_t indexCount, uint32_t instanceCount);

        static void DrawArraysInstanced(const VertexArray &vao, uint32_t vertexCount, uint32_t instanceCount,
                                        uint32_t first = 0);

        [[nodiscard]] static const Statistics &GetStats() { return s_Stats; }

        static void ResetStats();

    private:
        Renderer() = delete;

        static Statistics s_Stats;
    };
}

#endif //ASHEN_RENDERER_H