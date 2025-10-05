#ifndef ASHEN_IMAGELOADER_H
#define ASHEN_IMAGELOADER_H

#include <string>
#include <vector>
#include <filesystem>

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Image data structure
     */
    struct ImageData {
        std::vector<unsigned char> pixels;
        int width = 0;
        int height = 0;
        int channels = 0;

        [[nodiscard]] bool IsValid() const { return !pixels.empty(); }
    };

    /**
     * @brief Image loading utility
     */
    class ImageLoader {
    public:
        /**
         * @brief Load image from file path
         * @param path Path to image file
         * @param flipVertically Whether to flip image vertically (useful for OpenGL)
         * @return ImageData structure with pixel data and dimensions
         * @throws std::runtime_error if loading fails
         */
        static ImageData Load(const fs::path &path, bool flipVertically = true);

        /**
         * @brief Check if file extension is a supported image format
         */
        static bool IsSupportedFormat(const std::string &extension);

        /**
         * @brief Get list of supported image extensions
         */
        static std::vector<std::string> GetSupportedExtensions();
    };
}

#endif //ASHEN_IMAGELOADER_H
