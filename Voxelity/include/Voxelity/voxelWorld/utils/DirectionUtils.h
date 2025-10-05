#ifndef VOXELITY_DIRECTIONUTILS_H
#define VOXELITY_DIRECTIONUTILS_H

#include <glm/vec3.hpp>

namespace voxelity {
    enum class CubicDirection {
        ZP = 0, // +Z
        ZN = 1, // -Z
        XP = 2, // +X
        XN = 3, // -X
        YP = 4, // +Y
        YN = 5 // -Y
    };

    namespace DirectionUtils {
        CubicDirection getOpposite(CubicDirection direction);

        glm::ivec3 getOffset(CubicDirection direction);

        bool isValid(CubicDirection direction);

        CubicDirection fromOffset(int x, int y, int z);

        CubicDirection fromOffset(const glm::ivec3 &offset);

        CubicDirection fromIndex(int index);
    }
}

#endif //VOXELITY_DIRECTIONUTILS_H