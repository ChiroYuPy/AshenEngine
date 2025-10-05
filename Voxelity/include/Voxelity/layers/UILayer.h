#ifndef VOXELITY_UILAYER_H
#define VOXELITY_UILAYER_H

#include "Ashen/core/Layer.h"
#include "Ashen/renderer/Camera.h"
#include "Ashen/renderer/gfx/Texture.h"

namespace voxelity {
    class UILayer final : public ash::Layer {
    public:
        UILayer();

        void OnRender() override;

        void OnEvent(ash::Event &event) override;

    private:
        ash::Ref<ash::UICamera> m_UICamera;
        ash::Ref<ash::Texture2D> m_CrosshairTexture;

        bool m_3DCrosshair;
    };
}

#endif //VOXELITY_UILAYER_H