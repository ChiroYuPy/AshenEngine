#ifndef ASHEN_CONTAINER_H
#define ASHEN_CONTAINER_H

#include "Control.h"

namespace ash {
    class Container : public Control {
    public:
        explicit Container(const String& name = "Container") : Control(name) {}
        [[nodiscard]] String GetClassName() const override { return "Container"; }
    };

    class VBoxContainer : public Container {
    protected:
        float spacing = 4.0f;
    
    public:
        explicit VBoxContainer(const String& name = "VBoxContainer") : Container(name) {}
        [[nodiscard]] String GetClassName() const override { return "VBoxContainer"; }
    
        void SetSpacing(const float s) { spacing = s; }
        [[nodiscard]] float GetSpacing() const { return spacing; }
    
        void ArrangeChildren() const {
            float yOffset = 0;
            for (const auto& child : children) {
                const auto ctrl = dynamic_cast<Control*>(child.get());
                if (ctrl && ctrl->IsVisible()) {
                    ctrl->SetPosition(Vec2(0, yOffset));
                    yOffset += ctrl->GetSize().y + spacing;
                }
            }
        }
    };

    class HBoxContainer : public Container {
    protected:
        float spacing = 4.0f;
    
    public:
        explicit HBoxContainer(const String& name = "HBoxContainer") : Container(name) {}
        [[nodiscard]] String GetClassName() const override { return "HBoxContainer"; }
    
        void SetSpacing(const float s) { spacing = s; }
        [[nodiscard]] float GetSPacing() const { return spacing; }
    
        void ArrangeChildren() const {
            float xOffset = 0;
            for (const auto& child : children) {
                const auto ctrl = dynamic_cast<Control*>(child.get());
                if (ctrl && ctrl->IsVisible()) {
                    ctrl->SetPosition(Vec2(xOffset, 0));
                    xOffset += ctrl->GetSize().x + spacing;
                }
            }
        }
    };

    // CenterContainer - centre son contenu
    class CenterContainer : public Container {
    public:
        explicit CenterContainer(const String& name = "CenterContainer") : Container(name) {}
        [[nodiscard]] String GetClassName() const override { return "CenterContainer"; }
    
        void CenterChildren() const {
            for (const auto& child : children) {
                const auto ctrl = dynamic_cast<Control*>(child.get());
                if (ctrl && ctrl->IsVisible()) {
                    Vec2 childSize = ctrl->GetSize();
                    Vec2 offset = (size - childSize) * 0.5f;
                    ctrl->SetPosition(offset);
                }
            }
        }
    };

    // Label - affiche du texte
    class Label : public Control {
    protected:
        String text;
    
    public:
        explicit Label(const String& name = "Label") : Control(name) {}
        [[nodiscard]] String GetClassName() const override { return "Label"; }
    
        void SetText(const String& t) { text = t; }
        [[nodiscard]] String GetText() const { return text; }
    
        void PrintInfo() const override {
            Logger::Info() << name << " [" << text << "] - Pos: ("  << position.x << ", " << position.y << ")";
        }
    };

    // Button - bouton cliquable (sans gestion d'événements pour l'instant)
    class Button : public Control {
    protected:
        String text;
        bool disabled = false;
    
    public:
        explicit Button(const String& name = "Button") : Control(name) {}
        [[nodiscard]] String GetClassName() const override { return "Button"; }
    
        void SetText(const String& t) { text = t; }
        [[nodiscard]] String GetText() const { return text; }
    
        void SetDisabled(const bool d) { disabled = d; }
        [[nodiscard]] bool IsDisabled() const { return disabled; }
    
        void PrintInfo() const override {
            Logger::Info() << name << " [" << text << "]";
            if (disabled) Logger::Info() << " (disabled)";
            Logger::Info() << " - Pos: (" << position.x << ", " << position.y << ")";
        }
    };

    // Panel - fond pour grouper des éléments
    class Panel : public Control {
    public:
        explicit Panel(const String& name = "Panel") : Control(name) {}
        [[nodiscard]] String GetClassName() const override { return "Panel"; }
    };
}

#endif //ASHEN_CONTAINER_H