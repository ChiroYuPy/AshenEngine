#ifndef ASHEN_PLATFORM_H
#define ASHEN_PLATFORM_H

#include <string>
#include <cstdint>

namespace ash::Platform {
    // ----- Platform detection -----
    bool IsWindows();

    bool IsLinux();

    bool IsMac();

    bool IsUnix();

    bool Is64Bit();

    std::string GetPlatformName();

    // ----- ANSI colors -----
    void EnableAnsiColors();

    // ----- CPU info -----
    unsigned int GetCPUCoreCount();

    std::string GetCPUName();

    // ----- RAM info -----
    size_t GetTotalRAM(); // octets
    size_t GetAvailableRAM(); // octets

    // ----- GPU info (minimal, via OpenGL) -----
    std::string GetGPUName();

    std::string GetGPUVendor();

    // ----- Endianness -----
    enum class Endianness { Little, Big };

    Endianness GetEndianness();
}

#endif // ASHEN_PLATFORM_H