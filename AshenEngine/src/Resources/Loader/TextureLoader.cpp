#include "Ashen/Resources/Loader/TextureLoader.h"

#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Utils/ImageLoader.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {
    Texture2D TextureLoader::Load2D(
        const fs::path &path,
        const TextureConfig &config
    ) {
        if (!FileSystem::Exists(path)) {
            throw std::runtime_error("Texture file not found: " + path.string());
        }

        // Load image data
        const ImageData imageData = ImageLoader::Load(path, true);

        return CreateFromImageData(imageData, config);
    }

    TextureCubeMap TextureLoader::LoadCubemap(
        const std::array<fs::path, 6> &facesPaths,
        const TextureConfig &config
    ) {
        TextureCubeMap cubemap;

        constexpr CubeMapFace faces[6] = {
            CubeMapFace::PositiveX, CubeMapFace::NegativeX,
            CubeMapFace::PositiveY, CubeMapFace::NegativeY,
            CubeMapFace::PositiveZ, CubeMapFace::NegativeZ
        };

        for (int i = 0; i < 6; ++i) {
            if (!FileSystem::Exists(facesPaths[i])) {
                throw std::runtime_error("Cubemap face not found: " + facesPaths[i].string());
            }

            const ImageData imageData = ImageLoader::Load(facesPaths[i], false);

            // Determine texture formats
            TextureFormat internalFormat, format;
            switch (imageData.channels) {
                case 1:
                    internalFormat = TextureFormat::R8;
                    format = TextureFormat::Red;
                    break;
                case 3:
                    internalFormat = TextureFormat::RGB8;
                    format = TextureFormat::RGB;
                    break;
                case 4:
                    internalFormat = TextureFormat::RGBA8;
                    format = TextureFormat::RGBA;
                    break;
                default:
                    throw std::runtime_error("Unsupported channel count: " + std::to_string(imageData.channels));
            }

            cubemap.SetFace(
                faces[i],
                internalFormat,
                imageData.width,
                imageData.height,
                format,
                PixelDataType::UnsignedByte,
                imageData.pixels.data()
            );
        }

        cubemap.ApplyConfig(config);
        return cubemap;
    }

    fs::path TextureLoader::FindTexture(
        const fs::path &basePath,
        const String &textureName
    ) {
        // Check exact path first
        fs::path texPath = basePath / textureName;
        if (FileSystem::Exists(texPath)) {
            return texPath;
        }

        // Try with supported extensions
        const auto extensions = ImageLoader::GetSupportedExtensions();
        for (const auto &ext: extensions) {
            texPath = basePath / (textureName + ext);
            if (FileSystem::Exists(texPath)) {
                return texPath;
            }
        }

        return {}; // Not found
    }

    Texture2D TextureLoader::CreateFromImageData(
        const ImageData &imageData,
        const TextureConfig &config
    ) {
        if (!imageData.IsValid()) {
            throw std::runtime_error("Invalid image data");
        }

        Texture2D texture;

        // Determine texture formats based on channel count
        TextureFormat internalFormat, format;
        switch (imageData.channels) {
            case 1:
                internalFormat = TextureFormat::R8;
                format = TextureFormat::Red;
                break;
            case 3:
                internalFormat = TextureFormat::RGB8;
                format = TextureFormat::RGB;
                break;
            case 4:
                internalFormat = TextureFormat::RGBA8;
                format = TextureFormat::RGBA;
                break;
            default:
                throw std::runtime_error("Unsupported channel count: " + std::to_string(imageData.channels));
        }

        // Upload texture data to GPU
        texture.SetData(
            internalFormat,
            imageData.width,
            imageData.height,
            format,
            PixelDataType::UnsignedByte,
            imageData.pixels.data()
        );

        // Apply configuration (filtering, wrapping, mipmaps)
        texture.ApplyConfig(config);

        return texture;
    }
}