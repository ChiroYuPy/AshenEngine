

#include "Ashen/Scene/SceneTree.h"

namespace ash {
    SceneTree::SceneTree() {
        m_Root = std::make_shared<Node>("__ROOT__");
        m_Root->m_Tree = this;
    }

    SceneTree::~SceneTree() = default;

    void SceneTree::LoadScene(const NodeRef& scene) {
        if (m_CurrentScene)
            m_Root->RemoveChild(m_CurrentScene.get());

        m_CurrentScene = scene;
        if (m_CurrentScene) {
            m_Root->AddChild(m_CurrentScene);
        }
    }

    void SceneTree::UnloadScene() {
        if (m_CurrentScene) {
            m_Root->RemoveChild(m_CurrentScene.get());
            m_CurrentScene = nullptr;
        }
    }

    Node* SceneTree::FindNodeByID(uint32_t id) const {
        //TODO
        return nullptr;
    }

    Node* SceneTree::FindNodeByName(const std::string& name) const {
        if (m_CurrentScene && m_CurrentScene->GetName() == name)
            return m_CurrentScene.get();
        return m_CurrentScene ? m_CurrentScene->FindChild(name, true).get() : nullptr;
    }

    void SceneTree::Update(const float ts) const {
        if (m_CurrentScene) {
            m_CurrentScene->OnUpdate(ts);
            for (const auto& child : m_CurrentScene->GetChildren()) {
                child->OnUpdate(ts);
            }
        }
    }

    void SceneTree::Render() const {
        if (m_CurrentScene) {
            m_CurrentScene->OnRender();
            for (const auto& child : m_CurrentScene->GetChildren()) {
                child->OnRender();
            }
        }
    }
}
