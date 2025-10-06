#ifndef ASHEN_ISKYBOX_H
#define ASHEN_ISKYBOX_H

#include <glm/glm.hpp>

namespace ash {
    class ISkybox {
    public:
        virtual ~ISkybox() = default;

        virtual void Render(const glm::mat4 &view, const glm::mat4 &projection) = 0;
    };
}

#endif // ASHEN_ISKYBOX_H
