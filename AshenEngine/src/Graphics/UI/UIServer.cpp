#include "Ashen/Graphics/UI/UIServer.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Scene/Control.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    UIServer::UIData UIServer::s_Data;

    void UIServer::Init() {
        s_Data.Camera = nullptr;
        s_Data.ScreenSize = Vec2(1280.0f, 720.0f);
        s_Data.MousePosition = Vec2(0.0f);
        s_Data.FocusedControl = nullptr;
        s_Data.HoveredControl = nullptr;
        s_Data.TooltipText = "";
        s_Data.TooltipPosition = Vec2(0.0f);
        s_Data.TooltipTimer = 0.0f;
        s_Data.ClipStack.clear();

        Logger::Info("UIServer initialized");
    }

    void UIServer::Shutdown() {
        s_Data.Camera.reset();
        s_Data.FocusedControl = nullptr;
        s_Data.HoveredControl = nullptr;
        s_Data.ClipStack.clear();

        Logger::Info("UIServer shutdown");
    }

    void UIServer::BeginFrame() {
        // Reset per-frame data
        s_Data.TooltipTimer = 0.0f;
    }

    void UIServer::EndFrame() {
        // Draw tooltip if needed
        DrawTooltip();
    }

    void UIServer::RenderTree(const Ref<Node> &root) {
        if (!root) return;

        // Start 2D rendering if we have a camera
        if (s_Data.Camera) {
            Renderer2D::BeginScene(*s_Data.Camera);
        }

        // Render the node tree recursively
        RenderNode(root);

        if (s_Data.Camera) {
            Renderer2D::EndScene();
        }
    }

    void UIServer::RenderNode(const Ref<Node> &node) {
        if (!node || !node->IsVisible()) return;

        // Check if this is a Control node
        if (const auto control = std::dynamic_pointer_cast<Control>(node)) {
            // Set up clipping if enabled
            if (control->GetClipContents()) {
                PushClipRect(control->GetGlobalRect());
            }

            // Draw the control
            control->_Draw();

            // Render children
            for (const auto &child: node->GetChildren()) {
                RenderNode(child);
            }

            // Pop clipping
            if (control->GetClipContents()) {
                PopClipRect();
            }
        } else {
            // Not a control, just render children
            for (const auto &child: node->GetChildren()) {
                RenderNode(child);
            }
        }
    }

    void UIServer::SetCamera(const Ref<Camera> &camera) {
        s_Data.Camera = camera;
    }

    Ref<Camera> UIServer::GetCamera() {
        return s_Data.Camera;
    }

    void UIServer::SetScreenSize(const Vec2 &size) {
        s_Data.ScreenSize = size;
    }

    Vec2 UIServer::GetScreenSize() {
        return s_Data.ScreenSize;
    }

    void UIServer::ProcessInput(Event &event) {
        EventDispatcher dispatcher(event);

        // Update mouse position
        dispatcher.Dispatch<MouseMovedEvent>([](const MouseMovedEvent &e) {
            s_Data.MousePosition = Vec2(e.GetX(), e.GetY());
            return false;
        });

        // TODO: Dispatch input to controls
    }

    Control *UIServer::GetControlAtPosition(const Vec2 &pos, const Ref<Node> &root) {
        if (!root) return nullptr;

        // Check children in reverse order (top to bottom)
        const auto &children = root->GetChildren();
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if (const auto control = GetControlAtPosition(pos, *it))
                return control;
        }

        // Check this node if it's a control
        if (const auto control = std::dynamic_pointer_cast<Control>(root)) {
            if (control->IsVisible() && control->GetMouseFilter()) {
                const BBox2 rect = control->GetGlobalRect();
                if (rect.Contains(pos))
                    return control.get();
            }
        }

        return nullptr;
    }

    void UIServer::SetFocusedControl(Control *control) {
        if (s_Data.FocusedControl == control) return;

        if (s_Data.FocusedControl) {
            s_Data.FocusedControl->ReleaseFocus();
        }

        s_Data.FocusedControl = control;

        if (s_Data.FocusedControl) {
            s_Data.FocusedControl->m_HasFocus = true;
            if (s_Data.FocusedControl->OnFocusEntered) {
                s_Data.FocusedControl->OnFocusEntered();
            }
        }
    }

    Control *UIServer::GetFocusedControl() {
        return s_Data.FocusedControl;
    }

    Vec2 UIServer::GetMousePosition() {
        return s_Data.MousePosition;
    }

    Control *UIServer::GetHoveredControl() {
        return s_Data.HoveredControl;
    }

    void UIServer::UpdateHoveredControl(const Ref<Node> &root) {
        Control *newHovered = GetControlAtPosition(s_Data.MousePosition, root);

        if (s_Data.HoveredControl != newHovered) {
            if (s_Data.HoveredControl) {
                s_Data.HoveredControl->m_IsHovered = false;
                if (s_Data.HoveredControl->OnMouseExited) {
                    s_Data.HoveredControl->OnMouseExited();
                }
            }

            s_Data.HoveredControl = newHovered;

            if (s_Data.HoveredControl) {
                s_Data.HoveredControl->m_IsHovered = true;
                if (s_Data.HoveredControl->OnMouseEntered) {
                    s_Data.HoveredControl->OnMouseEntered();
                }
            }
        }
    }

    void UIServer::SetTooltip(const String &text, const Vec2 &position) {
        s_Data.TooltipText = text;
        s_Data.TooltipPosition = position;
        s_Data.TooltipTimer = 0.0f;
    }

    void UIServer::ClearTooltip() {
        s_Data.TooltipText = "";
        s_Data.TooltipTimer = 0.0f;
    }

    void UIServer::DrawTooltip() {
        if (s_Data.TooltipText.empty()) return;
        if (s_Data.TooltipTimer < 0.5f) return; // Wait 0.5s before showing tooltip

        // Simple tooltip rendering
        const Vec2 size(200.0f, 30.0f);
        const BBox2 rect(s_Data.TooltipPosition, s_Data.TooltipPosition + size);

        DrawRect(rect, Vec4(0.1f, 0.1f, 0.1f, 0.9f));
        DrawRectOutline(rect, Vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f);
        DrawText(s_Data.TooltipText, s_Data.TooltipPosition + Vec2(5.0f, 8.0f), 12.0f, Vec4(1.0f));
    }

    void UIServer::DrawControl(Control *control) {
        if (!control) return;
        control->_Draw();
    }

    void UIServer::DrawRect(const BBox2 &rect, const Vec4 &color) {
        Renderer2D::DrawQuad(Vec3(rect.min.x, rect.min.y, 0.0f), rect.Size(), color);
    }

    void UIServer::DrawRectOutline(const BBox2 &rect, const Vec4 &color, const float width) {
        const float oldWidth = 2.0f; // TODO: Get current line width
        Renderer2D::SetLineWidth(width);
        Renderer2D::DrawRect(Vec3(rect.min.x, rect.min.y, 0.0f), rect.Size(), color);
        Renderer2D::SetLineWidth(oldWidth);
    }

    void UIServer::DrawRoundedRect(const BBox2 &rect, const Vec4 &color, float radius) {
        // For now, just draw a regular rect
        // TODO: Implement proper rounded rectangles
        DrawRect(rect, color);
    }

    void UIServer::DrawTexture(const Ref<Texture2D> &texture, const BBox2 &rect, const Vec4 &modulate) {
        if (!texture) return;
        Renderer2D::DrawQuad(Vec3(rect.min.x, rect.min.y, 0.0f), rect.Size(), texture, modulate);
    }

    void UIServer::DrawText(const String &text, const Vec2 &position, const float fontSize, const Vec4 &color) {
        // TODO: Implement proper text rendering
        // For now, draw a placeholder rect to show where text would be
        const Vec2 size(text.length() * fontSize * 0.6f, fontSize * 1.2f);
        BBox2 textRect(position, position + size);
        // DrawRect(textRect, Vec4(color.r, color.g, color.b, 0.1f));

        // This is a placeholder - you'll need to implement actual text rendering
        // using a font atlas/bitmap font/FreeType etc.
    }

    void UIServer::PushClipRect(const BBox2 &rect) {
        s_Data.ClipStack.push_back(rect);
        // TODO: Set OpenGL scissor test
    }

    void UIServer::PopClipRect() {
        if (!s_Data.ClipStack.empty()) {
            s_Data.ClipStack.pop_back();
        }
        // TODO: Restore previous scissor rect or disable
    }

    BBox2 UIServer::GetCurrentClipRect() {
        if (s_Data.ClipStack.empty()) {
            return BBox2(Vec2(0.0f), s_Data.ScreenSize);
        }
        return s_Data.ClipStack.back();
    }

    bool UIServer::IsClipping() {
        return !s_Data.ClipStack.empty();
    }
} // namespace ash