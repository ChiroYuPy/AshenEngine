#ifndef VOXELITY_UILAYER_H
#define VOXELITY_UILAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/GraphicsAPI/Texture.h"

namespace voxelity {
    class UILayer final : public ash::Layer {
    public:
        UILayer();

        void OnRender() override;

        void OnEvent(ash::Event &event) override;

    private:
        ash::Ref<ash::UICamera> m_UICamera;
        ash::Ref<ash::Texture2D> m_CrosshairTexture;

        bool showCrosshair;
    };
}

#endif //VOXELITY_UILAYER_H