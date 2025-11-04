#include "Ashen/ImGui/InspectorPanel.h"

#include <imgui.h>
#include <typeinfo>

#include "Ashen/Nodes/Node.h"
#include "Ashen/Nodes/Node2D.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Transform.h"

namespace ash {
    void InspectorPanel::Render(Node* selectedNode) {
        ImGui::Begin("Inspector");

        if (selectedNode) {
            // Draw type info at the top
            const char* typeName = typeid(*selectedNode).name();
            String typeStr(typeName);
            size_t colonPos = typeStr.find_last_of(':');
            if (colonPos != String::npos) {
                typeStr = typeStr.substr(colonPos + 1);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));
            ImGui::TextWrapped("Type: %s", typeStr.c_str());
            ImGui::PopStyleColor();
            ImGui::Separator();

            // Draw common node properties
            DrawNodeProperties(selectedNode);

            // Check if it's a Node2D or Node3D and draw transform properties
            if (auto* node2D = dynamic_cast<Node2D*>(selectedNode)) {
                ImGui::Separator();
                DrawNode2DProperties(node2D);
            } else if (auto* node3D = dynamic_cast<Node3D*>(selectedNode)) {
                ImGui::Separator();
                DrawNode3DProperties(node3D);
            }
        } else {
            ImGui::TextDisabled("No node selected");
            ImGui::TextWrapped("Select a node in the Scene Hierarchy to inspect its properties.");
        }

        ImGui::End();
    }

    void InspectorPanel::DrawNodeProperties(Node* node) {
        if (!node) return;

        if (ImGui::CollapsingHeader("Node Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Name
            char nameBuffer[256];
            String name = node->GetName();
            strncpy_s(nameBuffer, name.c_str(), sizeof(nameBuffer) - 1);
            nameBuffer[sizeof(nameBuffer) - 1] = '\0';

            if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
                node->SetName(nameBuffer);
            }

            // Path (read-only)
            String path = node->GetPath();
            ImGui::BeginDisabled();
            ImGui::InputText("Path", const_cast<char*>(path.c_str()), path.length() + 1, ImGuiInputTextFlags_ReadOnly);
            ImGui::EndDisabled();

            // Processing
            bool isProcessing = node->IsProcessing();
            if (ImGui::Checkbox("Processing Enabled", &isProcessing)) {
                node->SetProcessMode(isProcessing);
            }

            // Groups
            const auto& groups = node->GetGroups();
            if (!groups.empty()) {
                ImGui::Spacing();
                ImGui::TextDisabled("Groups:");
                for (const auto& group : groups) {
                    ImGui::BulletText("%s", group.c_str());
                }
            }
        }
    }

    void InspectorPanel::DrawNode2DProperties(Node2D* node) {
        if (!node) return;

        if (ImGui::CollapsingHeader("Transform 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Position
            Vec2 position = node->GetPosition();
            if (ImGui::DragFloat2("Position", &position.x, 0.1f)) {
                node->SetPosition(position);
            }

            // Rotation (in degrees for easier editing)
            float rotationDeg = node->GetRotationDegrees();
            if (ImGui::DragFloat("Rotation", &rotationDeg, 0.5f)) {
                node->SetRotationDegrees(rotationDeg);
            }

            // Scale
            Vec2 scale = node->GetScale();
            if (ImGui::DragFloat2("Scale", &scale.x, 0.01f, 0.001f, 100.0f)) {
                node->SetScale(scale);
            }

            // Reset buttons
            ImGui::Spacing();
            if (ImGui::Button("Reset Position")) {
                node->SetPosition(Vec2(0.0f, 0.0f));
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Rotation")) {
                node->SetRotationDegrees(0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Scale")) {
                node->SetScale(Vec2(1.0f, 1.0f));
            }

            // Global transform (read-only)
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled("Global Transform:");
            Vec2 globalPos = node->GetGlobalPosition();
            ImGui::Text("Position: (%.2f, %.2f)", globalPos.x, globalPos.y);
            float globalRot = ToDegrees(node->GetGlobalRotation());
            ImGui::Text("Rotation: %.2f°", globalRot);
        }
    }

    void InspectorPanel::DrawNode3DProperties(Node3D* node) {
        if (!node) return;

        if (ImGui::CollapsingHeader("Transform 3D", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Position
            Vec3 position = node->GetPosition();
            if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
                node->SetPosition(position);
            }

            // Rotation (using Euler angles in degrees for easier editing)
            Vec3 eulerRad = node->local_transform.GetEulerAngles();
            Vec3 eulerDeg = Vec3(ToDegrees(eulerRad.x), ToDegrees(eulerRad.y), ToDegrees(eulerRad.z));
            if (ImGui::DragFloat3("Rotation", &eulerDeg.x, 0.5f)) {
                Vec3 newEulerRad = Vec3(ToRadians(eulerDeg.x), ToRadians(eulerDeg.y), ToRadians(eulerDeg.z));
                node->local_transform.SetEulerAngles(newEulerRad);
            }

            // Scale
            Vec3 scale = node->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.0f)) {
                node->SetScale(scale);
            }

            // Reset buttons
            ImGui::Spacing();
            if (ImGui::Button("Reset Position")) {
                node->SetPosition(Vec3(0.0f, 0.0f, 0.0f));
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Rotation")) {
                node->local_transform.SetEulerAngles(Vec3(0.0f, 0.0f, 0.0f));
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Scale")) {
                node->SetScale(Vec3(1.0f, 1.0f, 1.0f));
            }

            // Global transform (read-only)
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled("Global Transform:");
            Vec3 globalPos = node->GetGlobalPosition();
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", globalPos.x, globalPos.y, globalPos.z);

            // Direction vectors
            ImGui::Spacing();
            ImGui::TextDisabled("Direction Vectors:");
            Vec3 right = node->GetRight();
            Vec3 up = node->GetUp();
            Vec3 forward = node->GetForward();
            ImGui::Text("Right:   (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
            ImGui::Text("Up:      (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
            ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
        }
    }
}
