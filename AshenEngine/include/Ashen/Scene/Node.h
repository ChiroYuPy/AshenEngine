#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Ashen/Core/Types.h"
#include <memory>

namespace ash {

    class SceneTree;
    class Node;

    template<typename T>
    using Ref = std::shared_ptr<T>;

    class Node {
    protected:
        String name;
        Node* parent = nullptr;
        Node* owner = nullptr;
        Vector<Ref<Node>> children;
        SceneTree* tree = nullptr;
        bool insideTree = false;
        bool visible = true;

    public:
        explicit Node(String nodeName = "");
        virtual ~Node() = default;

        [[nodiscard]] virtual String GetClassName() const { return "Node"; }

        void AddChild(const Ref<Node> &child);
        void RemoveChild(Node* child);

        [[nodiscard]] Node* GetParent() const;
        [[nodiscard]] Node* GetChild(int idx) const;
        [[nodiscard]] int GetChildCount() const;
        [[nodiscard]] Vector<Ref<Node>> GetChildren() const;
        [[nodiscard]] Node* GetNode(const String& path) const;
        [[nodiscard]] Node* FindChild(const String& pattern, bool recursive = true) const;

        void SetName(const String& newName);
        [[nodiscard]] String GetName() const;

        void SetOwner(Node* newOwner);
        [[nodiscard]] Node* GetOwner() const;

        [[nodiscard]] bool IsInsideTree() const;
        [[nodiscard]] SceneTree* GetTree() const;

        void SetVisible(bool v);
        [[nodiscard]] bool IsVisible() const;

        [[nodiscard]] String GetPath() const;

    protected:
        [[nodiscard]] bool HasNode(const String& nodeName) const;
        void AddChildToTree(Node* child) const;
        static void RemoveChildFromTree(Node* child);

        friend class SceneTree;
    };

}

#endif // ASHEN_NODE_H