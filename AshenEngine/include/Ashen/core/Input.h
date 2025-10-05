#ifndef ASHEN_INPUT_H
#define ASHEN_INPUT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <bitset>

#include "Ashen/events/Event.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/events/MouseEvent.h"
#include "Ashen/math/Math.h"

namespace pixl {
    class Window;

    enum class CursorMode : GLint {
        Normal = 0x00034001,
        Hidden = 0x00034002,
        Disabled = 0x00034003
    };

    class Input {
    public:
        static void Init(const Window &window);

        static void OnEvent(Event &event);

        static void Update(); // Call once per frame

        // Key state queries
        static bool IsKeyPressed(int keycode);

        static bool IsKeyJustPressed(int keycode); // True only on first frame
        static bool IsKeyJustReleased(int keycode);

        // Mouse button queries
        static bool IsMouseButtonPressed(int button);

        static bool IsMouseButtonJustPressed(int button);

        static bool IsMouseButtonJustReleased(int button);

        // Mouse position & movement
        static Vec2 GetMousePosition();

        static Vec2 GetMouseDelta();

        static float GetMouseScrollDelta();

        // Cursor control
        static void SetCursorMode(CursorMode mode);

        static void ResetMouseDelta();

        static void EnableRawMouseMotion(bool enabled);

        static void EnableStickyKeys(bool enabled);

        static void EnableStickyMouseButtons(bool enabled);

    private:
        static constexpr size_t MAX_KEYS = 512;
        static constexpr size_t MAX_MOUSE_BUTTONS = 8;

        inline static GLFWwindow *s_Window = nullptr;

        // Current frame state
        inline static std::bitset<MAX_KEYS> s_Keys;
        inline static std::bitset<MAX_MOUSE_BUTTONS> s_MouseButtons;

        // Previous frame state (for "just pressed/released" detection)
        inline static std::bitset<MAX_KEYS> s_KeysPrevious;
        inline static std::bitset<MAX_MOUSE_BUTTONS> s_MouseButtonsPrevious;

        // Mouse state
        inline static Vec2 s_MousePosition{0.0f};
        inline static Vec2 s_MousePositionPrevious{0.0f};
        inline static Vec2 s_MouseDelta{0.0f};
        inline static float s_MouseScrollDelta = 0.0f;
        inline static bool s_FirstMouseUpdate = true;

        static bool OnKeyPressed(const KeyPressedEvent &e);

        static bool OnKeyReleased(const KeyReleasedEvent &e);

        static bool OnMouseButtonPressed(const MouseButtonPressedEvent &e);

        static bool OnMouseButtonReleased(const MouseButtonReleasedEvent &e);

        static bool OnMouseMoved(const MouseMovedEvent &e);

        static bool OnMouseScrolled(const MouseScrolledEvent &e);
    };
}

#endif //ASHEN_INPUT_H