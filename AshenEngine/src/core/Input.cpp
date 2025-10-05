#include "Ashen/core/Window.h"
#include "Ashen/core/Input.h"

namespace ash {
    void Input::Init(const Window &window) {
        s_Window = window.GetHandle();
        s_Keys.reset();
        s_KeysPrevious.reset();
        s_MouseButtons.reset();
        s_MouseButtonsPrevious.reset();
        s_FirstMouseUpdate = true;

        double x, y;
        glfwGetCursorPos(window.GetHandle(), &x, &y);
        s_MousePosition = {static_cast<float>(x), static_cast<float>(y)};
        s_MousePositionPrevious = s_MousePosition;
    }

    void Input::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(OnKeyPressed);
        dispatcher.Dispatch<KeyReleasedEvent>(OnKeyReleased);
        dispatcher.Dispatch<MouseButtonPressedEvent>(OnMouseButtonPressed);
        dispatcher.Dispatch<MouseButtonReleasedEvent>(OnMouseButtonReleased);
        dispatcher.Dispatch<MouseMovedEvent>(OnMouseMoved);
        dispatcher.Dispatch<MouseScrolledEvent>(OnMouseScrolled);
    }

    void Input::Update() {
        // Save previous frame state
        s_KeysPrevious = s_Keys;
        s_MouseButtonsPrevious = s_MouseButtons;

        // Calculate mouse delta
        if (s_FirstMouseUpdate) {
            s_MouseDelta = {0.0f, 0.0f};
            s_FirstMouseUpdate = false;
        } else {
            s_MouseDelta = s_MousePosition - s_MousePositionPrevious;
        }
        s_MousePositionPrevious = s_MousePosition;

        // Reset scroll delta
        s_MouseScrollDelta = 0.0f;
    }

    bool Input::OnKeyPressed(const KeyPressedEvent &e) {
        const int key = e.GetKeyCode();
        if (key >= 0 && key < MAX_KEYS)
            s_Keys[key] = true;
        return false; // Don't block event
    }

    bool Input::OnKeyReleased(const KeyReleasedEvent &e) {
        const int key = e.GetKeyCode();
        if (key >= 0 && key < MAX_KEYS)
            s_Keys[key] = false;
        return false;
    }

    bool Input::OnMouseButtonPressed(const MouseButtonPressedEvent &e) {
        const int button = e.GetMouseButton();
        if (button >= 0 && button < MAX_MOUSE_BUTTONS)
            s_MouseButtons[button] = true;
        return false;
    }

    bool Input::OnMouseButtonReleased(const MouseButtonReleasedEvent &e) {
        const int button = e.GetMouseButton();
        if (button >= 0 && button < MAX_MOUSE_BUTTONS)
            s_MouseButtons[button] = false;
        return false;
    }

    bool Input::OnMouseMoved(const MouseMovedEvent &e) {
        s_MousePosition = {e.GetX(), e.GetY()};
        return false;
    }

    bool Input::OnMouseScrolled(const MouseScrolledEvent &e) {
        s_MouseScrollDelta = e.GetYOffset();
        return false;
    }

    // Public query functions
    bool Input::IsKeyPressed(const int keycode) {
        if (keycode < 0 || keycode >= MAX_KEYS)
            return false;
        return s_Keys[keycode];
    }

    bool Input::IsKeyJustPressed(const int keycode) {
        if (keycode < 0 || keycode >= MAX_KEYS)
            return false;
        return s_Keys[keycode] && !s_KeysPrevious[keycode];
    }

    bool Input::IsKeyJustReleased(const int keycode) {
        if (keycode < 0 || keycode >= MAX_KEYS)
            return false;
        return !s_Keys[keycode] && s_KeysPrevious[keycode];
    }

    bool Input::IsMouseButtonPressed(const int button) {
        if (button < 0 || button >= MAX_MOUSE_BUTTONS)
            return false;
        return s_MouseButtons[button];
    }

    bool Input::IsMouseButtonJustPressed(const int button) {
        if (button < 0 || button >= MAX_MOUSE_BUTTONS)
            return false;
        return s_MouseButtons[button] && !s_MouseButtonsPrevious[button];
    }

    bool Input::IsMouseButtonJustReleased(const int button) {
        if (button < 0 || button >= MAX_MOUSE_BUTTONS)
            return false;
        return !s_MouseButtons[button] && s_MouseButtonsPrevious[button];
    }

    glm::vec2 Input::GetMousePosition() {
        return s_MousePosition;
    }

    glm::vec2 Input::GetMouseDelta() {
        return s_MouseDelta;
    }

    float Input::GetMouseScrollDelta() {
        return s_MouseScrollDelta;
    }

    void Input::SetCursorMode(const CursorMode mode) {
        if (!s_Window) return;
        glfwSetInputMode(s_Window, GLFW_CURSOR, static_cast<int>(mode));
    }

    void Input::ResetMouseDelta() {
        s_MouseDelta = {0.0f, 0.0f};
        s_MousePositionPrevious = s_MousePosition;
        s_FirstMouseUpdate = true;
    }

    void Input::EnableRawMouseMotion(const bool enabled) {
        if (!s_Window) return;
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(s_Window, GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
    }

    void Input::EnableStickyKeys(const bool enabled) {
        if (!s_Window) return;
        glfwSetInputMode(s_Window, GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
    }

    void Input::EnableStickyMouseButtons(const bool enabled) {
        if (!s_Window) return;
        glfwSetInputMode(s_Window, GLFW_STICKY_MOUSE_BUTTONS, enabled ? GLFW_TRUE : GLFW_FALSE);
    }
}