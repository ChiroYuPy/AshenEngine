#ifndef ASHEN_CONTROL_H
#define ASHEN_CONTROL_H

#include "Node.h"
#include "Ashen/Math/BBox.h"

namespace ash {

    enum class Anchor {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight,
        Full
    };

    class Control : public Node {
    public:
        explicit Control(const String& name = "Control") : Node(name) {}
        virtual ~Control() = default;

        void SetSize(const Vec2& size);
        Vec2 GetSize() const { return m_Size; }
        virtual Vec2 GetMinimumSize() const { return m_MinSize; }

        BBox2 GetRect() const;
        BBox2 GetGlobalRect() const;

        void SetAnchor(Anchor anchor);
        void SetMargins(float left, float top, float right, float bottom);

        void SetMouseFilter(bool enabled) { m_MouseFilter = enabled; }
        bool GetMouseFilter() const { return m_MouseFilter; }

        void GrabFocus();
        void ReleaseFocus();
        bool HasFocus() const { return m_HasFocus; }
        bool IsHovered() const { return m_IsHovered; }  // ← RENDRE PUBLIC

        void OnDraw() override;
        virtual void DrawSelf() {}

        std::function<void()> OnFocusEntered;
        std::function<void()> OnFocusExited;
        std::function<void()> OnMouseEntered;
        std::function<void()> OnMouseExited;
        std::function<void(Event&)> OnGuiInput;

    protected:
        Vec2 m_Size{100.0f, 100.0f};
        Vec2 m_MinSize{0.0f, 0.0f};

        Anchor m_Anchor{Anchor::TopLeft};
        Vec4 m_Margins{0.0f};

        bool m_MouseFilter{true};
        bool m_HasFocus{false};
        bool m_IsHovered{false};

        friend class UISystem;
        friend class SceneTree;  // ← AJOUTER CETTE LIGNE
    };

    class Container : public Control {
    public:
        explicit Container(const String& name = "Container") : Control(name) {}

        void OnProcess(float delta) override;
        virtual void LayoutChildren() {}
    };

} // namespace ash

#endif // ASHEN_CONTROL_H