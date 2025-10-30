#ifndef ASHEN_CAMERA_CONTROLLER_H
#define ASHEN_CAMERA_CONTROLLER_H

#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Events/Event.h"

namespace ash {
    class CameraController {
    public:
        virtual ~CameraController() = default;

        CameraController(const CameraController &) = delete;

        CameraController &operator=(const CameraController &) = delete;

        virtual void Update(float deltaTime) = 0;

        virtual void OnEvent(Event &event) = 0;

        bool IsEnabled() const { return m_Enabled; }
        void SetEnabled(const bool enabled) { m_Enabled = enabled; }

        Camera &GetCamera() { return m_Camera; }
        const Camera &GetCamera() const { return m_Camera; }

    protected:
        explicit CameraController(Camera &camera)
            : m_Camera(camera) {
        }

        Camera &m_Camera;
        bool m_Enabled = true;
    };
}

#endif // ASHEN_CAMERA_CONTROLLER_H
