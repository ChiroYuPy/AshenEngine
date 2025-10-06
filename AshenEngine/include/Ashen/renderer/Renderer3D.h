#ifndef ASHEN_RENDERER3D_H
#define ASHEN_RENDERER3D_H

#include <vector>
#include <glm/glm.hpp>
#include "Ashen/core/Types.h"
#include "Ashen/math/Math.h"

namespace ash {
    struct Line3D {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec4 color;
    };

    class Camera;

    class Renderer3D {
    public:
        static void Init();

        static void Shutdown();

        static void BeginScene(const Camera &camera);

        static void EndScene();

        static void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color);

    private:
        static void FlushLines();

        static std::vector<Line3D> s_Lines;
        static glm::mat4 s_ViewProjection;

        static uint32_t s_VAO, s_VBO;
        static uint32_t s_ShaderProgram;
    };
}

#endif //ASHEN_RENDERER3D_H