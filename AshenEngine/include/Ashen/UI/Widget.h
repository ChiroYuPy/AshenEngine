#ifndef ASHEN_WIDGET_H
#define ASHEN_WIDGET_H

#include <ranges>
#include <vector>
#include <glm/glm.hpp>

#include "Ashen/core/Types.h"
#include "Ashen/events/Event.h"

namespace ash {
    struct Bounds {
        glm::vec2 Position{0.0f};
        glm::vec2 Size{0.0f};

        [[nodiscard]] bool Contains(const glm::vec2 &point) const {
            return point.x >= Position.x && point.x <= Position.x + Size.x &&
                   point.y >= Position.y && point.y <= Position.y + Size.y;
        }

        [[nodiscard]] glm::vec2 Center() const {
            return Position + Size * 0.5f;
        }
    };

    class Widget {
    public:
        Widget() = default;

        virtual ~Widget() = default;

        Widget(const Widget &) = delete;

        Widget &operator=(const Widget &) = delete;

        // Lifecycle
        virtual void OnAttach() {
        }

        virtual void OnDetach() {
        }

        // Update & Render
        virtual void OnUpdate(float ts) {
        }

        virtual void OnRender() {
        }

        // Event handling - returns true if event was handled
        virtual bool OnEvent(Event &event) { return false; }

        // Hierarchy management
        template<typename T, typename... Args>
            requires std::is_base_of_v<Widget, T>
        T *AddChild(Args &&... args) {
            auto child = MakeScope<T>(std::forward<Args>(args)...);
            T *ptr = child.get();
            child->m_Parent = this;
            child->OnAttach();
            m_Children.push_back(std::move(child));
            return ptr;
        }

        void RemoveChild(Widget *child) {
            const auto it = std::ranges::find_if(m_Children,
                                                 [child](const auto &c) { return c.get() == child; });
            if (it != m_Children.end()) {
                (*it)->OnDetach();
                m_Children.erase(it);
            }
        }

        void ClearChildren() {
            for (const auto &child: m_Children)
                child->OnDetach();

            m_Children.clear();
        }

        // Transform
        void SetPosition(const glm::vec2 &pos) { m_Bounds.Position = pos; }
        void SetSize(const glm::vec2 &size) { m_Bounds.Size = size; }
        void SetBounds(const Bounds &bounds) { m_Bounds = bounds; }

        [[nodiscard]] const Bounds &GetBounds() const { return m_Bounds; }
        [[nodiscard]] glm::vec2 GetPosition() const { return m_Bounds.Position; }
        [[nodiscard]] glm::vec2 GetSize() const { return m_Bounds.Size; }

        [[nodiscard]] glm::vec2 GetAbsolutePosition() const {
            return m_Parent ? m_Parent->GetAbsolutePosition() + m_Bounds.Position : m_Bounds.Position;
        }

        // State
        void SetVisible(const bool visible) { m_Visible = visible; }
        void SetEnabled(const bool enabled) { m_Enabled = enabled; }
        [[nodiscard]] bool IsVisible() const { return m_Visible; }
        [[nodiscard]] bool IsEnabled() const { return m_Enabled; }
        [[nodiscard]] bool IsHovered() const { return m_Hovered; }

        // Hierarchy queries
        [[nodiscard]] Widget *GetParent() const { return m_Parent; }
        [[nodiscard]] const std::vector<Scope<Widget> > &GetChildren() const { return m_Children; }
        [[nodiscard]] size_t GetChildCount() const { return m_Children.size(); }

    protected:
        // Internal event processing (leaf to root)
        bool ProcessEvent(Event &event) {
            if (!m_Visible || !m_Enabled) return false;

            // Children process first (reverse order for top-to-bottom)
            for (const auto &it: std::ranges::reverse_view(m_Children)) {
                if (it->ProcessEvent(event)) {
                    return true; // Event consumed by child
                }
            }

            // Then this widget
            return OnEvent(event);
        }

        // Internal update/render (root to leaf)
        void ProcessUpdate(const float ts) {
            if (!m_Visible) return;
            OnUpdate(ts);
            for (const auto &child: m_Children)
                child->ProcessUpdate(ts);
        }

        void ProcessRender() {
            if (!m_Visible) return;
            OnRender();
            for (const auto &child: m_Children)
                child->ProcessRender();
        }

        void UpdateHoverState(const glm::vec2 &mousePos) {
            if (!m_Visible || !m_Enabled) {
                m_Hovered = false;
                return;
            }

            Bounds absoluteBounds = m_Bounds;
            absoluteBounds.Position = GetAbsolutePosition();
            m_Hovered = absoluteBounds.Contains(mousePos);

            for (const auto &child: m_Children)
                child->UpdateHoverState(mousePos);
        }

        Bounds m_Bounds;
        Widget *m_Parent = nullptr;
        std::vector<Scope<Widget> > m_Children;
        bool m_Visible = true;
        bool m_Enabled = true;
        bool m_Hovered = false;

        friend class Layer;
    };
}

#endif //ASHEN_WIDGET_H