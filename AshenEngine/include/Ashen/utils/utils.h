#ifndef ASHEN_UTILS_H
#define ASHEN_UTILS_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <stb_image.h>

namespace pixl {
    static std::string readFileAsString(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file : " << filepath << std::endl;
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    inline std::vector<unsigned char> LoadImageFile(const std::string &path, int &width, int &height, int &channels) {
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) throw std::runtime_error("Failed to load image: " + path);
        std::vector pixels(data, data + width * height * channels);
        stbi_image_free(data);
        return pixels;
    }
}

#endif //ASHEN_UTILS_H