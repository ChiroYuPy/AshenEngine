#ifndef ASHEN_SCENEHIERARCHYPANEL_H
#define ASHEN_SCENEHIERARCHYPANEL_H

#include "Ashen/Core/Types.h"

namespace ash {
    class Node;
    class NodeGraph;

    /**
     * @brief ImGui panel for viewing and interacting with the scene node hierarchy
     */
    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel() = default;
        ~SceneHierarchyPanel() = default;

        /**
         * @brief Render the scene hierarchy panel
         * @param graph The node graph to display
         */
        void Render(NodeGraph* graph);

        /**
         * @brief Get the currently selected node
         * @return Pointer to selected node or nullptr
         */
        [[nodiscard]] Node* GetSelectedNode() const { return m_SelectedNode; }

        /**
         * @brief Set the selected node
         * @param node Node to select
         */
        void SetSelectedNode(Node* node) { m_SelectedNode = node; }

        /**
         * @brief Clear the selection
         */
        void ClearSelection() { m_SelectedNode = nullptr; }

    private:
        /**
         * @brief Recursively draw a node and its children in the tree
         * @param node Node to draw
         */
        void DrawNodeTree(Node* node);

        Node* m_SelectedNode = nullptr;
    };
}

#endif // ASHEN_SCENEHIERARCHYPANEL_H
