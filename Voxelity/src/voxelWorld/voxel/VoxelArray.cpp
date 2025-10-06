#include "Voxelity/voxelWorld/voxel/VoxelArray.h"

#include <stdexcept>

namespace voxelity {
    VoxelArray::VoxelArray() : voxels{} {
        fill(VoxelID::AIR);
    }

    VoxelType VoxelArray::get(const int x, const int y, const int z) const {
        return voxels[index(x, y, z)];
    }

    void VoxelArray::set(const int x, const int y, const int z, const VoxelType voxel) {
        voxels[index(x, y, z)] = voxel;
    }

    void VoxelArray::fill(const VoxelType ID) {
        voxels.fill(ID);
    }

    int VoxelArray::index(const int x, const int y, const int z) {
#ifndef NDEBUG
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
            throw std::out_of_range("voxel position out of bounds");
#endif
        return x + SIZE * (z + SIZE * y);
    }

    double VoxelArray::getMemoryUsage() {
        return static_cast<double>(VOLUME) * sizeof(VoxelType);
    }
}