#ifndef ASHEN_TESTLAYER_H
#define ASHEN_TESTLAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Cameras/Camera.h"

namespace ash {
    class Renderer2DTestLayer final : public Layer {
    public:
        void OnAttach() override;

        void OnUpdate(float deltaTime) override;

        void OnRender() override;

        void OnEvent(Event &event) override;

    private:
        Ref<OrthographicCamera> mCamera;
    };
}

#endif //ASHEN_TESTLAYER_H
