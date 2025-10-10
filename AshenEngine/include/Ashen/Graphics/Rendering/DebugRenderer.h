#ifndef ASHEN_DEBUGRENDERER_H
#define ASHEN_DEBUGRENDERER_H

#include <memory>
#include <vector>
#include "Ashen/Math/Math.h"

namespace ash {
    class Camera;
    class ShaderProgram;
    class VertexArray;
    class VertexBuffer;

    /**
     * @brief Simple debug renderer for lines, boxes, spheres, etc.
     * Useful for visualizing physics, bounding boxes, normals, etc.
     */
    class DebugRenderer {
    public:
        static void Init();
        static void Shutdown();

        // Must be called before drawing
        static void Begin(const Camera& camera);
        static void End();

        // Line drawing
        static void DrawLine(const Vec3& from, const Vec3& to, const Vec3& color = Vec3(1.0f));
        static void DrawRay(const Vec3& origin, const Vec3& direction, float length = 1.0f, const Vec3& color = Vec3(1.0f));

        // Box drawing
        static void DrawBox(const Vec3& min, const Vec3& max, const Vec3& color = Vec3(1.0f));
        static void DrawWireCube(const Vec3& center, const Vec3& size, const Vec3& color = Vec3(1.0f));
        static void DrawWireCube(const Mat4& transform, const Vec3& color = Vec3(1.0f));

        // Sphere drawing
        static void DrawWireSphere(const Vec3& center, float radius, const Vec3& color = Vec3(1.0f), int segments = 16);

        // Coordinate system
        static void DrawAxes(const Vec3& origin, float length = 1.0f);
        static void DrawGrid(const Vec3& center, float size, int divisions, const Vec3& color = Vec3(0.5f));

        // Frustum
        static void DrawFrustum(const Mat4& viewProjection, const Vec3& color = Vec3(1.0f));

        // Utilities
        static void SetLineWidth(float width);
        static void Clear();

    private:
        struct DebugLine {
            Vec3 start;
            Vec3 color1;
            Vec3 end;
            Vec3 color2;
        };

        struct DebugData;
        static std::unique_ptr<DebugData> s_Data;

        static void Flush();
        static void AddLine(const Vec3& start, const Vec3& end, const Vec3& color1, const Vec3& color2);
    };
}

#endif // ASHEN_DEBUGRENDERER_H