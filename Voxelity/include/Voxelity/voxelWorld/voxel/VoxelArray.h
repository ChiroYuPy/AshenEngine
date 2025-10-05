#ifndef VOXELITY_VOXELARRAY_H
#define VOXELITY_VOXELARRAY_H

#include <array>

#include "Voxelity/voxelWorld/voxel/VoxelType.h"

namespace voxelity {
    class VoxelArray {
    public:
        static constexpr int SIZE = 32;
        static constexpr int VOLUME = SIZE * SIZE * SIZE;

        VoxelArray();

        VoxelType get(int x, int y, int z) const;

        void set(int x, int y, int z, VoxelType voxel);

        void fill(VoxelType ID);

        static double getMemoryUsage();

    private:
        static int index(int x, int y, int z);

        std::array<VoxelType, VOLUME> voxels;
    };
}


#endif //VOXELITY_VOXELARRAY_H