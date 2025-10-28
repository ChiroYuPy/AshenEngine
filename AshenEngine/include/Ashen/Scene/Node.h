#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Ashen/Core/Types.h"
#include "Ashen/Core/UUID.h"  // ← AJOUTER CETTE LIGNE
#include "Ashen/Math/Math.h"
#include "Ashen/Events/Event.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace ash {

class Node : public std::enable_shared_from_this<Node> {
public:
    using NodePtr = std::shared_ptr<Node>;
    using NodeWeak = std::weak_ptr<Node>;

    explicit Node(const String& name = "Node") : m_UUID(), m_Name(name) {}  // ← AJOUTER m_UUID()
    virtual ~Node() = default;

    virtual void OnReady() {}
    virtual void OnProcess(float delta) {}
    virtual void OnDraw() {}
    virtual void OnInput(Event& event) {}

    void AddChild(NodePtr child);
    void RemoveChild(NodePtr child);
    void RemoveFromParent();

    NodePtr GetParent() const { return m_Parent.lock(); }
    const std::vector<NodePtr>& GetChildren() const { return m_Children; }
    NodePtr FindChild(const String& name, bool recursive = false) const;

    void SetName(const String& name) { m_Name = name; }
    String GetName() const { return m_Name; }

    UUID GetUUID() const { return m_UUID; }  // ← AJOUTER CETTE LIGNE

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }
    bool IsVisibleInTree() const;

    void SetProcessEnabled(bool enabled) { m_ProcessEnabled = enabled; }
    bool IsProcessEnabled() const { return m_ProcessEnabled; }

    void SetPosition(const Vec2& pos) { m_Position = pos; m_TransformDirty = true; }
    Vec2 GetPosition() const { return m_Position; }
    Vec2 GetGlobalPosition() const;

    void SetRotation(float rot) { m_Rotation = rot; m_TransformDirty = true; }
    float GetRotation() const { return m_Rotation; }

    void SetScale(const Vec2& scale) { m_Scale = scale; m_TransformDirty = true; }
    Vec2 GetScale() const { return m_Scale; }

    Mat4 GetTransform() const;
    Mat4 GetGlobalTransform() const;

protected:
    UUID m_UUID;  // ← AJOUTER CETTE LIGNE
    String m_Name;
    NodeWeak m_Parent;
    std::vector<NodePtr> m_Children;

    Vec2 m_Position{0.0f};
    float m_Rotation{0.0f};
    Vec2 m_Scale{1.0f};

    mutable Mat4 m_Transform{1.0f};
    mutable bool m_TransformDirty{true};

    bool m_Visible{true};
    bool m_ProcessEnabled{true};
    bool m_IsReady{false};

    friend class SceneTree;
    friend class UISystem;  // ← AJOUTER CETTE LIGNE
};

} // namespace ash

#endif // ASHEN_NODE_H