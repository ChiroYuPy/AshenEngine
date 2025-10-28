#ifndef ASHEN_VIEWPORT_H
#define ASHEN_VIEWPORT_H
#include "Ashen/GraphicsAPI/RenderCommand.h"

namespace ash {

    struct Viewport {
        float x = 0.0f;
        float y = 0.0f;
        float width = 1.0f;
        float height = 1.0f;
        float minDepth = 0.0f;
        float maxDepth = 1.0f;

        Viewport() = default;
        Viewport(const float x, const float y, const float w, const float h) : x(x), y(y), width(w), height(h) {}
    };

} // namespace ash

#endif //ASHEN_VIEWPORT_H