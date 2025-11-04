#ifndef ASHEN_IMGUILAYER_H
#define ASHEN_IMGUILAYER_H

#include "Ashen/Core/Layer.h"

namespace ash {
    /**
     * @brief Layer for ImGui rendering and event handling
     */
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& event) override;

        void Begin();
        void End();

        void SetDarkThemeColors();

    private:
        float m_Time = 0.0f;
    };
}

#endif // ASHEN_IMGUILAYER_H
