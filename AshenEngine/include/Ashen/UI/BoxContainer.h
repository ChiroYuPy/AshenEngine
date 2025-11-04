#ifndef ASHEN_UI_BOX_CONTAINER_H
#define ASHEN_UI_BOX_CONTAINER_H

#include "Ashen/Nodes/Control.h"
#include "Ashen/Math/Math.h"

namespace ash {
    enum class BoxOrientation {
        Horizontal,
        Vertical
    };

    class BoxContainer : public Control {
    public:
        explicit BoxContainer(BoxOrientation orientation);

        BoxOrientation GetOrientation() const { return m_Orientation; }

        float GetSeparation() const { return m_Separation; }
        void SetSeparation(float separation) { m_Separation = Max(separation, 0.0f); }

        void _Process(float delta) override;

    protected:
        void UpdateLayout() override;

    private:
        BoxOrientation m_Orientation;
        float m_Separation = 4.0f;
    };

    class HBoxContainer : public BoxContainer {
    public:
        HBoxContainer() : BoxContainer(BoxOrientation::Horizontal) {}
    };

    class VBoxContainer : public BoxContainer {
    public:
        VBoxContainer() : BoxContainer(BoxOrientation::Vertical) {}
    };
}

#endif // ASHEN_UI_BOX_CONTAINER_H