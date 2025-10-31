#include "Ashen/Nodes/Containers.h"

namespace ash {
    Container::Container(String name) : Control(MovePtr(name)) {
    }

    void Container::UpdateChildrenLayout() {
        for (const auto &child: m_Children)
            if (const auto ctrl = dynamic_cast<Container *>(child.get()))
                ctrl->UpdateChildrenLayout();
    }

    void Container::OnResized() {
        UpdateChildrenLayout();
    }

    VBoxContainer::VBoxContainer(String name) : Container(MovePtr(name)) {
    }

    void VBoxContainer::UpdateChildrenLayout() {
        float y_offset = 0.f;

        for (const auto &child: m_Children) {
            const auto ctrl = dynamic_cast<Control *>(child.get());
            if (!ctrl || !ctrl->visible) continue;

            ctrl->position = Vec2(0.f, y_offset);
            Vec2 child_size = ctrl->GetSize();
            child_size.x = size.x;
            ctrl->SetSize(child_size);

            y_offset += child_size.y + separation;
        }
    }

    HBoxContainer::HBoxContainer(String name) : Container(MovePtr(name)) {
    }

    void HBoxContainer::UpdateChildrenLayout() {
        float x_offset = 0.f;

        for (const auto &child: m_Children) {
            const auto ctrl = dynamic_cast<Control *>(child.get());
            if (!ctrl || !ctrl->visible) continue;

            ctrl->position = Vec2(x_offset, 0.f);
            Vec2 child_size = ctrl->GetSize();
            child_size.y = size.y;
            ctrl->SetSize(child_size);

            x_offset += child_size.x + separation;
        }
    }
}
