#include "Ashen/Core/Platform.h"

#include <cstdlib>
#include <thread>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <intrin.h>
#elif defined(__APPLE__) || defined(__MACH__)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/sysctl.h>
#elif defined(__linux__) || defined(__unix__)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
#endif

#include <GL/gl.h>

namespace ash::Platform {
    bool IsWindows() {
#if defined(_WIN32)
        return true;
#else
        return false;
#endif
    }

    bool IsLinux() {
#if defined(__linux__)
        return true;
#else
        return false;
#endif
    }

    bool IsMac() {
#if defined(__APPLE__) || defined(__MACH__)
        return true;
#else
        return false;
#endif
    }

    bool IsUnix() {
#if defined(__unix__) || defined(__unix)
        return true;
#else
        return false;
#endif
    }

    bool Is64Bit() {
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
        return true;
#else
        return false;
#endif
    }

    String GetPlatformName() {
        if (IsWindows()) return "Windows";
        if (IsLinux()) return "Linux";
        if (IsMac()) return "macOS";
        if (IsUnix()) return "Unix";
        return "Unknown";
    }

    void EnableAnsiColors() {
#ifdef _WIN32
        const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    unsigned int GetCPUCoreCount() {
        return std::thread::hardware_concurrency();
    }

    String GetCPUName() {
#ifdef _WIN32
        int cpuInfo[4] = {0};
        __cpuid(cpuInfo, 0x80000000);
        const unsigned int nExIds = cpuInfo[0];
        char brand[0x40] = {0};
        if (nExIds >= 0x80000004) {
            __cpuid(reinterpret_cast<int *>(cpuInfo), 0x80000002);
            std::memcpy(brand, cpuInfo, 16);
            __cpuid(reinterpret_cast<int *>(cpuInfo), 0x80000003);
            std::memcpy(brand + 16, cpuInfo, 16);
            __cpuid(reinterpret_cast<int *>(cpuInfo), 0x80000004);
            std::memcpy(brand + 32, cpuInfo, 16);
        }
        return String(brand);
#elif defined(__APPLE__) || defined(__MACH__)
        char str[256];
        size_t size = sizeof(str);
        sysctlbyname("machdep.cpu.brand_string", &str, &size, nullptr, 0);
        return String(str);
#elif defined(__linux__)
        std::ifstream cpuinfo("/proc/cpuinfo");
        String line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != String::npos) {
                auto pos = line.find(":");
                if (pos != String::npos)
                    return line.substr(pos + 2);
            }
        }
        return "Unknown CPU";
#else
        return "Unknown CPU";
#endif
    }

    size_t GetTotalRAM() {
#ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys;
#elif defined(__linux__)
        long pages = sysconf(_SC_PHYS_PAGES);
        long page_size = sysconf(_SC_PAGE_SIZE);
        return pages * page_size;
#elif defined(__APPLE__)
        int64_t size;
        size_t len = sizeof(size);
        sysctlbyname("hw.memsize", &size, &len, nullptr, 0);
        return size;
#else
        return 0;
#endif
    }

    size_t GetAvailableRAM() {
#ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullAvailPhys;
#elif defined(__linux__)
        std::ifstream meminfo("/proc/meminfo");
        String line;
        size_t freeMem = 0;
        while (std::getline(meminfo, line)) {
            if (line.find("MemAvailable:") != String::npos) {
                auto pos = line.find_first_of("0123456789");
                if (pos != String::npos)
                    freeMem = std::stoull(line.substr(pos)) * 1024;
                break;
            }
        }
        return freeMem;
#elif defined(__APPLE__)
        int64_t memFree = 0;
        return memFree;
#else
        return 0;
#endif
    }

    String GetGPUName() {
        const GLubyte *renderer = glGetString(GL_RENDERER);
        return renderer ? reinterpret_cast<const char *>(renderer) : "Unknown GPU";
    }

    String GetGPUVendor() {
        const GLubyte *vendor = glGetString(GL_VENDOR);
        return vendor ? reinterpret_cast<const char *>(vendor) : "Unknown Vendor";
    }

    Endianness GetEndianness() {
        uint32_t i = 1;
        return *reinterpret_cast<uint8_t *>(&i) == 1 ? Endianness::Little : Endianness::Big;
    }
}
