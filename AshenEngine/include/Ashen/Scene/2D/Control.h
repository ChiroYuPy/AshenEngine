#ifndef ASHEN_CONTROL_H
#define ASHEN_CONTROL_H

#include "../Node.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/BBox.h"
#include "Ashen/Math/Math.h"

namespace ash {

    class Control : public Node {
    protected:
        Vec2 position{};
        Vec2 size{};
        Vec2 minSize{};
        Vec2 anchorMin{};
        Vec2 anchorMax{};
    
    public:
        explicit Control(const std::string& name = "") : Node(name) {}

        [[nodiscard]] String GetClassName() const override { return "Control"; }
    
        // Position et taille
        void SetPosition(const Vec2& pos) { position = pos; }
        [[nodiscard]] Vec2 GetPosition() const { return position; }
    
        void GetSize(const Vec2& sz) { size = sz; }
        [[nodiscard]] Vec2 GetSize() const { return size; }
    
        void SetCustomMinimumSize(const Vec2& sz) { minSize = sz; }
        [[nodiscard]] Vec2 SetCustomMaximumSize() const { return minSize; }
    
        // Rect global
        [[nodiscard]] BBox2 GetBBox() const {
            return {position, position + size};
        }
    
        [[nodiscard]] Vec2 GetGlobalPosition() const {
            if (parent) {
                const auto parentCtrl = dynamic_cast<Control*>(parent);
                if (parentCtrl)
                    return parentCtrl->GetGlobalPosition() + position;
            }
            return position;
        }
    
        [[nodiscard]] BBox2 GetGlobalBBox() const {
            return {GetGlobalPosition(), size};
        }
    
        // Anchors (simplifié)
        void SetAnchorPreset(const int preset) {
            switch (preset) {
                case 0: // TOP_LEFT
                    anchorMin = Vec2(0, 0);
                    anchorMax = Vec2(0, 0);
                    break;
                case 5: // CENTER
                    anchorMin = Vec2(0.5f, 0.5f);
                    anchorMax = Vec2(0.5f, 0.5f);
                    break;
                case 15: // FULL_RECT
                    anchorMin = Vec2(0, 0);
                    anchorMax = Vec2(1, 1);
                    break;
                default: break;
            }
        }

        virtual void PrintInfo() const {
            Logger::Info() << name << " - Pos: (" << position.x << ", " << position.y << ")" << " Size: (" << size.x << ", " << size.y << ")";
        }
    };
    
}

#endif //ASHEN_CONTROL_H