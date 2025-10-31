#ifndef ASHEN_CANVASITEM_H
#define ASHEN_CANVASITEM_H

#include "Node.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class CanvasItem : public Node {
    public:
        explicit CanvasItem(String name = "CanvasItem");

        bool visible = true;
        int z_index = 0;
        bool z_as_relative = true;
        float modulate_alpha = 1.f;
        Vec4 modulate_color = Vec4(1.f, 1.f, 1.f, 1.f);

        void Draw() override;

        void Hide();

        void Show();

        bool IsVisible() const;

        bool IsVisibleInTree() const;

        int GetZIndex() const;

        void SetZIndex(int z);

        int GetGlobalZIndex() const;
    };
}

#endif // ASHEN_CANVASITEM_H
