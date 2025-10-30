#ifndef ASHEN_TEXTURELOADER_H
#define ASHEN_TEXTURELOADER_H

#include <filesystem>
#include <array>

#include "Ashen/Core/Types.h"

namespace ash {
    namespace fs = std::filesystem;

    // Forward declarations
    class Texture2D;
    class TextureCubeMap;
    struct TextureConfig;
    struct ImageData;

    /**
     * @brief Static texture loader - all methods are static
     * Converts image files to GPU textures
     */
    class TextureLoader final {
    public:
        // No instantiation
        TextureLoader() = delete;

        /**
         * @brief Load 2D texture from file
         * @param path Path to image file
         * @param config Texture configuration (filtering, wrapping, mipmaps)
         * @return Loaded texture (moved)
         */
        [[nodiscard]] static Texture2D Load2D(
            const fs::path &path,
            const TextureConfig &config
        );

        /**
         * @brief Load cubemap from 6 face images
         * @param facesPaths Array of paths: [+X, -X, +Y, -Y, +Z, -Z]
         * @param config Texture configuration
         * @return Loaded cubemap (moved)
         */
        [[nodiscard]] static TextureCubeMap LoadCubemap(
            const std::array<fs::path, 6> &facesPaths,
            const TextureConfig &config
        );

        /**
         * @brief Find texture file with supported extensions
         * Searches for: .png, .jpg, .jpeg, .bmp, .tga, .hdr
         */
        [[nodiscard]] static fs::path FindTexture(
            const fs::path &basePath,
            const std::string &textureName
        );

    private:
        /**
         * @brief Create texture from loaded image data
         */
        static Texture2D CreateFromImageData(
            const ImageData &imageData,
            const TextureConfig &config
        );
    };
}

#endif // ASHEN_TEXTURELOADER_H