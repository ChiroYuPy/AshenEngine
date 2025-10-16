#ifndef ASHEN_SCENETREE_H
#define ASHEN_SCENETREE_H

#include <unordered_set>
#include "Node.h"

namespace ash {
    class SceneTree {
    public:
        using NodeRef = std::shared_ptr<Node>;

        SceneTree();
        ~SceneTree();

        [[nodiscard]] Node* GetRoot() const { return m_Root.get(); }
        [[nodiscard]] Node* GetCurrentScene() const { return m_CurrentScene.get(); }

        void LoadScene(const NodeRef& scene);
        void UnloadScene();

        [[nodiscard]] Node* FindNodeByID(uint32_t id) const;
        [[nodiscard]] Node* FindNodeByName(const std::string& name) const;

        void Update(float ts) const;
        void Render() const;

        void Destroy() { m_CurrentScene = nullptr; m_Root = nullptr; }

    private:
        NodeRef m_Root;
        NodeRef m_CurrentScene;
        std::unordered_set<uint32_t> m_NodeRegistry;

        friend class Node;
    };
}

#endif //ASHEN_SCENETREE_H