#ifndef ASHEN_SCENE_TREE_H
#define ASHEN_SCENE_TREE_H

#include "Control.h"
#include "Node.h"
#include "Ashen/Events/Event.h"
// SUPPRIMER: #include "Ashen/Graphics/UI/UISystem.h"

namespace ash {
    class SceneTree {
    public:
        SceneTree();
        ~SceneTree();

        void ChangeScene(const Ref<Node> &newRoot);
        void ChangeSceneDeferred(const Ref<Node> &newRoot);

        Ref<Node> GetRoot() const { return m_Root; }
        Ref<Node> GetCurrentScene() const { return m_CurrentScene; }

        void Process(float delta);
        void PhysicsProcess(float delta);
        void Input(Event &event);

        void NotifyNodeEnterTree(Node *node);
        void NotifyNodeExitTree(Node *node);
        void NotifyNodeReady(const Node *node);

        void QueueDelete(const Ref<Node> &node);
        void ProcessQueuedDeletions();

        void SetPaused(const bool paused) { m_Paused = paused; }
        bool IsPaused() const { return m_Paused; }

        void SetFocusedControl(Control *control);
        Control *GetFocusedControl() const { return m_FocusedControl; }

        Ref<Node> FindNodeByPath(const String &path) const;
        Ref<Node> FindNodeByUUID(const UUID &uuid) const;

        Vector<Ref<Node> > GetNodesInGroup(const String &group) const;

        void AddToGroup(Node *node, const String &group);
        void RemoveFromGroup(Node *node, const String &group);
        void CallGroup(const String &group, const String &method,
                       const Function<void(Ref<Node>)> &callback);

        Function<void()> OnTreeChanged;
        Function<void(Ref<Node>)> OnNodeAdded;
        Function<void(Ref<Node>)> OnNodeRemoved;

    private:
        void ProcessNode(const Ref<Node> &node, float delta);
        void PhysicsProcessNode(const Ref<Node> &node, float delta);
        void InputNode(const Ref<Node> &node, Event &event);

        Ref<Node> m_Root;
        Ref<Node> m_CurrentScene;
        Vector<Ref<Node> > m_QueuedForDeletion;
        HashMap<String, Vector<Node *> > m_Groups;
        HashMap<UUID, Node *> m_UUIDMap;

        Control *m_FocusedControl = nullptr;
        bool m_Paused = false;

        Ref<Node> m_DeferredSceneChange;
    };
} // namespace ash

#endif // ASHEN_SCENE_TREE_H