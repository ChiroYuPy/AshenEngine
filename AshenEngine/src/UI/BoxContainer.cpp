#include "Ashen/UI/BoxContainer.h"

namespace ash {
    BoxContainer::BoxContainer(BoxOrientation orientation)
        : Control("BoxContainer")
        , m_Orientation(orientation) {
    }

    void BoxContainer::_Process(float delta) {
        UpdateLayout();
        Control::_Process(delta);
    }

    void BoxContainer::UpdateLayout() {
        Vector<Control*> visibleChildren;

        // Collect visible children
        for (const auto& childPtr : GetChildren()) {
            Node* child = childPtr.get();
            if (Control* control = dynamic_cast<Control*>(child)) {
                visibleChildren.push_back(control);
            }
        }

        if (visibleChildren.empty()) return;

        float currentPos = 0.0f;

        if (m_Orientation == BoxOrientation::Horizontal) {
            // Horizontal layout
            for (Control* child : visibleChildren) {
                Vec2 childSize = child->GetSize();
                child->SetGlobalPosition(GetGlobalPosition() + Vec2(currentPos, 0.0f));
                currentPos += childSize.x + m_Separation;
            }

            // Update container size
            float totalWidth = currentPos - m_Separation;
            float maxHeight = 0.0f;
            for (Control* child : visibleChildren) {
                maxHeight = Max(maxHeight, child->GetSize().y);
            }
            size = Vec2(totalWidth, maxHeight);

        } else {
            // Vertical layout
            for (Control* child : visibleChildren) {
                Vec2 childSize = child->GetSize();
                child->SetGlobalPosition(GetGlobalPosition() + Vec2(0.0f, currentPos));
                currentPos += childSize.y + m_Separation;
            }

            // Update container size
            float maxWidth = 0.0f;
            float totalHeight = currentPos - m_Separation;
            for (Control* child : visibleChildren) {
                maxWidth = Max(maxWidth, child->GetSize().x);
            }
            size = Vec2(maxWidth, totalHeight);
        }
    }
}