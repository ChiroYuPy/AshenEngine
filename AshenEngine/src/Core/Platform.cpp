#include "Ashen/Core/Platform.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Platform {
    void enableAnsiColors() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    std::string getPlatformName() {
#if defined(_WIN32)
        return "Windows";
#elif defined(__APPLE__) || defined(__MACH__)
        return "macOS";
#elif defined(__linux__)
        return "Linux";
#elif defined(__unix__)
        return "Unix";
#else
        return "Unknown";
#endif
    }
}