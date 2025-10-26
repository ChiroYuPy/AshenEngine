#include "Ashen/Scene/SceneTree.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    SceneTree::SceneTree() {
        // Create an empty root node
        m_Root = MakeRef<Node>("Root");
        m_Root->_EnterTree();
        m_Paused = false;
        m_FocusedControl = nullptr;

        Logger::Info("SceneTree created");
    }

    SceneTree::~SceneTree() {
        if (m_Root) {
            m_Root->_ExitTree();
        }
        m_Root.reset();
        m_CurrentScene.reset();
        m_QueuedForDeletion.clear();
        m_Groups.clear();
        m_UUIDMap.clear();

        Logger::Info("SceneTree destroyed");
    }

    void SceneTree::ChangeScene(const Ref<Node> &newRoot) {
        if (m_CurrentScene) {
            m_Root->RemoveChild(m_CurrentScene);
        }

        m_CurrentScene = newRoot;

        if (m_CurrentScene) {
            m_Root->AddChild(m_CurrentScene);
        }

        if (OnTreeChanged) {
            OnTreeChanged();
        }
    }

    void SceneTree::ChangeSceneDeferred(const Ref<Node> &newRoot) {
        m_DeferredSceneChange = newRoot;
    }

    void SceneTree::Process(const float delta) {
        if (m_Paused) return;

        // Apply deferred scene change
        if (m_DeferredSceneChange) {
            ChangeScene(m_DeferredSceneChange);
            m_DeferredSceneChange.reset();
        }

        // Process the tree
        if (m_Root) {
            ProcessNode(m_Root, delta);
        }

        // Process queued deletions
        ProcessQueuedDeletions();
    }

    void SceneTree::PhysicsProcess(const float delta) {
        if (m_Paused) return;

        if (m_Root) {
            PhysicsProcessNode(m_Root, delta);
        }
    }

    void SceneTree::Input(Event &event) {
        if (m_Root) {
            InputNode(m_Root, event);
        }
    }

    void SceneTree::ProcessNode(const Ref<Node> &node, const float delta) {
        if (!node) return;

        node->_Process(delta);

        // Process children (make a copy to handle modifications during iteration)
        auto children = node->GetChildren();
        for (const auto &child: children) {
            ProcessNode(child, delta);
        }
    }

    void SceneTree::PhysicsProcessNode(const Ref<Node> &node, const float delta) {
        if (!node) return;

        node->_PhysicsProcess(delta);

        auto children = node->GetChildren();
        for (const auto &child: children) {
            PhysicsProcessNode(child, delta);
        }
    }

    void SceneTree::InputNode(const Ref<Node> &node, Event &event) {
        if (!node) return;

        node->_Input(event);

        auto children = node->GetChildren();
        for (const auto &child: children) {
            InputNode(child, event);
        }
    }

    void SceneTree::NotifyNodeEnterTree(Node *node) {
        if (!node) return;

        // Add to UUID map
        m_UUIDMap[node->GetUUID()] = node;

        if (OnNodeAdded) {
            // Need to get shared_ptr from raw pointer - this is tricky
            // For now, just log
            Logger::Debug() << "Node entered tree: " << node->GetName();
        }
    }

    void SceneTree::NotifyNodeExitTree(Node *node) {
        if (!node) return;

        // Remove from UUID map
        m_UUIDMap.erase(node->GetUUID());

        // Remove from all groups
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

    void SceneTree::NotifyNodeReady(const Node *node) {
        if (!node) return;
        Logger::Debug() << "Node ready: " << node->GetName();
    }

    void SceneTree::QueueDelete(const Ref<Node> &node) {
        if (!node) return;

        node->QueueFree();
        m_QueuedForDeletion.push_back(node);
    }

    void SceneTree::ProcessQueuedDeletions() {
        for (const auto &node: m_QueuedForDeletion) {
            if (node) {
                node->RemoveFromParent();
            }
        }
        m_QueuedForDeletion.clear();
    }

    void SceneTree::SetFocusedControl(Control *control) {
        if (m_FocusedControl == control) return;

        if (m_FocusedControl) {
            m_FocusedControl->ReleaseFocus();
        }

        m_FocusedControl = control;

        if (m_FocusedControl) {
            m_FocusedControl->m_HasFocus = true;
            if (m_FocusedControl->OnFocusEntered) {
                m_FocusedControl->OnFocusEntered();
            }
        }
    }

    Ref<Node> SceneTree::FindNodeByPath(const String &path) const {
        if (!m_Root) return nullptr;

        // Split path by '/'
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

        // Navigate the tree
        Ref<Node> current_node = m_Root;
        for (const auto &part: parts) {
            if (part.empty()) continue;
            current_node = current_node->GetChild(part);
            if (!current_node) return nullptr;
        }

        return current_node;
    }

    Ref<Node> SceneTree::FindNodeByUUID(const UUID &uuid) const {
        auto it = m_UUIDMap.find(uuid);
        if (it != m_UUIDMap.end()) {
            // Convert raw pointer back to shared_ptr
            // This is unsafe! Better to store weak_ptr in the map
            // For now, return nullptr as we can't safely reconstruct shared_ptr
            Logger::Warn() << "FindNodeByUUID: Cannot safely return shared_ptr from raw pointer";
            return nullptr;
        }
        return nullptr;
    }

    Vector<Ref<Node> > SceneTree::GetNodesInGroup(const String &group) const {
        Vector<Ref<Node> > result;

        auto it = m_Groups.find(group);
        if (it != m_Groups.end()) {
            // Convert raw pointers to shared_ptrs
            // This is unsafe without proper weak_ptr storage
            Logger::Warn() << "GetNodesInGroup: Cannot safely return shared_ptrs from raw pointers";
        }

        return result;
    }

    void SceneTree::AddToGroup(Node *node, const String &group) {
        if (!node) return;

        auto &nodes = m_Groups[group];
        if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
            nodes.push_back(node);
        }
    }

    void SceneTree::RemoveFromGroup(Node *node, const String &group) {
        if (!node) return;

        auto it = m_Groups.find(group);
        if (it != m_Groups.end()) {
            auto &nodes = it->second;
            nodes.erase(
                std::remove(nodes.begin(), nodes.end(), node),
                nodes.end()
            );
        }
    }

    void SceneTree::CallGroup(const String &group, const String &method,
                              const Function<void(Ref<Node>)> &callback) {
        auto it = m_Groups.find(group);
        if (it != m_Groups.end()) {
            // Make a copy to handle modifications during iteration
            Vector<Node *> nodesCopy = it->second;
            for (Node *node: nodesCopy) {
                if (node) {
                    // Cannot safely convert raw pointer to shared_ptr
                    Logger::Warn() << "CallGroup: Cannot safely call with shared_ptr";
                }
            }
        }
    }
} // namespace ash