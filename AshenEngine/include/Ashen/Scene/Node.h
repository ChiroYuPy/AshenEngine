#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Component.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Transform.h"

namespace ash {
    class SceneTree;

    class Node : public std::enable_shared_from_this<Node> {
    public:
        using NodeRef = std::shared_ptr<Node>;

        explicit Node(const std::string& name = "Node");
        ~Node();

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        // ===== Hiérarchie =====
        void AddChild(const NodeRef& child);
        void RemoveChild(Node* child);
        [[nodiscard]] Node* GetParent() const { return m_Parent; }
        [[nodiscard]] const std::vector<NodeRef>& GetChildren() const { return m_Children; }
        [[nodiscard]] int GetChildCount() const { return static_cast<int>(m_Children.size()); }
        [[nodiscard]] Node* GetChild(int idx) const;
        [[nodiscard]] NodeRef FindChild(const std::string& name, bool recursive = true) const;

        // ===== Nommage =====
        void SetName(const std::string& name);
        [[nodiscard]] std::string GetName() const { return m_Name; }
        [[nodiscard]] std::string GetPath() const;

        // ===== État =====
        void SetActive(bool active);
        [[nodiscard]] bool IsActive() const { return m_Active && (m_Parent ? m_Parent->IsActive() : true); }
        [[nodiscard]] bool IsLocalActive() const { return m_Active; }

        void SetVisible(bool visible);
        [[nodiscard]] bool IsVisible() const { return m_Visible && (m_Parent ? m_Parent->IsVisible() : true); }

        // ===== Transform =====
        void SetLocalTransform(const Transform& t) { m_LocalTransform = t; MarkTransformDirty(); }
        [[nodiscard]] const Transform& GetLocalTransform() const { return m_LocalTransform; }

        void SetLocalPosition(const Vec3& pos) { m_LocalTransform.position = pos; MarkTransformDirty(); }
        [[nodiscard]] Vec3 GetLocalPosition() const { return m_LocalTransform.position; }

        void SetLocalRotation(const Vec3& rot) { m_LocalTransform.rotation = rot; MarkTransformDirty(); }
        [[nodiscard]] Vec3 GetLocalRotation() const { return m_LocalTransform.rotation; }

        void SetLocalScale(const Vec3& s) { m_LocalTransform.scale = s; MarkTransformDirty(); }
        [[nodiscard]] Vec3 GetLocalScale() const { return m_LocalTransform.scale; }

        [[nodiscard]] Transform GetGlobalTransform() const;
        [[nodiscard]] Vec3 GetGlobalPosition() const;
        [[nodiscard]] Vec3 GetGlobalRotation() const;
        [[nodiscard]] Vec3 GetGlobalScale() const;

        void SetGlobalPosition(const Vec3& pos);
        [[nodiscard]] Mat4 GetWorldMatrix() const;

        // ===== Composants =====
        template<typename T, typename... Args>
        requires std::is_base_of_v<Component, T>
        T* AddComponent(Args&&... args) {
            auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
            T* ptr = comp.get();
            m_Components[&typeid(T)] = std::move(comp);
            ptr->OnCreate();
            return ptr;
        }

        template<typename T>
        T* GetComponent() {
            auto it = m_Components.find(&typeid(T));
            if (it != m_Components.end()) {
                return dynamic_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        template<typename T>
        bool HasComponent() const {
            return m_Components.find(&typeid(T)) != m_Components.end();
        }

        void RemoveComponent(const std::type_info& type);

        const auto& GetAllComponents() const { return m_Components; }

        // ===== Cycle de vie =====
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float ts);
        virtual void OnRender();

        // ===== Utilitaires =====
        [[nodiscard]] SceneTree* GetTree() const { return m_Tree; }
        [[nodiscard]] bool IsInsideTree() const { return m_Tree != nullptr; }
        void SetOwner(Node* owner) { m_Owner = owner; }
        [[nodiscard]] Node* GetOwner() const { return m_Owner; }

        [[nodiscard]] uint32_t GetInstanceID() const { return m_InstanceID; }

        PropertyRegistry& GetProperties() { return m_Properties; }

    protected:
        void MarkTransformDirty() const;
        void UpdateCachedTransforms() const;

    private:
        void AddChildInternal(const NodeRef& child);
        void RemoveChildInternal(Node* child);
        void SetTreeRecursive(SceneTree* tree);

        // Identité
        std::string m_Name;
        uint32_t m_InstanceID;
        static uint32_t s_NextInstanceID;

        // Hiérarchie
        Node* m_Parent = nullptr;
        std::vector<NodeRef> m_Children;
        Node* m_Owner = nullptr;
        SceneTree* m_Tree = nullptr;

        // État
        bool m_Active = true;
        bool m_Visible = true;

        // Transform
        Transform m_LocalTransform;
        mutable Transform m_GlobalTransform;
        mutable Mat4 m_WorldMatrix{};
        mutable bool m_TransformDirty = true;

        // Composants
        std::unordered_map<const std::type_info*, std::unique_ptr<Component>> m_Components;

        // Propriétés
        PropertyRegistry m_Properties;

        friend class SceneTree;
    };
}

#endif // ASHEN_NODE_H