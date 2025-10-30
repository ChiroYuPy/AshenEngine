#ifndef ASHEN_INPUT_H
#define ASHEN_INPUT_H

#include <bitset>
#include <glad/glad.h>

#include "Ashen/Core/Codes.h"
#include "Ashen/Events/Event.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Math/Math.h"
#include "Ashen/Core/Types.h"

struct GLFWwindow;

namespace ash {
    class Window;

    enum class CursorMode : GLint {
        Normal = 0x00034001,
        Hidden = 0x00034002,
        Captured = 0x00034003
    };

    enum class InputState {
        None = 0,
        Pressed = 1 << 0,
        Released = 1 << 1,
        JustPressed = 1 << 2
    };

    inline InputState operator|(InputState a, InputState b) {
        return static_cast<InputState>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline InputState operator&(InputState a, InputState b) {
        return static_cast<InputState>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline bool HasFlag(InputState state, InputState flag) {
        return (static_cast<int>(state) & static_cast<int>(flag)) != 0;
    }

    class Input {
    public:
        static void Init(const Window &window);

        static void Shutdown();

        static void OnEvent(Event &event);

        static void Update();

        static bool IsKeyPressed(Key keycode);

        static bool IsKeyJustPressed(Key keycode);

        static bool IsKeyJustReleased(Key keycode);

        static bool IsKeyDown(const Key keycode) { return IsKeyPressed(keycode); }
        static bool IsKeyUp(const Key keycode) { return !IsKeyPressed(keycode); }

        static bool IsKeyRepeating(Key keycode);

        static bool AreKeysPressed(std::initializer_list<Key> keycodes);

        static bool AnyKeyPressed(std::initializer_list<Key> keycodes);

        static bool IsMouseButtonPressed(MouseButton button);

        static bool IsMouseButtonJustPressed(MouseButton button);

        static bool IsMouseButtonJustReleased(MouseButton button);

        static bool IsMouseButtonDown(const MouseButton button) { return IsMouseButtonPressed(button); }
        static bool IsMouseButtonUp(const MouseButton button) { return !IsMouseButtonPressed(button); }

        static bool AreMouseButtonsPressed(std::initializer_list<MouseButton> buttons);

        static bool AnyMouseButtonPressed(std::initializer_list<MouseButton> buttons);

        static Vec2 GetMousePosition();

        static Vec2 GetMouseDelta();

        static Vec2 GetMouseDeltaSmooth(float smoothing = 0.5f);

        static float GetMouseX() { return GetMousePosition().x; }
        static float GetMouseY() { return GetMousePosition().y; }

        static float GetMouseDeltaX() { return GetMouseDelta().x; }
        static float GetMouseDeltaY() { return GetMouseDelta().y; }

        static float GetMouseScrollDelta();

        static Vec2 GetMouseScroll2D();

        static void SetCursorMode(CursorMode mode);

        static CursorMode GetCursorMode();

        static void ShowCursor();

        static void HideCursor();

        static void CaptureCursor();

        static void ReleaseCursor();

        static void SetCursorPosition(const Vec2 &position);

        static void SetCursorPosition(float x, float y);

        static void EnableRawMouseMotion(bool enabled);

        static bool IsRawMouseMotionEnabled();

        static void EnableStickyKeys(bool enabled);

        static void EnableStickyMouseButtons(bool enabled);

        static void SetMouseSensitivity(float sensitivity);

        static float GetMouseSensitivity();

        static void ResetMouseDelta();

        static void ResetInput();

        static bool IsControllerConnected(int controllerID = 0);

        static int GetConnectedControllerCount();

        static void EnableInputHistory(bool enabled, size_t historySize = 10);

        static const Vector<int> &GetKeyHistory();

        static String GetKeyName(Key keycode);

        static String GetMouseButtonName(MouseButton button);

        static bool AnyKeyPressed();

        static bool AnyMouseButtonPressed();

    private:
        static constexpr size_t MAX_KEYS = 512;
        static constexpr size_t MAX_MOUSE_BUTTONS = 8;
        static constexpr size_t MAX_HISTORY = 20;

        struct InputData {
            GLFWwindow *window = nullptr;

            std::bitset<MAX_KEYS> keys;
            std::bitset<MAX_KEYS> keysPrevious;
            std::bitset<MAX_KEYS> keysRepeating;

            std::bitset<MAX_MOUSE_BUTTONS> mouseButtons;
            std::bitset<MAX_MOUSE_BUTTONS> mouseButtonsPrevious;

            Vec2 mousePosition{0.0f};
            Vec2 mousePositionPrevious{0.0f};
            Vec2 mouseDelta{0.0f};
            Vec2 mouseDeltaSmoothed{0.0f};

            float mouseScrollDelta = 0.0f;
            Vec2 mouseScroll2D{0.0f};

            CursorMode cursorMode = CursorMode::Normal;
            float mouseSensitivity = 1.0f;
            bool rawMouseMotion = true;
            bool firstMouseUpdate = true;

            bool historyEnabled = false;
            size_t maxHistorySize = 10;
            Vector<int> keyHistory;
        };

        static InputData s_Data;

        static bool OnKeyPressed(const KeyPressedEvent &e);

        static bool OnKeyReleased(const KeyReleasedEvent &e);

        static bool OnMouseButtonPressed(const MouseButtonPressedEvent &e);

        static bool OnMouseButtonReleased(const MouseButtonReleasedEvent &e);

        static bool OnMouseMoved(const MouseMovedEvent &e);

        static bool OnMouseScrolled(const MouseScrolledEvent &e);

        static void UpdateMouseDelta();

        static void UpdateInputHistory(Key keycode);

        static bool IsValidKey(Key keycode);

        static bool IsValidMouseButton(MouseButton button);
    };
}

#endif // ASHEN_INPUT_H