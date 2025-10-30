#include "Ashen/Utils/FileSystem.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace ash {
    String FileSystem::ReadFileAsString(const fs::path &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open())
            throw std::runtime_error("Failed to open file: " + filepath.string());

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    bool FileSystem::Exists(const fs::path &path) {
        return fs::exists(path);
    }

    bool FileSystem::IsDirectory(const fs::path &path) {
        return fs::is_directory(path);
    }

    // Fonction helper pour filtrer les fichiers selon les extensions
    template<typename Iterator>
    static Vector<fs::path> CollectFiles(const Iterator &begin, const Iterator &end,
                                         const Vector<String> &extensions) {
        Vector<fs::path> files;
        for (auto it = begin; it != end; ++it) {
            if (!it->is_regular_file()) continue;

            if (extensions.empty()) {
                files.push_back(it->path());
            } else {
                const auto ext = it->path().extension().string();
                if (std::any_of(extensions.begin(), extensions.end(), [&ext](const String &validExt) {
                    return ext == validExt;
                })) {
                    files.push_back(it->path());
                }
            }
        }
        return files;
    }

    Vector<fs::path> FileSystem::ScanDirectory(
        const fs::path &dir,
        const Vector<String> &extensions,
        const bool recursive
    ) {
        if (!Exists(dir) || !IsDirectory(dir))
            return {};

        if (recursive)
            return CollectFiles(fs::recursive_directory_iterator(dir), fs::recursive_directory_iterator(),
                                extensions);
        return CollectFiles(fs::directory_iterator(dir), fs::directory_iterator(), extensions);
    }

    fs::path FileSystem::GetAbsolutePath(const fs::path &path) {
        return fs::absolute(path);
    }
}