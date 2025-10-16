#ifndef ASHEN_EDITORSELECTION_H
#define ASHEN_EDITORSELECTION_H

#include <functional>
#include "Ashen/Scene/Node.h"

namespace ash {
    class EditorSelection {
    public:
        using SelectionCallback = std::function<void(Node*)>;

        void Select(Node* node, bool additive = false);
        void Deselect(const Node* node);
        void ClearSelection();

        [[nodiscard]] Node* GetPrimarySelection() const {
            return m_Selected.empty() ? nullptr : m_Selected[0];
        }

        [[nodiscard]] const std::vector<Node*>& GetSelected() const { return m_Selected; }
        [[nodiscard]] bool IsSelected(const Node* node) const;

        void SetSelectionCallback(const SelectionCallback& cb) { m_Callback = cb; }

    private:
        std::vector<Node*> m_Selected{};
        SelectionCallback m_Callback{};
    };
}

#endif //ASHEN_EDITORSELECTION_H