#include "Ashen/Core/Input.h"
#include "Ashen/Core/Window.h"
#include <GLFW/glfw3.h>

namespace ash {
    Input::InputData Input::s_Data;

    // ========== Initialization ==========
    void Input::Init(const Window &window) {
        s_Data.window = static_cast<GLFWwindow *>(window.GetHandle());

        // Reset all states
        s_Data.keys.reset();
        s_Data.keysPrevious.reset();
        s_Data.keysRepeating.reset();
        s_Data.mouseButtons.reset();
        s_Data.mouseButtonsPrevious.reset();

        s_Data.mousePosition = Vec2(0.0f);
        s_Data.mousePositionPrevious = Vec2(0.0f);
        s_Data.mouseDelta = Vec2(0.0f);
        s_Data.mouseDeltaSmoothed = Vec2(0.0f);
        s_Data.mouseScrollDelta = 0.0f;
        s_Data.mouseScroll2D = Vec2(0.0f);

        s_Data.firstMouseUpdate = true;
        s_Data.cursorMode = CursorMode::Normal;
        s_Data.mouseSensitivity = 1.0f;
        s_Data.rawMouseMotion = true;

        // Enable raw mouse motion if supported
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(s_Data.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        // Get initial mouse position
        double x, y;
        glfwGetCursorPos(s_Data.window, &x, &y);
        s_Data.mousePosition = Vec2(static_cast<float>(x), static_cast<float>(y));
        s_Data.mousePositionPrevious = s_Data.mousePosition;
    }

    void Input::Shutdown() {
        s_Data.window = nullptr;
        s_Data.keys.reset();
        s_Data.keysPrevious.reset();
        s_Data.keysRepeating.reset();
        s_Data.mouseButtons.reset();
        s_Data.mouseButtonsPrevious.reset();
        s_Data.keyHistory.clear();
    }

    // ========== Frame Updates ==========
    void Input::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent &e) {
            return OnKeyPressed(e);
        });

        dispatcher.Dispatch<KeyReleasedEvent>([](KeyReleasedEvent &e) {
            return OnKeyReleased(e);
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent &e) {
            return OnMouseButtonPressed(e);
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent &e) {
            return OnMouseButtonReleased(e);
        });

        dispatcher.Dispatch<MouseMovedEvent>([](MouseMovedEvent &e) {
            return OnMouseMoved(e);
        });

        dispatcher.Dispatch<MouseScrolledEvent>([](MouseScrolledEvent &e) {
            return OnMouseScrolled(e);
        });
    }

    void Input::Update() {
        // Save previous frame state
        s_Data.keysPrevious = s_Data.keys;
        s_Data.mouseButtonsPrevious = s_Data.mouseButtons;

        // Clear repeating flags (they're set by events)
        s_Data.keysRepeating.reset();

        // Update mouse delta with smoothing
        UpdateMouseDelta();

        // Reset scroll delta (it's set by events)
        s_Data.mouseScrollDelta = 0.0f;
        s_Data.mouseScroll2D = Vec2(0.0f);
    }

    // ========== Keyboard Queries ==========
    bool Input::IsKeyPressed(KeyCode keycode) {
        if (!IsValidKey(keycode)) return false;
        return s_Data.keys[static_cast<size_t>(keycode)];
    }

    bool Input::IsKeyJustPressed(KeyCode keycode) {
        if (!IsValidKey(keycode)) return false;
        return s_Data.keys[static_cast<size_t>(keycode)] &&
               !s_Data.keysPrevious[static_cast<size_t>(keycode)];
    }

    bool Input::IsKeyJustReleased(KeyCode keycode) {
        if (!IsValidKey(keycode)) return false;
        return !s_Data.keys[static_cast<size_t>(keycode)] &&
               s_Data.keysPrevious[static_cast<size_t>(keycode)];
    }

    bool Input::IsKeyRepeating(KeyCode keycode) {
        if (!IsValidKey(keycode)) return false;
        return s_Data.keysRepeating[static_cast<size_t>(keycode)];
    }

    bool Input::AreKeysPressed(std::initializer_list<KeyCode> keycodes) {
        for (const KeyCode key: keycodes) {
            if (!IsKeyPressed(key)) return false;
        }
        return true;
    }

    bool Input::AnyKeyPressed(std::initializer_list<KeyCode> keycodes) {
        for (const KeyCode key: keycodes) {
            if (IsKeyPressed(key)) return true;
        }
        return false;
    }

    // ========== Mouse Button Queries ==========
    bool Input::IsMouseButtonPressed(MouseButton button) {
        if (!IsValidMouseButton(button)) return false;
        return s_Data.mouseButtons[static_cast<size_t>(button)];
    }

    bool Input::IsMouseButtonJustPressed(MouseButton button) {
        if (!IsValidMouseButton(button)) return false;
        return s_Data.mouseButtons[static_cast<size_t>(button)] &&
               !s_Data.mouseButtonsPrevious[static_cast<size_t>(button)];
    }

    bool Input::IsMouseButtonJustReleased(MouseButton button) {
        if (!IsValidMouseButton(button)) return false;
        return !s_Data.mouseButtons[static_cast<size_t>(button)] &&
               s_Data.mouseButtonsPrevious[static_cast<size_t>(button)];
    }

    bool Input::AreMouseButtonsPressed(std::initializer_list<MouseButton> buttons) {
        for (const MouseButton button: buttons) {
            if (!IsMouseButtonPressed(button)) return false;
        }
        return true;
    }

    bool Input::AnyMouseButtonPressed(std::initializer_list<MouseButton> buttons) {
        for (const MouseButton button: buttons) {
            if (IsMouseButtonPressed(button)) return true;
        }
        return false;
    }

    // ========== Mouse Position & Movement ==========
    Vec2 Input::GetMousePosition() {
        return s_Data.mousePosition;
    }

    Vec2 Input::GetMouseDelta() {
        return s_Data.mouseDelta * s_Data.mouseSensitivity;
    }

    Vec2 Input::GetMouseDeltaSmooth(float smoothing) {
        return s_Data.mouseDeltaSmoothed * s_Data.mouseSensitivity * smoothing;
    }

    // ========== Mouse Scroll ==========
    float Input::GetMouseScrollDelta() {
        return s_Data.mouseScrollDelta;
    }

    Vec2 Input::GetMouseScroll2D() {
        return s_Data.mouseScroll2D;
    }

    // ========== Cursor Control ==========
    void Input::SetCursorMode(CursorMode mode) {
        if (!s_Data.window || s_Data.cursorMode == mode) return;

        s_Data.cursorMode = mode;
        glfwSetInputMode(s_Data.window, GLFW_CURSOR, static_cast<int>(mode));

        // Reset mouse delta when changing cursor mode
        if (mode == CursorMode::Disabled || mode == CursorMode::Captured) {
            ResetMouseDelta();
        }
    }

    CursorMode Input::GetCursorMode() {
        return s_Data.cursorMode;
    }

    void Input::ShowCursor() {
        SetCursorMode(CursorMode::Normal);
    }

    void Input::HideCursor() {
        SetCursorMode(CursorMode::Hidden);
    }

    void Input::CaptureCursor() {
        SetCursorMode(CursorMode::Captured);
    }

    void Input::ReleaseCursor() {
        SetCursorMode(CursorMode::Normal);
    }

    void Input::SetCursorPosition(const Vec2 &position) {
        SetCursorPosition(position.x, position.y);
    }

    void Input::SetCursorPosition(float x, float y) {
        if (!s_Data.window) return;
        glfwSetCursorPos(s_Data.window, static_cast<double>(x), static_cast<double>(y));
        s_Data.mousePosition = Vec2(x, y);
        s_Data.mousePositionPrevious = s_Data.mousePosition;
    }

    // ========== Input Settings ==========
    void Input::EnableRawMouseMotion(bool enabled) {
        if (!s_Data.window) return;
        s_Data.rawMouseMotion = enabled;

        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(s_Data.window, GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
        }
    }

    bool Input::IsRawMouseMotionEnabled() {
        return s_Data.rawMouseMotion;
    }

    void Input::EnableStickyKeys(bool enabled) {
        if (!s_Data.window) return;
        glfwSetInputMode(s_Data.window, GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
    }

    void Input::EnableStickyMouseButtons(bool enabled) {
        if (!s_Data.window) return;
        glfwSetInputMode(s_Data.window, GLFW_STICKY_MOUSE_BUTTONS, enabled ? GLFW_TRUE : GLFW_FALSE);
    }

    void Input::SetMouseSensitivity(float sensitivity) {
        s_Data.mouseSensitivity = sensitivity;
    }

    float Input::GetMouseSensitivity() {
        return s_Data.mouseSensitivity;
    }

    // ========== Input State Reset ==========
    void Input::ResetMouseDelta() {
        s_Data.mouseDelta = Vec2(0.0f);
        s_Data.mouseDeltaSmoothed = Vec2(0.0f);
        s_Data.mousePositionPrevious = s_Data.mousePosition;
        s_Data.firstMouseUpdate = true;
    }

    void Input::ResetInput() {
        s_Data.keys.reset();
        s_Data.keysPrevious.reset();
        s_Data.keysRepeating.reset();
        s_Data.mouseButtons.reset();
        s_Data.mouseButtonsPrevious.reset();
        ResetMouseDelta();
        s_Data.keyHistory.clear();
    }

    // ========== Gamepad/Controller Support ==========
    bool Input::IsControllerConnected(int controllerID) {
        return glfwJoystickPresent(controllerID) == GLFW_TRUE;
    }

    int Input::GetConnectedControllerCount() {
        int count = 0;
        for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
            if (glfwJoystickPresent(i)) ++count;
        }
        return count;
    }

    // ========== Input History ==========
    void Input::EnableInputHistory(bool enabled, size_t historySize) {
        s_Data.historyEnabled = enabled;
        s_Data.maxHistorySize = historySize;
        if (!enabled) {
            s_Data.keyHistory.clear();
        }
    }

    const Vector<int> &Input::GetKeyHistory() {
        return s_Data.keyHistory;
    }

    // ========== Utility Functions ==========
    String Input::GetKeyName(const KeyCode keycode) {
        const char *name = glfwGetKeyName(static_cast<int>(keycode), 0);
        return name ? String(name) : "Unknown";
    }

    String Input::GetMouseButtonName(const MouseButton button) {
        switch (button) {
            case MouseButton::Left: return "Left";
            case MouseButton::Right: return "Right";
            case MouseButton::Middle: return "Middle";
            default: return "Button" + std::to_string(static_cast<int>(button));
        }
    }

    bool Input::AnyKeyPressed() {
        return s_Data.keys.any();
    }

    bool Input::AnyMouseButtonPressed() {
        return s_Data.mouseButtons.any();
    }

    // ========== Event Handlers ==========
    bool Input::OnKeyPressed(const KeyPressedEvent &e) {
        KeyCode keycode = e.GetKeyCode();
        if (!IsValidKey(keycode)) return false;

        const size_t key = static_cast<size_t>(keycode);
        s_Data.keys[key] = true;

        if (e.IsRepeat()) {
            s_Data.keysRepeating[key] = true;
        } else {
            UpdateInputHistory(keycode);
        }

        return false;
    }

    bool Input::OnKeyReleased(const KeyReleasedEvent &e) {
        KeyCode keycode = e.GetKeyCode();
        if (!IsValidKey(keycode)) return false;

        const size_t key = static_cast<size_t>(keycode);
        s_Data.keys[key] = false;
        s_Data.keysRepeating[key] = false;

        return false;
    }

    bool Input::OnMouseButtonPressed(const MouseButtonPressedEvent &e) {
        MouseButton button = e.GetButton();
        if (!IsValidMouseButton(button)) return false;

        s_Data.mouseButtons[static_cast<size_t>(button)] = true;
        return false;
    }

    bool Input::OnMouseButtonReleased(const MouseButtonReleasedEvent &e) {
        MouseButton button = e.GetButton();
        if (!IsValidMouseButton(button)) return false;

        s_Data.mouseButtons[static_cast<size_t>(button)] = false;
        return false;
    }

    bool Input::OnMouseMoved(const MouseMovedEvent &e) {
        s_Data.mousePosition = Vec2(e.GetX(), e.GetY());
        return false;
    }

    bool Input::OnMouseScrolled(const MouseScrolledEvent &e) {
        s_Data.mouseScrollDelta = e.GetYOffset();
        s_Data.mouseScroll2D = Vec2(e.GetXOffset(), e.GetYOffset());
        return false;
    }

    // ========== Internal Utilities ==========
    void Input::UpdateMouseDelta() {
        if (s_Data.firstMouseUpdate) {
            s_Data.mouseDelta = Vec2(0.0f);
            s_Data.mouseDeltaSmoothed = Vec2(0.0f);
            s_Data.firstMouseUpdate = false;
        } else {
            s_Data.mouseDelta = s_Data.mousePosition - s_Data.mousePositionPrevious;

            // Exponential smoothing
            constexpr float smoothingFactor = 0.3f;
            s_Data.mouseDeltaSmoothed = s_Data.mouseDeltaSmoothed * (1.0f - smoothingFactor) +
                                        s_Data.mouseDelta * smoothingFactor;
        }

        s_Data.mousePositionPrevious = s_Data.mousePosition;
    }

    void Input::UpdateInputHistory(KeyCode keycode) {
        if (!s_Data.historyEnabled) return;

        s_Data.keyHistory.push_back(static_cast<int>(keycode));

        if (s_Data.keyHistory.size() > s_Data.maxHistorySize)
            s_Data.keyHistory.erase(s_Data.keyHistory.begin());
    }

    bool Input::IsValidKey(KeyCode keycode) {
        const int key = static_cast<int>(keycode);
        return key >= 0 && key < static_cast<int>(MAX_KEYS);
    }

    bool Input::IsValidMouseButton(MouseButton button) {
        const int btn = static_cast<int>(button);
        return btn >= 0 && btn < static_cast<int>(MAX_MOUSE_BUTTONS);
    }
} // namespace ash