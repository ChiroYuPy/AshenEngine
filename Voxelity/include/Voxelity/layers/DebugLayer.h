#ifndef VOXELITY_DEBUGLAYER_H
#define VOXELITY_DEBUGLAYER_H

#include "Ashen/core/Layer.h"
#include "Ashen/renderer/Camera.h"

namespace voxelity {
    class DebugLayer final : public pixl::Layer {
    public:
        DebugLayer();

        void OnRender() override;

        void OnEvent(pixl::Event &event) override;

    private:
        pixl::Ref<pixl::OrthographicCamera> m_DebugCamera;
    };
}

#endif //VOXELITY_DEBUGLAYER_H