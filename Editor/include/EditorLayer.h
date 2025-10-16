#ifndef ASHEN_EDITOR_LAYER_H
#define ASHEN_EDITOR_LAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Scene/Node.h"
#include "Ashen/Scene/SceneTree.h"
#include "EditorSelection.h"
#include "EditorActions.h"

namespace ash {
    class EditorLayer final : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnRender() override;
        void OnEvent(Event& event) override;

    private:
        void DrawMainMenuBar();
        void DrawSceneHierarchy();
        void DrawProperties();
        void DrawToolbar();
        void DrawTransformGizmo();

        void OnNodeSelected(const Node* node);
        void CreateNewNode(const std::string& name);
        void DeleteSelectedNode();
        void DuplicateSelectedNode();

        void DrawNodeTree(Node* node, int depth = 0);
        void DrawComponentProperties() const;

        Ref<SceneTree> m_Scene;
        EditorSelection m_Selection;
        EditorActionStack m_ActionStack;

        bool m_ShowProperties = true;
        bool m_ShowHierarchy = true;
        bool m_ShowGizmo = true;

        // Gizmo state
        enum class GizmoMode { TRANSLATE, ROTATE, SCALE };
        GizmoMode m_GizmoMode = GizmoMode::TRANSLATE;
        Vec3 m_GizmoStartPos{};
        bool m_GizmoActive = false;
    };
}

#endif // ASHEN_EDITOR_LAYER_H