#ifndef VOXELITY_APPLICATION_H
#define VOXELITY_APPLICATION_H

#include "Ashen/core/Application.h"

namespace voxelity {
    class VoxelityApp : public ash::Application {
    public:
        explicit VoxelityApp(const ash::ApplicationSettings &settings);

        static VoxelityApp &Get() {
            return static_cast<VoxelityApp &>(Application::Get());
        }
    };
}

#endif //VOXELITY_APPLICATION_H
