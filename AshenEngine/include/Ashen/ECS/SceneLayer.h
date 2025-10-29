#ifndef ASHEN_SCENE_LAYER_H
#define ASHEN_SCENE_LAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/ECS/Scene.h"
#include "Ashen/Core/Types.h"

namespace ash {
    // Layer qui contient une scène
    class SceneLayer : public Layer {
    public:
        explicit SceneLayer(const String& name = "SceneLayer")
            : Layer(name), m_Scene(MakeOwn<Scene>()) {}

        ~SceneLayer() override = default;

        void OnAttach() override {
            // Initialisation de la scène
        }

        void OnDetach() override {
            m_Scene->Clear();
        }

        void OnUpdate(float ts) override {
            m_Scene->OnUpdate(ts);
        }

        void OnRender() override {
            m_Scene->OnRender();
        }

        void OnEvent(Event& event) override {
            m_Scene->OnEvent(event);
        }

        Scene* GetScene() const { return m_Scene.get(); }

    protected:
        Own<Scene> m_Scene;
    };

} // namespace ash

#endif // ASHEN_SCENE_LAYER_H