#ifndef ASHEN_INPUT_H
#define ASHEN_INPUT_H

#include <bitset>
#include <array>
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

    // ========== Cursor Mode ==========
    enum class CursorMode : GLint {
        Normal = 0x00034001,
        Hidden = 0x00034002,
        Captured = 0x00034003
    };

    // ========== Input State ==========
    enum class InputState {
        None = 0,
        Pressed = 1 << 0, // Currently down
        Released = 1 << 1, // Just released this frame
        JustPressed = 1 << 2 // Just pressed this frame
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

    // ========== Input Manager ==========
    class Input {
    public:
        // ========== Initialization ==========
        static void Init(const Window &window);

        static void Shutdown();

        // ========== Frame Updates ==========
        static void OnEvent(Event &event);

        static void Update(); // Call once per frame

        // ========== Keyboard Queries ==========
        static bool IsKeyPressed(Key keycode);

        static bool IsKeyJustPressed(Key keycode);

        static bool IsKeyJustReleased(Key keycode);

        static bool IsKeyDown(const Key keycode) { return IsKeyPressed(keycode); }
        static bool IsKeyUp(const Key keycode) { return !IsKeyPressed(keycode); }

        // Get key repeat status
        static bool IsKeyRepeating(Key keycode);

        // Multi-key queries
        static bool AreKeysPressed(std::initializer_list<Key> keycodes);

        static bool AnyKeyPressed(std::initializer_list<Key> keycodes);

        // ========== Mouse Button Queries ==========
        static bool IsMouseButtonPressed(MouseButton button);

        static bool IsMouseButtonJustPressed(MouseButton button);

        static bool IsMouseButtonJustReleased(MouseButton button);

        static bool IsMouseButtonDown(const MouseButton button) { return IsMouseButtonPressed(button); }
        static bool IsMouseButtonUp(const MouseButton button) { return !IsMouseButtonPressed(button); }

        // Multi-button queries
        static bool AreMouseButtonsPressed(std::initializer_list<MouseButton> buttons);

        static bool AnyMouseButtonPressed(std::initializer_list<MouseButton> buttons);

        // ========== Mouse Position & Movement ==========
        static Vec2 GetMousePosition();

        static Vec2 GetMouseDelta();

        static Vec2 GetMouseDeltaSmooth(float smoothing = 0.5f);

        static float GetMouseX() { return GetMousePosition().x; }
        static float GetMouseY() { return GetMousePosition().y; }

        static float GetMouseDeltaX() { return GetMouseDelta().x; }
        static float GetMouseDeltaY() { return GetMouseDelta().y; }

        // ========== Mouse Scroll ==========
        static float GetMouseScrollDelta();

        static Vec2 GetMouseScroll2D();

        // ========== Cursor Control ==========
        static void SetCursorMode(CursorMode mode);

        static CursorMode GetCursorMode();

        static void ShowCursor();

        static void HideCursor();

        static void CaptureCursor(); // Lock and hide
        static void ReleaseCursor(); // Show and unlock

        static void SetCursorPosition(const Vec2 &position);

        static void SetCursorPosition(float x, float y);

        // ========== Input Settings ==========
        static void EnableRawMouseMotion(bool enabled);

        static bool IsRawMouseMotionEnabled();

        static void EnableStickyKeys(bool enabled);

        static void EnableStickyMouseButtons(bool enabled);

        static void SetMouseSensitivity(float sensitivity);

        static float GetMouseSensitivity();

        // ========== Input State Reset ==========
        static void ResetMouseDelta();

        static void ResetInput(); // Reset all input states

        // ========== Gamepad/Controller Support (Future) ==========
        // These can be implemented when gamepad support is needed
        static bool IsControllerConnected(int controllerID = 0);

        static int GetConnectedControllerCount();

        // ========== Input History (for combos, gestures) ==========
        static void EnableInputHistory(bool enabled, size_t historySize = 10);

        static const Vector<int> &GetKeyHistory();

        // ========== Utility Functions ==========
        static String GetKeyName(Key keycode);

        static String GetMouseButtonName(MouseButton button);

        // Check if any key/button is pressed
        static bool AnyKeyPressed();

        static bool AnyMouseButtonPressed();

    private:
        // ========== Internal State ==========
        static constexpr size_t MAX_KEYS = 512;
        static constexpr size_t MAX_MOUSE_BUTTONS = 8;
        static constexpr size_t MAX_HISTORY = 20;

        struct InputData {
            GLFWwindow *window = nullptr;

            // Keyboard state
            std::bitset<MAX_KEYS> keys;
            std::bitset<MAX_KEYS> keysPrevious;
            std::bitset<MAX_KEYS> keysRepeating;

            // Mouse button state
            std::bitset<MAX_MOUSE_BUTTONS> mouseButtons;
            std::bitset<MAX_MOUSE_BUTTONS> mouseButtonsPrevious;

            // Mouse position and movement
            Vec2 mousePosition{0.0f};
            Vec2 mousePositionPrevious{0.0f};
            Vec2 mouseDelta{0.0f};
            Vec2 mouseDeltaSmoothed{0.0f};

            // Mouse scroll
            float mouseScrollDelta = 0.0f;
            Vec2 mouseScroll2D{0.0f};

            // Settings
            CursorMode cursorMode = CursorMode::Normal;
            float mouseSensitivity = 1.0f;
            bool rawMouseMotion = true;
            bool firstMouseUpdate = true;

            // Input history
            bool historyEnabled = false;
            size_t maxHistorySize = 10;
            Vector<int> keyHistory;
        };

        static InputData s_Data;

        // ========== Event Handlers ==========
        static bool OnKeyPressed(const KeyPressedEvent &e);

        static bool OnKeyReleased(const KeyReleasedEvent &e);

        static bool OnMouseButtonPressed(const MouseButtonPressedEvent &e);

        static bool OnMouseButtonReleased(const MouseButtonReleasedEvent &e);

        static bool OnMouseMoved(const MouseMovedEvent &e);

        static bool OnMouseScrolled(const MouseScrolledEvent &e);

        // ========== Internal Utilities ==========
        static void UpdateMouseDelta();

        static void UpdateInputHistory(Key keycode);

        static bool IsValidKey(Key keycode);

        static bool IsValidMouseButton(MouseButton button);
    };
} // namespace ash

#endif // ASHEN_INPUT_H