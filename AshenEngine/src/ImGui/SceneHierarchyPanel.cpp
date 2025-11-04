#include "Ashen/ImGui/SceneHierarchyPanel.h"

#include <imgui.h>
#include <typeinfo>

#include "Ashen/Nodes/NodeGraph.h"
#include "Ashen/Nodes/Node.h"

namespace ash {
    void SceneHierarchyPanel::Render(NodeGraph* graph) {
        if (!graph) return;

        ImGui::Begin("Scene Hierarchy");

        Node* root = graph->GetRoot();
        if (root) {
            DrawNodeTree(root);
        } else {
            ImGui::TextDisabled("(Empty Scene)");
        }

        // Right-click on blank space to deselect
        if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
            ClearSelection();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawNodeTree(Node* node) {
        if (!node) return;

        // Build node label with name and type
        const String& nodeName = node->GetName();
        const char* typeName = typeid(*node).name();

        // Remove namespace prefix from type name (platform-dependent)
        String typeStr(typeName);
        size_t colonPos = typeStr.find_last_of(':');
        if (colonPos != String::npos) {
            typeStr = typeStr.substr(colonPos + 1);
        }

        // Create unique ID for ImGui
        String label = nodeName + " (" + typeStr + ")";
        ImGui::PushID(node);

        // Check if this node has children
        bool hasChildren = node->GetChildCount() > 0;

        // Set flags
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                 | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;

        // If no children, show as leaf
        if (!hasChildren) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        // Highlight selected node
        if (m_SelectedNode == node) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);

        // Handle selection
        if (ImGui::IsItemClicked()) {
            m_SelectedNode = node;
        }

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Focus")) {
                m_SelectedNode = node;
            }
            ImGui::Separator();
            ImGui::TextDisabled("Path: %s", node->GetPath().c_str());

            // Show groups if any
            const auto& groups = node->GetGroups();
            if (!groups.empty()) {
                ImGui::Separator();
                ImGui::TextDisabled("Groups:");
                for (const auto& group : groups) {
                    ImGui::BulletText("%s", group.c_str());
                }
            }

            ImGui::EndPopup();
        }

        // Draw children recursively
        if (nodeOpen && hasChildren) {
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                Node* child = node->GetChild(i);
                if (child) {
                    DrawNodeTree(child);
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
