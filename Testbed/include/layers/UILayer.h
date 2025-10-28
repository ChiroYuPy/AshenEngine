#ifndef ASHEN_UILAYER_H
#define ASHEN_UILAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Camera/Camera.h"

namespace ash {
    enum class UIState {
        MainMenu,
        GameHUD,
        PauseMenu
    };

    class UILayer final : public Layer {
    public:
        void OnAttach() override;

        void OnUpdate(float deltaTime) override;

        void OnRender() override;

        void OnEvent(Event &event) override;

        void OnDetach() override;

        void SetUIState(UIState state);

    private:
        void CreateMainMenu();

        void CreateGameHUD();

        void CreatePauseMenu();

        Ref<OrthographicCamera> mCamera;
        UIState mCurrentState;
    };
}

#endif //ASHEN_UILAYER_H
