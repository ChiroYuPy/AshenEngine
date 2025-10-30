#ifndef ASHEN_PLATFORM_H
#define ASHEN_PLATFORM_H

#include "Ashen/Core/Types.h"

namespace ash::Platform {
    bool IsWindows();

    bool IsLinux();

    bool IsMac();

    bool IsUnix();

    bool Is64Bit();

    String GetPlatformName();

    void EnableAnsiColors();

    unsigned int GetCPUCoreCount();

    String GetCPUName();

    Size GetTotalRAM();

    Size GetAvailableRAM();

    String GetGPUName();

    String GetGPUVendor();

    enum class Endianness { Little, Big };

    Endianness GetEndianness();
}

#endif // ASHEN_PLATFORM_H