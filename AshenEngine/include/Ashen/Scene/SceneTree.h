#ifndef ASHEN_SCENETREE_H
#define ASHEN_SCENETREE_H

#include "Node.h"

namespace ash {

    class SceneTree {
        Ref<Node> root;
        Ref<Node> currentScene;

    public:
        SceneTree() {
            root = std::make_shared<Node>("root");
            root->insideTree = true;
            root->tree = this;
        }

        ~SceneTree() = default;

        [[nodiscard]] Node* GetRoot() const { return root.get(); }
        [[nodiscard]] Node* getCurrentScene() const { return currentScene.get(); }

        void change_scene(const Ref<Node> &newScene) {
            if (!newScene) return;

            if (currentScene) {
                root->RemoveChild(currentScene.get());
            }

            root->AddChild(newScene);
            currentScene = newScene;
        }
    };

}

#endif //ASHEN_SCENETREE_H