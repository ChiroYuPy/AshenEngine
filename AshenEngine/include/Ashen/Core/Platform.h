#ifndef ASHEN_PLATFORM_H
#define ASHEN_PLATFORM_H

#include <string>

namespace Platform {
    void enableAnsiColors();

    std::string getPlatformName();
}

#endif //ASHEN_PLATFORM_H