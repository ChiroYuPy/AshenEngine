#ifndef ASHEN_MESHLOADER_H
#define ASHEN_MESHLOADER_H

#include <filesystem>

namespace ash {
    namespace fs = std::filesystem;

    class Mesh;

    class MeshLoader final {
    public:
        MeshLoader() = delete;

        [[nodiscard]] static Mesh Load(
            const fs::path &path,
            bool flipUVs = true
        );

        [[nodiscard]] static bool IsSupported(const std::string &extension);
    };
}

#endif // ASHEN_MESHLOADER_H