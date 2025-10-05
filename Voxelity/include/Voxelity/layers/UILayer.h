#ifndef VOXELITY_UILAYER_H
#define VOXELITY_UILAYER_H

#include "Ashen/core/Layer.h"
#include "Ashen/renderer/Camera.h"

namespace voxelity {
    class UILayer final : public pixl::Layer {
    public:
        UILayer();

        void OnRender() override;

        void OnEvent(pixl::Event &event) override;

        void ToggleInventory() { m_ShowInventory = !m_ShowInventory; }
        [[nodiscard]] bool IsInventoryOpen() const { return m_ShowInventory; }

    private:
        pixl::Ref<pixl::OrthographicCamera> m_UICamera;
        bool m_ShowInventory = false;
    };
}

#endif //VOXELITY_UILAYER_H
