#include "Ashen/Nodes/CanvasItem.h"

namespace ash {
    CanvasItem::CanvasItem(String name) : Node(MovePtr(name)) {
    }

    void CanvasItem::Draw() {
        if (!visible) return;
        Node::Draw();
    }

    void CanvasItem::Hide() { visible = false; }

    void CanvasItem::Show() { visible = true; }

    bool CanvasItem::IsVisible() const { return visible; }

    bool CanvasItem::IsVisibleInTree() const {
        if (!visible) return false;
        if (!m_Parent) return visible;
        if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
            return parent->IsVisibleInTree();

        return visible;
    }

    int CanvasItem::GetZIndex() const { return z_index; }

    void CanvasItem::SetZIndex(const int z) { z_index = z; }

    int CanvasItem::GetGlobalZIndex() const {
        if (!z_as_relative || !m_Parent) return z_index;
        if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
            return parent->GetGlobalZIndex() + z_index;

        return z_index;
    }
}
