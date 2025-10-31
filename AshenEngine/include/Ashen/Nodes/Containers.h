#ifndef ASHEN_CONTAINERS_H
#define ASHEN_CONTAINERS_H

#include "Control.h"

namespace ash {
    class Container : public Control {
    public:
        explicit Container(String name = "Container");

        virtual void UpdateChildrenLayout();

        void OnResized() override;
    };

    class VBoxContainer : public Container {
    public:
        explicit VBoxContainer(String name = "VBoxContainer");

        float separation = 4.f;

        void UpdateChildrenLayout() override;
    };

    class HBoxContainer : public Container {
    public:
        explicit HBoxContainer(String name = "HBoxContainer");

        float separation = 4.f;

        void UpdateChildrenLayout() override;
    };
}

#endif //ASHEN_CONTAINERS_H
