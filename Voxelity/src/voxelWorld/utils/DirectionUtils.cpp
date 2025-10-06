#include "Voxelity/voxelWorld/utils/DirectionUtils.h"

#include <stdexcept>

namespace voxelity::DirectionUtils {
    CubicDirection getOpposite(const CubicDirection direction) {
        switch (direction) {
            case CubicDirection::XP: return CubicDirection::XN;
            case CubicDirection::XN: return CubicDirection::XP;
            case CubicDirection::YP: return CubicDirection::YN;
            case CubicDirection::YN: return CubicDirection::YP;
            case CubicDirection::ZP: return CubicDirection::ZN;
            case CubicDirection::ZN: return CubicDirection::ZP;
            default: throw std::invalid_argument("Invalid CubicDirection");
        }
    }

    glm::ivec3 getOffset(const CubicDirection direction) {
        switch (direction) {
            case CubicDirection::XP: return {1, 0, 0};
            case CubicDirection::XN: return {-1, 0, 0};
            case CubicDirection::YP: return {0, 1, 0};
            case CubicDirection::YN: return {0, -1, 0};
            case CubicDirection::ZP: return {0, 0, 1};
            case CubicDirection::ZN: return {0, 0, -1};
            default: throw std::invalid_argument("Invalid CubicDirection");
        }
    }

    bool isValid(CubicDirection direction) {
        return static_cast<int>(direction) >= 0 && static_cast<int>(direction) <= 5;
    }

    CubicDirection fromOffset(const int x, const int y, const int z) {
        if (x == 1 && y == 0 && z == 0) return CubicDirection::XP;
        if (x == -1 && y == 0 && z == 0) return CubicDirection::XN;
        if (x == 0 && y == 1 && z == 0) return CubicDirection::YP;
        if (x == 0 && y == -1 && z == 0) return CubicDirection::YN;
        if (x == 0 && y == 0 && z == 1) return CubicDirection::ZP;
        if (x == 0 && y == 0 && z == -1) return CubicDirection::ZN;
        throw std::invalid_argument("Invalid offset for CubicDirection");
    }

    CubicDirection fromOffset(const glm::ivec3 &offset) {
        return fromOffset(offset.x, offset.y, offset.z);
    }

    CubicDirection fromIndex(int index) {
        if (index >= 0 && index < 6)
            return static_cast<CubicDirection>(index);
        return CubicDirection::ZP;
    }
}