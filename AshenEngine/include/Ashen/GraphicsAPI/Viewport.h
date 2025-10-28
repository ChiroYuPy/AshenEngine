#ifndef ASHEN_VIEWPORT_H
#define ASHEN_VIEWPORT_H

#include "RenderCommand.h"

namespace ash {

    class Viewport {
    public:
        Viewport(const int x, const int y, const int width, const int height) : m_X(x), m_Y(y), m_Width(width), m_Height(height) {}

        void Apply() const {
            RenderCommand::SetViewport(m_X, m_Y, m_Width, m_Height);
        }

    private:
        int m_X, m_Y, m_Width, m_Height;
    };

} // namespace ash

#endif //ASHEN_VIEWPORT_H