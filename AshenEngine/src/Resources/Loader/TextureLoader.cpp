#include "Ashen/Resources/Loader/TextureLoader.h"

#include "Ashen/Core/Logger.h"

namespace ash {
    Texture2D TextureLoader::Load2D(const fs::path& path, const TextureConfig& config) {
        if (!FileSystem::Exists(path))
            throw std::runtime_error("Texture file not found: " + path.string());

        const ImageData imageData = ImageLoader::Load(path, true);
        return FromImageData(imageData, config);
    }

    Texture2D TextureLoader::FromImageData(const ImageData& imageData, const TextureConfig& config) {
        if (!imageData.IsValid())
            throw std::runtime_error("Invalid image data");

        Texture2D texture;

        TextureFormat internalFormat, format;
        GetTextureFormats(imageData.channels, internalFormat, format);

        texture.SetData(
            internalFormat,
            imageData.width,
            imageData.height,
            format,
            PixelDataType::UnsignedByte,
            imageData.pixels.data()
        );

        texture.ApplyConfig(config);

        return texture;
    }

    TextureCubeMap TextureLoader::LoadCubemap(
        const std::array<fs::path, 6>& facesPaths,
        const TextureConfig& config
    ) {
        TextureCubeMap cubemap;

        constexpr CubeMapFace faces[6] = {
            CubeMapFace::PositiveX, CubeMapFace::NegativeX,
            CubeMapFace::PositiveY, CubeMapFace::NegativeY,
            CubeMapFace::PositiveZ, CubeMapFace::NegativeZ
        };

        for (int i = 0; i < 6; ++i) {
            if (!FileSystem::Exists(facesPaths[i]))
                throw std::runtime_error("Cubemap face not found: " + facesPaths[i].string());

            const ImageData imageData = ImageLoader::Load(facesPaths[i], false);

            TextureFormat internalFormat, format;
            GetTextureFormats(imageData.channels, internalFormat, format);

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

    fs::path TextureLoader::FindTexture(const fs::path& basePath, const std::string& textureName) {
        // Check if the exact path exists
        fs::path texPath = basePath / textureName;
        if (FileSystem::Exists(texPath))
            return texPath;

        // Try with supported extensions
        for (const auto& ext : ImageLoader::GetSupportedExtensions()) {
            texPath = basePath / (textureName + ext);
            if (FileSystem::Exists(texPath))
                return texPath;
        }

        return {};
    }

    Vector<std::string> TextureLoader::ScanForTextures(const fs::path& directory) {
        Vector<std::string> textures;
        const auto files = FileSystem::ScanDirectory(
            directory,
            ImageLoader::GetSupportedExtensions(),
            true
        );

        for (const auto& file : files) {
            textures.push_back(file.filename().string());
        }

        return textures;
    }

    void TextureLoader::GetTextureFormats(
        const int channels,
        TextureFormat& internalFormat,
        TextureFormat& format
    ) {
        switch (channels) {
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
                throw std::runtime_error("Unsupported number of channels: " + std::to_string(channels));
        }
    }
}