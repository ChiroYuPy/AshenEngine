#include "Ashen/Graphics/UI/UISystem.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"

namespace ash {

UISystem::Data UISystem::s_Data;

void UISystem::Init() {
    s_Data.Root = nullptr;
    s_Data.Camera = nullptr;
    s_Data.ScreenSize = Vec2(1280.0f, 720.0f);
    s_Data.MousePosition = Vec2(0.0f);
    s_Data.FocusedControl = nullptr;
    s_Data.HoveredControl = nullptr;
    s_Data.IsRendering = false;

    Logger::Info("UISystem initialized");
}

void UISystem::Shutdown() {
    s_Data.Root.reset();
    s_Data.Camera.reset();
    s_Data.FocusedControl = nullptr;
    s_Data.HoveredControl = nullptr;

    Logger::Info("UISystem shutdown");
}

void UISystem::BeginFrame() {
    s_Data.MousePosition = Input::GetMousePosition();
    UpdateHoveredControl();
}

void UISystem::EndFrame() {
    // Nothing for now
}

void UISystem::Update(float delta) {
    if (!s_Data.Root) return;
    ProcessNode(s_Data.Root, delta);
}

void UISystem::Render() {
    if (!s_Data.Root || !s_Data.Camera) return;

    // Important: Ne pas appeler BeginScene/EndScene ici
    // Le UILayer doit gérer ça
    s_Data.IsRendering = true;
    RenderNode(s_Data.Root);
    s_Data.IsRendering = false;
}

bool UISystem::HandleEvent(Event& event) {
    if (!s_Data.Root) return false;

    InputNode(s_Data.Root, event);
    return false;
}

void UISystem::SetRoot(std::shared_ptr<Node> root) {
    s_Data.Root = root;
    if (root && !root->m_IsReady) {
        root->OnReady();
        root->m_IsReady = true;
    }
}

std::shared_ptr<Node> UISystem::GetRoot() {
    return s_Data.Root;
}

void UISystem::SetCamera(std::shared_ptr<Camera> camera) {
    s_Data.Camera = camera;
}

std::shared_ptr<Camera> UISystem::GetCamera() {
    return s_Data.Camera;
}

void UISystem::SetScreenSize(const Vec2& size) {
    s_Data.ScreenSize = size;
}

Vec2 UISystem::GetScreenSize() {
    return s_Data.ScreenSize;
}

void UISystem::SetFocusedControl(Control* control) {
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

Control* UISystem::GetFocusedControl() {
    return s_Data.FocusedControl;
}

Vec2 UISystem::GetMousePosition() {
    return s_Data.MousePosition;
}

Control* UISystem::GetHoveredControl() {
    return s_Data.HoveredControl;
}

// Private methods

void UISystem::ProcessNode(std::shared_ptr<Node> node, float delta) {
    if (!node || !node->IsVisible() || !node->IsProcessEnabled()) return;
    
    node->OnProcess(delta);
    
    for (const auto& child : node->GetChildren()) {
        ProcessNode(child, delta);
    }
}

void UISystem::RenderNode(std::shared_ptr<Node> node) {
    if (!node || !node->IsVisibleInTree()) return;
    
    node->OnDraw();
    
    for (const auto& child : node->GetChildren()) {
        RenderNode(child);
    }
}

void UISystem::InputNode(std::shared_ptr<Node> node, Event& event) {
    if (!node || !node->IsVisible()) return;
    
    // Process children first (top to bottom)
    auto children = node->GetChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        InputNode(*it, event);
    }
    
    node->OnInput(event);
}

void UISystem::UpdateHoveredControl() {
    Control* newHovered = FindControlAt(s_Data.MousePosition, s_Data.Root);
    
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

Control* UISystem::FindControlAt(const Vec2& pos, std::shared_ptr<Node> node) {
    if (!node || !node->IsVisible()) return nullptr;
    
    // Check children first (top to bottom)
    auto children = node->GetChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if (Control* found = FindControlAt(pos, *it)) {
            return found;
        }
    }
    
    // Check this node if it's a control
    if (auto control = std::dynamic_pointer_cast<Control>(node)) {
        if (control->GetMouseFilter()) {
            BBox2 rect = control->GetGlobalRect();
            if (rect.Contains(pos)) {
                return control.get();
            }
        }
    }
    
    return nullptr;
}

} // namespace ash