#include "EditorSelection.h"
#include <algorithm>

namespace ash {
    void EditorSelection::Select(Node* node, bool additive) {
        if (!node) return;

        if (!additive)
            m_Selected.clear();

        if (IsSelected(node)) {
            if (additive) Deselect(node);

        } else
            m_Selected.push_back(node);

        if (m_Callback)
            m_Callback(GetPrimarySelection());
    }

    void EditorSelection::Deselect(const Node* node) {
        auto it = std::ranges::find(m_Selected, node);
        if (it != m_Selected.end())
            m_Selected.erase(it);
    }

    void EditorSelection::ClearSelection() {
        m_Selected.clear();
        if (m_Callback) {
            m_Callback(nullptr);
        }
    }

    bool EditorSelection::IsSelected(const Node* node) const {
        return std::ranges::find(m_Selected, node) != m_Selected.end();
    }
}