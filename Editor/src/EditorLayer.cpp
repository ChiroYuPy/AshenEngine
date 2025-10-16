// ============================================================================
// DIAGNOSTIC SIGSEGV ImGui - Causes possibles et solutions
// ============================================================================

/*
PROBLÈME IDENTIFIÉ DANS EditorLayer.cpp:

void EditorLayer::DrawComponentProperties() const {
    const auto& components = m_Selection.GetPrimarySelection()->GetAllComponents();

    // 🔴 CRASH ICI! GetPrimarySelection() peut retourner nullptr
    // Vous appelez -> sur un nullptr!
}

Et dans DrawProperties():

void EditorLayer::DrawProperties() {
    const auto selected = m_Selection.GetPrimarySelection();
    if (!selected) {
        ImGui::TextDisabled("(No node selected)");
        return;  // ✅ OK, on évite le crash ici
    }

    // ... code sûr ...

    DrawComponentProperties();  // 🔴 MAIS LÀ, DrawComponentProperties ne vérifie pas!
}
*/

// ============================================================================
// VERSION CORRIGÉE - EditorLayer.cpp
// ============================================================================

#include "EditorLayer.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <ranges>

namespace ash {
    EditorLayer::EditorLayer() {
        m_Scene = std::make_shared<SceneTree>();
        m_Selection.SetSelectionCallback([this](Node* node) {
            OnNodeSelected(node);
        });
    }

    void EditorLayer::OnAttach() {
        Logger::Info("Editor Layer attached");

        const auto scene = std::make_shared<Node>("Scene");

        const auto player = std::make_shared<Node>("Player");
        player->SetLocalPosition(Vec3(0, 0, 0));

        const auto camera = std::make_shared<Node>("Camera");
        camera->SetLocalPosition(Vec3(0, 2, 5));

        const auto light = std::make_shared<Node>("DirectionalLight");
        light->SetLocalPosition(Vec3(5, 10, 5));

        const auto mesh = std::make_shared<Node>("Mesh");
        mesh->SetLocalPosition(Vec3(0, 0, 0));

        scene->AddChild(player);
        scene->AddChild(camera);
        scene->AddChild(light);
        player->AddChild(mesh);

        m_Scene->LoadScene(scene);

        Logger::Info("Test scene loaded with {} nodes", scene->GetChildCount() + 1);
    }

    void EditorLayer::OnDetach() {
        Logger::Info("Editor Layer detached");
        m_Scene->UnloadScene();
    }

    void EditorLayer::OnUpdate(const float ts) {
        m_Scene->Update(ts);

        if (m_ShowGizmo && m_Selection.GetPrimarySelection()) {
            if (Input::IsKeyPressed(Key::T)) m_GizmoMode = GizmoMode::TRANSLATE;
            if (Input::IsKeyPressed(Key::R)) m_GizmoMode = GizmoMode::ROTATE;
            if (Input::IsKeyPressed(Key::S)) m_GizmoMode = GizmoMode::SCALE;

            if (Input::IsKeyPressed(Key::Z) && Input::IsKeyPressed(Key::LeftControl))
                m_ActionStack.Undo();
            if (Input::IsKeyPressed(Key::Y) && Input::IsKeyPressed(Key::LeftControl))
                m_ActionStack.Redo();

            if (Input::IsKeyPressed(Key::Delete))
                DeleteSelectedNode();

            if (Input::IsKeyPressed(Key::D) && Input::IsKeyPressed(Key::LeftControl))
                DuplicateSelectedNode();
        }
    }

    void EditorLayer::OnRender() {
        m_Scene->Render();

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver,
                               ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(1400, 900), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Editor##Main", nullptr, ImGuiWindowFlags_MenuBar)) {
            DrawMainMenuBar();
            DrawToolbar();

            ImGui::Columns(3, "layout", true);
            ImGui::SetColumnWidth(-1, 300);

            DrawSceneHierarchy();

            ImGui::NextColumn();
            ImGui::SetColumnWidth(-1, 600);
            ImGui::TextUnformatted("Viewport");
            ImGui::Dummy(ImVec2(0, 300));

            ImGui::NextColumn();
            ImGui::SetColumnWidth(-1, 300);
            DrawProperties();

            ImGui::Columns(1);
            ImGui::End();
        }
    }

    void EditorLayer::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
            if (e.GetMouseButton() == Mouse::ButtonLeft) {
                // TODO: Raycast dans la scène pour sélectionner
            }
            return false;
        });
    }

    void EditorLayer::DrawMainMenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    m_Scene->UnloadScene();
                    const auto newScene = std::make_shared<Node>("NewScene");
                    m_Scene->LoadScene(newScene);
                }
                if (ImGui::MenuItem("Save Scene"))
                    Logger::Info("Scene saved (not implemented)");

                if (ImGui::MenuItem("Load Scene"))
                    Logger::Info("Scene loaded (not implemented)");

                ImGui::Separator();
                if (ImGui::MenuItem("Delete", "Delete", false, m_Selection.GetPrimarySelection() != nullptr))
                    DeleteSelectedNode();

                if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, m_Selection.GetPrimarySelection() != nullptr))
                    DuplicateSelectedNode();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Empty Node"))
                    CreateNewNode("Node");

                if (ImGui::MenuItem("Cube"))
                    CreateNewNode("Cube");

                if (ImGui::MenuItem("Sphere"))
                    CreateNewNode("Sphere");

                if (ImGui::MenuItem("Light"))
                    CreateNewNode("Light");

                if (ImGui::MenuItem("Camera"))
                    CreateNewNode("Camera");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Hierarchy", nullptr, &m_ShowHierarchy);
                ImGui::MenuItem("Properties", nullptr, &m_ShowProperties);
                ImGui::MenuItem("Gizmo", nullptr, &m_ShowGizmo);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void EditorLayer::DrawToolbar() {
        ImGui::Spacing();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

        const bool isTranslate = m_GizmoMode == GizmoMode::TRANSLATE;
        bool isRotate = m_GizmoMode == GizmoMode::ROTATE;
        bool isScale = m_GizmoMode == GizmoMode::SCALE;

        if (ImGui::Button("Move (T)", ImVec2(80, 0))) m_GizmoMode = GizmoMode::TRANSLATE;
        ImGui::SameLine();
        ImGui::SetItemDefaultFocus();
        if (isTranslate) ImGui::SetItemDefaultFocus();

        if (ImGui::Button("Rotate (R)", ImVec2(80, 0))) m_GizmoMode = GizmoMode::ROTATE;
        ImGui::SameLine();

        if (ImGui::Button("Scale (S)", ImVec2(80, 0))) m_GizmoMode = GizmoMode::SCALE;
        ImGui::SameLine();

        ImGui::Spacing();
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);

        if (ImGui::Button("Create Node", ImVec2(140, 0)))
            ImGui::OpenPopup("create_node");

        if (ImGui::BeginPopupModal("create_node", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char nodeName[256] = "NewNode";
            ImGui::InputText("Node Name", nodeName, sizeof(nodeName));

            if (ImGui::Button("Create", ImVec2(120, 0))) {
                CreateNewNode(nodeName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void EditorLayer::DrawSceneHierarchy() {
        ImGui::Text("Hierarchy");
        ImGui::Separator();

        ImGui::BeginChild("hierarchy", ImVec2(0, -50));

        if (auto root = m_Scene->GetRoot())
            if (const auto scene = m_Scene->GetCurrentScene())
                DrawNodeTree(scene, 0);

        ImGui::EndChild();

        if (ImGui::Button("+ Add Node", ImVec2(-1, 0)))
            if (auto selected = m_Selection.GetPrimarySelection())
                CreateNewNode("NewNode");
    }

    void EditorLayer::DrawNodeTree(Node* node, int depth) {
        if (!node) return;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (node->GetChildCount() == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;
        if (m_Selection.IsSelected(node))
            flags |= ImGuiTreeNodeFlags_Selected;

        const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(node->GetInstanceID())),
                                       flags,
                                       "%s%s",
                                       node->IsLocalActive() ? "" : "[X] ",
                                       node->GetName().c_str());

        if (ImGui::IsItemClicked()) {
            const bool additive = ImGui::GetIO().KeyCtrl;
            m_Selection.Select(node, additive);
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Rename")) {
                // TODO: Implement rename
            }
            if (ImGui::MenuItem("Duplicate")) {
                DuplicateSelectedNode();
            }
            if (ImGui::MenuItem("Delete")) {
                DeleteSelectedNode();
            }
            if (ImGui::MenuItem("Add Child")) {
                CreateNewNode("NewChild");
            }
            ImGui::EndPopup();
        }

        if (opened) {
            for (const auto& child : node->GetChildren()) {
                DrawNodeTree(child.get(), depth + 1);
            }
            ImGui::TreePop();
        }
    }

    void EditorLayer::DrawProperties() {
        ImGui::Text("Properties");
        ImGui::Separator();

        const auto selected = m_Selection.GetPrimarySelection();
        if (!selected) {
            ImGui::TextDisabled("(No node selected)");
            return;
        }

        ImGui::BeginChild("properties", ImVec2(0, -50));

        ImGui::Text("ID: %u", selected->GetInstanceID());
        ImGui::Text("Path: %s", selected->GetPath().c_str());

        char nameBuf[256];
        std::strcpy(nameBuf, selected->GetName().c_str());

        if (ImGui::InputText("Name##NodeName", nameBuf, sizeof(nameBuf))) {
            selected->SetName(nameBuf);
        }

        bool active = selected->IsLocalActive();
        if (ImGui::Checkbox("Active", &active)) {
            selected->SetActive(active);
        }

        bool visible = selected->IsVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            selected->SetVisible(visible);
        }

        ImGui::Separator();
        ImGui::Text("Transform");

        const Vec3 localPos = selected->GetLocalPosition();
        float pos[3] = {localPos.x, localPos.y, localPos.z};
        if (ImGui::DragFloat3("Local Position##local", pos, 0.01f)) {
            selected->SetLocalPosition(Vec3(pos[0], pos[1], pos[2]));
        }

        const Vec3 localRot = selected->GetLocalRotation();
        float rot[3] = {glm::degrees(localRot.x), glm::degrees(localRot.y), glm::degrees(localRot.z)};
        if (ImGui::DragFloat3("Local Rotation##local", rot, 0.5f)) {
            selected->SetLocalRotation(Vec3(glm::radians(rot[0]), glm::radians(rot[1]), glm::radians(rot[2])));
        }

        const Vec3 localScale = selected->GetLocalScale();
        float scale[3] = {localScale.x, localScale.y, localScale.z};
        if (ImGui::DragFloat3("Local Scale##local", scale, 0.01f)) {
            selected->SetLocalScale(Vec3(scale[0], scale[1], scale[2]));
        }

        ImGui::Separator();

        const Vec3 globalPos = selected->GetGlobalPosition();
        ImGui::Text("Global Position: (%.2f, %.2f, %.2f)", globalPos.x, globalPos.y, globalPos.z);

        const Vec3 globalRot = selected->GetGlobalRotation();
        ImGui::Text("Global Rotation: (%.2f, %.2f, %.2f)",
                   glm::degrees(globalRot.x), glm::degrees(globalRot.y), glm::degrees(globalRot.z));

        ImGui::Separator();

        // ============================================================================
        // 🔴 PROBLÈME IDENTIFIÉ ICI
        // DrawComponentProperties() accède à GetPrimarySelection() SANS VÉRIFIER
        // ============================================================================
        DrawComponentProperties();

        ImGui::EndChild();

        if (ImGui::Button("Delete Node", ImVec2(-1, 0))) {
            DeleteSelectedNode();
        }
    }

    // ============================================================================
    // ✅ VERSION CORRIGÉE - Vérifier le pointeur!
    // ============================================================================
    void EditorLayer::DrawComponentProperties() const {
        // 🔴 AVANT: Pas de vérification - CRASH!
        // const auto& components = m_Selection.GetPrimarySelection()->GetAllComponents();

        // ✅ APRÈS: Vérifier d'abord!
        Node* selected = m_Selection.GetPrimarySelection();
        if (!selected) {
            ImGui::TextDisabled("(No node selected)");
            return;
        }

        const auto& components = selected->GetAllComponents();

        if (components.empty()) {
            ImGui::TextDisabled("(No components)");
            return;
        }

        ImGui::Text("Components:");
        ImGui::Separator();

        for (const auto& component : components | std::views::values) {
            std::string compName = component->GetComponentName();
            bool compActive = component->IsActive();

            if (ImGui::CollapsingHeader(compName.c_str())) {
                ImGui::Checkbox(("Active##" + compName).c_str(), &compActive);
                component->SetActive(compActive);

                auto& props = component->GetProperties();
                for (const auto& propName : props.GetAll() | std::views::keys) {
                    ImGui::TextDisabled("%s", propName.c_str());
                }
            }
        }

        if (ImGui::Button("Add Component##dropdown", ImVec2(-1, 0))) {
            ImGui::OpenPopup("add_component");
        }

        if (ImGui::BeginPopup("add_component")) {
            if (ImGui::MenuItem("Transform")) {
                // Already has transform
            }
            if (ImGui::MenuItem("Collider")) {
                // TODO: AddComponent<Collider>
            }
            if (ImGui::MenuItem("Rigidbody")) {
                // TODO: AddComponent<Rigidbody>
            }
            ImGui::EndPopup();
        }
    }

    void EditorLayer::OnNodeSelected(const Node* node) {
        Logger::Info("Selected node: {}", node ? node->GetName() : "None");
    }

    void EditorLayer::CreateNewNode(const std::string& name) {
        const auto newNode = std::make_shared<Node>(name);

        if (const auto selected = m_Selection.GetPrimarySelection()) {
            selected->AddChild(newNode);
        } else if (const auto scene = m_Scene->GetCurrentScene()) {
            scene->AddChild(newNode);
        }

        m_Selection.Select(newNode.get(), false);
        Logger::Info("Created node: {}", name);
    }

    void EditorLayer::DeleteSelectedNode() {
        const auto selected = m_Selection.GetPrimarySelection();
        if (!selected) return;

        if (const auto parent = selected->GetParent()) {
            parent->RemoveChild(selected);
            m_Selection.ClearSelection();
            Logger::Info("Deleted node: {}", selected->GetName());
        }
    }

    void EditorLayer::DuplicateSelectedNode() {
        const auto selected = m_Selection.GetPrimarySelection();
        if (!selected) return;

        const auto duplicate = std::make_shared<Node>(selected->GetName() + "_Copy");
        duplicate->SetLocalTransform(selected->GetLocalTransform());

        if (const auto parent = selected->GetParent()) {
            parent->AddChild(duplicate);
        }

        m_Selection.Select(duplicate.get(), false);
        Logger::Info("Duplicated node: {}", selected->GetName());
    }

    void EditorLayer::DrawTransformGizmo() {
        // Implémentation future
    }
}