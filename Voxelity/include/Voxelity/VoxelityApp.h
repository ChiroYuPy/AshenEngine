#ifndef VOXELITY_APPLICATION_H
#define VOXELITY_APPLICATION_H

#include "Ashen/core/Application.h"

namespace voxelity {
    class VoxelityApp : public pixl::Application {
    public:
        explicit VoxelityApp(const pixl::ApplicationSettings &settings);

        static VoxelityApp &Get() {
            return static_cast<VoxelityApp &>(Application::Get());
        }
    };
}

#endif //VOXELITY_APPLICATION_H
