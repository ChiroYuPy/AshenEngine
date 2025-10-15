#include "Ashen/Utils/ImageLoader.h"

#include <stb_image.h>
#include <stdexcept>
#include <algorithm>

namespace ash {
    ImageData ImageLoader::Load(const fs::path &path, const bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

        ImageData data;
        unsigned char *rawData = stbi_load(
            path.string().c_str(),
            &data.width,
            &data.height,
            &data.channels,
            0
        );

        if (!rawData) {
            throw std::runtime_error("Failed to load image: " + path.string());
        }

        const size_t size = data.width * data.height * data.channels;
        data.pixels.assign(rawData, rawData + size);

        stbi_image_free(rawData);

        return data;
    }

    bool ImageLoader::IsSupportedFormat(const std::string &extension) {
        const auto &supported = GetSupportedExtensions();
        return std::ranges::find(supported, extension) != supported.end();
    }

    Vector<std::string> ImageLoader::GetSupportedExtensions() {
        return {".png", ".jpg", ".jpeg", ".bmp", ".tga"};
    }
}