#ifndef ASHEN_TEXTURELOADER_H
#define ASHEN_TEXTURELOADER_H

#include <filesystem>
#include <array>
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Utils/ImageLoader.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {
    namespace fs = std::filesystem;

    /**
     * @brief Loads textures from image files and creates Texture objects
     */
    class TextureLoader {
    public:
        /**
         * @brief Load a 2D texture from file
         * @param path Path to image file
         * @param config Texture configuration
         * @return Texture2D object
         */
        static Texture2D Load2D(
            const fs::path& path,
            const TextureConfig& config = TextureConfig::Default()
        );

        /**
         * @brief Load a 2D texture from image data
         */
        static Texture2D FromImageData(
            const ImageData& imageData,
            const TextureConfig& config = TextureConfig::Default()
        );

        /**
         * @brief Load a cubemap from 6 face images
         * @param facesPaths Array of paths for +X, -X, +Y, -Y, +Z, -Z faces
         * @param config Texture configuration
         */
        static TextureCubeMap LoadCubemap(
            const std::array<fs::path, 6>& facesPaths,
            const TextureConfig& config = TextureConfig::Clamped()
        );

        /**
         * @brief Find texture file with supported extensions
         * @param basePath Base path to search
         * @param textureName Texture name without extension
         * @return Full path if found, empty path otherwise
         */
        static fs::path FindTexture(const fs::path& basePath, const std::string& textureName);

        /**
         * @brief Scan directory for available textures
         */
        static Vector<std::string> ScanForTextures(const fs::path& directory);

    private:
        /**
         * @brief Determine texture format from number of channels
         */
        static void GetTextureFormats(
            int channels,
            TextureFormat& internalFormat,
            TextureFormat& format
        );
    };
}

#endif // ASHEN_TEXTURELOADER_H