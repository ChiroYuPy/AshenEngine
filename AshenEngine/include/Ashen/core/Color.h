#ifndef ASHENENGINE_COLOR_H
#define ASHENENGINE_COLOR_H

#include <algorithm>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <glm/glm.hpp>

#include "Ashen/math/Math.h"

namespace pixl {
    class Color {
    public:
        float r, g, b, a;

        constexpr explicit Color()
            : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {
        }

        constexpr explicit Color(const float gray, const float alpha = 1.0f)
            : r(gray), g(gray), b(gray), a(alpha) {
        }

        constexpr explicit Color(const float r, const float g, const float b, const float a = 1.0f)
            : r(r), g(g), b(b), a(a) {
        }

        static constexpr Color fromBytes(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255) {
            return Color(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f,
                         static_cast<float>(a) / 255.f);
        }

        static Color fromHex(const std::string &hex) {
            std::string clean = hex;
            if (!clean.empty() && clean[0] == '#')
                clean = clean.substr(1);

            const uint32_t value = std::stoul(clean, nullptr, 16);
            if (clean.size() == 6)
                return fromBytes(value >> 16 & 0xFF, value >> 8 & 0xFF, value & 0xFF);
            if (clean.size() == 8)
                return fromBytes(value >> 24 & 0xFF, value >> 16 & 0xFF, value >> 8 & 0xFF, value & 0xFF);

            return white();
        }

        [[nodiscard]] Vec4 toVec4() const noexcept { return {r, g, b, a}; }
        [[nodiscard]] Vec3 toVec3() const noexcept { return {r, g, b}; }

        [[nodiscard]] uint32_t toUInt32() const noexcept {
            const uint8_t R = static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f);
            const uint8_t G = static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f);
            const uint8_t B = static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f);
            const uint8_t A = static_cast<uint8_t>(std::clamp(a, 0.0f, 1.0f) * 255.0f);
            return R << 24 | G << 16 | B << 8 | A;
        }

        [[nodiscard]] std::string toHex(const bool includeAlpha = false) const {
            std::ostringstream ss;
            ss << "#" << std::hex << std::setfill('0')
                    << std::setw(2) << static_cast<int>(r * 255)
                    << std::setw(2) << static_cast<int>(g * 255)
                    << std::setw(2) << static_cast<int>(b * 255);
            if (includeAlpha)
                ss << std::setw(2) << static_cast<int>(a * 255);
            return ss.str();
        }

        // === Operators ===
        constexpr Color operator+(const Color &other) const noexcept {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }

        constexpr Color operator-(const Color &other) const noexcept {
            return Color(r - other.r, g - other.g, b - other.b, a - other.a);
        }

        constexpr Color operator*(const float scalar) const noexcept {
            return Color(r * scalar, g * scalar, b * scalar, a * scalar);
        }

        constexpr Color operator*(const Color &other) const noexcept {
            return Color(r * other.r, g * other.g, b * other.b, a * other.a);
        }

        constexpr Color &operator*=(const float scalar) noexcept {
            r *= scalar;
            g *= scalar;
            b *= scalar;
            a *= scalar;
            return *this;
        }

        constexpr Color &operator+=(const Color &other) noexcept {
            r += other.r;
            g += other.g;
            b += other.b;
            a += other.a;
            return *this;
        }

        [[nodiscard]] constexpr Color clamped() const noexcept {
            return Color(
                std::clamp(r, 0.f, 1.f),
                std::clamp(g, 0.f, 1.f),
                std::clamp(b, 0.f, 1.f),
                std::clamp(a, 0.f, 1.f)
            );
        }

        // === Utility ===
        static constexpr Color lerp(const Color &a, const Color &b, const float t) noexcept {
            return Color(
                a.r + (b.r - a.r) * t,
                a.g + (b.g - a.g) * t,
                a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t
            );
        }

        // === Preset Colors ===
        static constexpr Color black() noexcept { return Color(0.f, 0.f, 0.f); }
        static constexpr Color white() noexcept { return Color(1.f, 1.f, 1.f); }
        static constexpr Color red() noexcept { return Color(1.f, 0.f, 0.f); }
        static constexpr Color green() noexcept { return Color(0.f, 1.f, 0.f); }
        static constexpr Color blue() noexcept { return Color(0.f, 0.f, 1.f); }
        static constexpr Color transparent() noexcept { return Color(0.f, 0.f, 0.f, 0.f); }
    };
}

#endif //ASHENENGINE_COLOR_H