#ifndef ASHEN_TRANSFORM_H
#define ASHEN_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ash {
    class Transform {
    public:
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
    };
}

#endif //ASHEN_TRANSFORM_H