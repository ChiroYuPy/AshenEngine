#ifndef ASHEN_FILESYSTEM_H
#define ASHEN_FILESYSTEM_H

#include <filesystem>
#include <string>

#include "Ashen/Core/Types.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Utility class for file system operations
     */
    class FileSystem {
    public:
        /**
         * @brief Read a file as a string
         */
        static std::string ReadFileAsString(const fs::path &filepath);

        /**
         * @brief Check if a file exists
         */
        static bool Exists(const fs::path &path);

        /**
         * @brief Check if path is a directory
         */
        static bool IsDirectory(const fs::path &path);

        /**
         * @brief Scan directory for files with specific extensions
         * @param dir Directory to scan
         * @param extensions List of extensions to filter (e.g., {".png", ".jpg"})
         * @param recursive Whether to scan recursively
         * @return List of file paths matching the criteria
         */
        static Vector<fs::path> ScanDirectory(
            const fs::path &dir,
            const Vector<std::string> &extensions = {},
            bool recursive = true
        );

        /**
         * @brief Get absolute path
         */
        static fs::path GetAbsolutePath(const fs::path &path);
    };
}

#endif //ASHEN_FILESYSTEM_H