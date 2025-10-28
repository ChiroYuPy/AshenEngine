#include "Ashen/Scene/SceneTree.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    SceneTree::SceneTree() {
        m_Root = MakeRef<Node>("Root");
        // SUPPRIMER: m_Root->_EnterTree();
        // Le nouveau système n'a pas besoin de _EnterTree
        m_Root->OnReady();  // Appeler OnReady directement
        m_Paused = false;
        m_FocusedControl = nullptr;

        Logger::Info("SceneTree created");
    }

    SceneTree::~SceneTree() {
        // SUPPRIMER: if (m_Root) m_Root->_ExitTree();
        m_Root.reset();
        m_CurrentScene.reset();
        m_QueuedForDeletion.clear();
        m_Groups.clear();
        m_UUIDMap.clear();

        Logger::Info("SceneTree destroyed");
    }

    void SceneTree::ProcessNode(const Ref<Node> &node, const float delta) {
        if (!node) return;

        // CHANGER: node->_Process(delta);
        node->OnProcess(delta);  // Utiliser OnProcess

        auto children = node->GetChildren();
        for (const auto &child: children) {
            ProcessNode(child, delta);
        }
    }

    void SceneTree::PhysicsProcessNode(const Ref<Node> &node, const float delta) {
        if (!node) return;

        // CHANGER: node->_PhysicsProcess(delta);
        // Le nouveau système n'a pas PhysicsProcess, on peut le supprimer
        // Ou créer une méthode virtuelle OnPhysicsProcess si nécessaire

        auto children = node->GetChildren();
        for (const auto &child: children) {
            PhysicsProcessNode(child, delta);
        }
    }

    void SceneTree::InputNode(const Ref<Node> &node, Event &event) {
        if (!node) return;

        // CHANGER: node->_Input(event);
        node->OnInput(event);  // Utiliser OnInput

        auto children = node->GetChildren();
        for (const auto &child: children) {
            InputNode(child, event);
        }
    }

    void SceneTree::NotifyNodeEnterTree(Node *node) {
        if (!node) return;

        // Ajouter au map UUID
        m_UUIDMap[node->GetUUID()] = node;

        if (OnNodeAdded) {
            Logger::Debug() << "Node entered tree: " << node->GetName();
        }
    }

    void SceneTree::NotifyNodeExitTree(Node *node) {
        if (!node) return;

        m_UUIDMap.erase(node->GetUUID());

        for (auto &[groupName, nodes]: m_Groups) {
            nodes.erase(
                std::remove(nodes.begin(), nodes.end(), node),
                nodes.end()
            );
        }

        if (OnNodeRemoved) {
            Logger::Debug() << "Node exited tree: " << node->GetName();
        }
    }

    void SceneTree::QueueDelete(const Ref<Node> &node) {
        if (!node) return;
        // SUPPRIMER: node->QueueFree();
        // Juste ajouter à la liste
        m_QueuedForDeletion.push_back(node);
    }

    void SceneTree::SetFocusedControl(Control *control) {
        if (m_FocusedControl == control) return;

        if (m_FocusedControl) {
            m_FocusedControl->ReleaseFocus();
        }

        m_FocusedControl = control;

        if (m_FocusedControl) {
            // OK car SceneTree est friend de Control
            m_FocusedControl->m_HasFocus = true;
            if (m_FocusedControl->OnFocusEntered) {
                m_FocusedControl->OnFocusEntered();
            }
        }
    }

    Ref<Node> SceneTree::FindNodeByPath(const String &path) const {
        if (!m_Root) return nullptr;

        Vector<String> parts;
        String current;
        for (char c: path) {
            if (c == '/') {
                if (!current.empty()) {
                    parts.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            parts.push_back(current);
        }

        Ref<Node> current_node = m_Root;
        for (const auto &part: parts) {
            if (part.empty()) continue;
            // CHANGER: current_node = current_node->GetChild(part);
            current_node = current_node->FindChild(part, false);  // Utiliser FindChild
            if (!current_node) return nullptr;
        }

        return current_node;
    }

    Ref<Node> SceneTree::FindNodeByUUID(const UUID &uuid) const {
        auto it = m_UUIDMap.find(uuid);
        if (it != m_UUIDMap.end()) {
            // Le problème ici est qu'on stocke des raw pointers
            // Pour l'instant, retourner nullptr
            Logger::Warn() << "FindNodeByUUID: Feature not fully implemented";
            return nullptr;
        }
        return nullptr;
    }

} // namespace ash