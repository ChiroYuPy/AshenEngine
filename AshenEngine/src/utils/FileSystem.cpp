#include "Ashen/utils/FileSystem.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace ash {
    std::string FileSystem::ReadFileAsString(const fs::path &filepath) {
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
    static std::vector<fs::path> CollectFiles(const Iterator &begin, const Iterator &end,
                                              const std::vector<std::string> &extensions) {
        std::vector<fs::path> files;
        for (auto it = begin; it != end; ++it) {
            if (!it->is_regular_file()) continue;

            if (extensions.empty()) {
                files.push_back(it->path());
            } else {
                const auto ext = it->path().extension().string();
                if (std::any_of(extensions.begin(), extensions.end(), [&ext](const std::string &validExt) {
                    return ext == validExt;
                })) {
                    files.push_back(it->path());
                }
            }
        }
        return files;
    }

    std::vector<fs::path> FileSystem::ScanDirectory(
        const fs::path &dir,
        const std::vector<std::string> &extensions,
        const bool recursive
    ) {
        if (!Exists(dir) || !IsDirectory(dir))
            return {};

        if (recursive) return CollectFiles(fs::recursive_directory_iterator(dir), fs::recursive_directory_iterator(),
                                           extensions);
        return CollectFiles(fs::directory_iterator(dir), fs::directory_iterator(), extensions);
    }

    fs::path FileSystem::GetAbsolutePath(const fs::path &path) {
        return fs::absolute(path);
    }
}