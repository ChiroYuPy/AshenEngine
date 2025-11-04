#ifndef ASHEN_INSPECTORPANEL_H
#define ASHEN_INSPECTORPANEL_H

#include "Ashen/Core/Types.h"

namespace ash {
    class Node;

    /**
     * @brief ImGui panel for inspecting and editing node properties
     */
    class InspectorPanel {
    public:
        InspectorPanel() = default;
        ~InspectorPanel() = default;

        /**
         * @brief Render the inspector panel
         * @param selectedNode The node to inspect/edit
         */
        void Render(Node* selectedNode);

    private:
        /**
         * @brief Draw common node properties
         * @param node Node to edit
         */
        void DrawNodeProperties(Node* node);

        /**
         * @brief Draw 2D transform properties
         * @param node Node2D to edit
         */
        void DrawNode2DProperties(class Node2D* node);

        /**
         * @brief Draw 3D transform properties
         * @param node Node3D to edit
         */
        void DrawNode3DProperties(class Node3D* node);
    };
}

#endif // ASHEN_INSPECTORPANEL_H
