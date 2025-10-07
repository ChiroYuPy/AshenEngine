#ifndef ASHEN_RENDERER3D_H
#define ASHEN_RENDERER3D_H

namespace ash {
    class Camera;

    class Renderer3D {
    public:
        static void Init();

        static void Shutdown();

        static void BeginScene(const Camera &camera);

        static void EndScene();
    };
}

#endif //ASHEN_RENDERER3D_H