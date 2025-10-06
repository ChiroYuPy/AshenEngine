#ifndef VOXELITY_DEBUGLAYER_H
#define VOXELITY_DEBUGLAYER_H

#include "Ashen/core/Layer.h"
#include "Ashen/core/Types.h"
#include "Ashen/renderer/Camera.h"

namespace voxelity {
    class DebugLayer final : public ash::Layer {
    public:
        DebugLayer();

        void OnRender() override;

        void OnEvent(ash::Event &event) override;

    private:
        ash::Ref<ash::OrthographicCamera> m_DebugCamera;
    };
}

#endif //VOXELITY_DEBUGLAYER_H