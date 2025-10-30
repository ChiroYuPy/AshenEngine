#ifndef ASHEN_UTILITIES_H
#define ASHEN_UTILITIES_H

#include "Ashen/Core/Types.h"
#include <algorithm>
#include <chrono>
#include <source_location>

namespace ash {
    // ========== Scoped Timer ==========
    // RAII timer for profiling code blocks
    class ScopedTimer {
    public:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = Clock::time_point;
        using Duration = std::chrono::duration<double, std::milli>;

        explicit ScopedTimer(const char *name)
            : m_Name(name), m_Start(Clock::now()) {
        }

        ~ScopedTimer() {
            auto end = Clock::now();
            Duration duration = end - m_Start;
            // Log or store timing result
            // Logger::Debug("{}: {:.3f}ms", m_Name, duration.count());
        }

        double GetElapsedMs() const {
            auto now = Clock::now();
            Duration duration = now - m_Start;
            return duration.count();
        }

    private:
        const char *m_Name;
        TimePoint m_Start;
    };

    // ========== Scope Guard ==========
    // Execute code on scope exit
    template<typename F>
    class ScopeGuard {
    public:
        explicit ScopeGuard(F &&func)
            : m_Func(std::forward<F>(func)), m_Active(true) {
        }

        ~ScopeGuard() {
            if (m_Active) m_Func();
        }

        void Dismiss() { m_Active = false; }

        ScopeGuard(const ScopeGuard &) = delete;

        ScopeGuard &operator=(const ScopeGuard &) = delete;

        ScopeGuard(ScopeGuard &&other) noexcept
            : m_Func(std::move(other.m_Func)), m_Active(other.m_Active) {
            other.m_Active = false;
        }

    private:
        F m_Func;
        bool m_Active;
    };

    template<typename F>
    ScopeGuard<F> MakeScopeGuard(F &&func) {
        return ScopeGuard<F>(std::forward<F>(func));
    }

#define SCOPE_EXIT(code) auto CONCAT(_scope_guard_, __LINE__) = \
        ::ash::MakeScopeGuard([&]() { code; })

    // ========== String Utilities ==========
    namespace StringUtils {
        inline String ToLower(StringView str) {
            String result(str);
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        inline String ToUpper(StringView str) {
            String result(str);
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return result;
        }

        inline bool StartsWith(StringView str, StringView prefix) {
            return str.size() >= prefix.size() &&
                   str.substr(0, prefix.size()) == prefix;
        }

        inline bool EndsWith(StringView str, StringView suffix) {
            return str.size() >= suffix.size() &&
                   str.substr(str.size() - suffix.size()) == suffix;
        }

        inline bool Contains(StringView str, StringView substr) {
            return str.find(substr) != StringView::npos;
        }

        inline String Trim(StringView str) {
            auto start = str.find_first_not_of(" \t\n\r");
            if (start == StringView::npos) return "";
            auto end = str.find_last_not_of(" \t\n\r");
            return String(str.substr(start, end - start + 1));
        }

        inline String TrimLeft(StringView str) {
            auto start = str.find_first_not_of(" \t\n\r");
            return start == StringView::npos ? "" : String(str.substr(start));
        }

        inline String TrimRight(StringView str) {
            auto end = str.find_last_not_of(" \t\n\r");
            return end == StringView::npos ? "" : String(str.substr(0, end + 1));
        }

        inline Vector<String> Split(StringView str, char delimiter) {
            Vector<String> tokens;
            size_t start = 0;
            size_t end = str.find(delimiter);

            while (end != StringView::npos) {
                tokens.emplace_back(str.substr(start, end - start));
                start = end + 1;
                end = str.find(delimiter, start);
            }

            tokens.emplace_back(str.substr(start));
            return tokens;
        }

        inline String Join(const Vector<String> &strings, StringView delimiter) {
            if (strings.empty()) return "";

            String result = strings[0];
            for (size_t i = 1; i < strings.size(); ++i) {
                result += delimiter;
                result += strings[i];
            }
            return result;
        }

        inline String Replace(StringView str, StringView from, StringView to) {
            String result(str);
            size_t pos = 0;
            while ((pos = result.find(from, pos)) != String::npos) {
                result.replace(pos, from.length(), to);
                pos += to.length();
            }
            return result;
        }

        inline bool IsNumeric(StringView str) {
            return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
        }

        inline bool IsAlphabetic(StringView str) {
            return !str.empty() && std::all_of(str.begin(), str.end(), ::isalpha);
        }

        inline bool IsAlphanumeric(StringView str) {
            return !str.empty() && std::all_of(str.begin(), str.end(), ::isalnum);
        }
    }

    // ========== Container Utilities ==========
    namespace ContainerUtils {
        template<typename Container, typename Pred>
        void EraseIf(Container &container, Pred predicate) {
            auto it = std::remove_if(container.begin(), container.end(), predicate);
            container.erase(it, container.end());
        }

        template<typename Container, typename T>
        bool Contains(const Container &container, const T &value) {
            return std::find(container.begin(), container.end(), value) != container.end();
        }

        template<typename Container>
        void Reverse(Container &container) {
            std::reverse(container.begin(), container.end());
        }

        template<typename Container>
        void Sort(Container &container) {
            std::sort(container.begin(), container.end());
        }

        template<typename Container, typename Comp>
        void Sort(Container &container, Comp comp) {
            std::sort(container.begin(), container.end(), comp);
        }

        template<typename Container>
        void Unique(Container &container) {
            Sort(container);
            auto it = std::unique(container.begin(), container.end());
            container.erase(it, container.end());
        }

        template<typename Container, typename Func>
        auto Map(const Container &container, Func func) {
            using ResultType = decltype(func(*container.begin()));
            Vector<ResultType> result;
            result.reserve(container.size());

            for (const auto &item: container) {
                result.push_back(func(item));
            }

            return result;
        }

        template<typename Container, typename Pred>
        auto Filter(const Container &container, Pred predicate) {
            Container result;
            std::copy_if(container.begin(), container.end(),
                         std::back_inserter(result), predicate);
            return result;
        }

        template<typename Container, typename T, typename BinaryOp>
        T Reduce(const Container &container, T initial, BinaryOp op) {
            return std::accumulate(container.begin(), container.end(), initial, op);
        }

        template<typename K, typename V>
        Vector<K> Keys(const HashMap<K, V> &map) {
            Vector<K> keys;
            keys.reserve(map.size());
            for (const auto &[key, value]: map) {
                keys.push_back(key);
            }
            return keys;
        }

        template<typename K, typename V>
        Vector<V> Values(const HashMap<K, V> &map) {
            Vector<V> values;
            values.reserve(map.size());
            for (const auto &[key, value]: map) {
                values.push_back(value);
            }
            return values;
        }
    }

    // ========== File Path Utilities ==========
    namespace PathUtils {
        inline String GetExtension(StringView path) {
            auto pos = path.find_last_of('.');
            return pos == StringView::npos ? "" : String(path.substr(pos + 1));
        }

        inline String GetFilename(StringView path) {
            auto pos = path.find_last_of("/\\");
            return pos == StringView::npos ? String(path) : String(path.substr(pos + 1));
        }

        inline String GetDirectory(StringView path) {
            auto pos = path.find_last_of("/\\");
            return pos == StringView::npos ? "" : String(path.substr(0, pos));
        }

        inline String GetFilenameWithoutExtension(StringView path) {
            String filename = GetFilename(path);
            auto pos = filename.find_last_of('.');
            return pos == String::npos ? filename : filename.substr(0, pos);
        }

        inline String Join(StringView path1, StringView path2) {
            if (path1.empty()) return String(path2);
            if (path2.empty()) return String(path1);

            String result(path1);
            if (result.back() != '/' && result.back() != '\\') {
                result += '/';
            }
            result += path2;
            return result;
        }

        inline String Normalize(StringView path) {
            String result(path);
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }

        inline bool IsAbsolute(StringView path) {
#ifdef _WIN32
            return path.size() >= 3 && path[1] == ':' && (path[2] == '/' || path[2] == '\\');
#else
            return !path.empty() && path[0] == '/';
#endif
        }
    }

    // ========== Math Utilities ==========
    namespace MathUtils {
        template<typename T>
        constexpr bool IsPowerOfTwo(T value) {
            return value > 0 && (value & value - 1) == 0;
        }

        template<typename T>
        constexpr T NextPowerOfTwo(T value) {
            if (value == 0) return 1;
            --value;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            return value + 1;
        }

        template<typename T>
        constexpr T AlignUp(T value, T alignment) {
            return value + alignment - 1 & ~(alignment - 1);
        }

        template<typename T>
        constexpr T AlignDown(T value, T alignment) {
            return value & ~(alignment - 1);
        }

        inline u32 HashCombine(u32 seed, u32 value) {
            // Based on boost::hash_combine
            return seed ^ value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        inline u64 HashCombine(u64 seed, u64 value) {
            return seed ^ value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        }
    }

    // ========== Bit Manipulation ==========
    namespace BitUtils {
        template<typename T>
        constexpr int CountSetBits(T value) {
            int count = 0;
            while (value) {
                count += value & 1;
                value >>= 1;
            }
            return count;
        }

        template<typename T>
        constexpr int TrailingZeros(T value) {
            if (value == 0) return sizeof(T) * 8;
            int count = 0;
            while ((value & 1) == 0) {
                ++count;
                value >>= 1;
            }
            return count;
        }

        template<typename T>
        constexpr int LeadingZeros(T value) {
            if (value == 0) return sizeof(T) * 8;
            int count = 0;
            T mask = T(1) << (sizeof(T) * 8 - 1);
            while ((value & mask) == 0) {
                ++count;
                mask >>= 1;
            }
            return count;
        }

        template<typename T>
        constexpr bool IsBitSet(T value, int bit) {
            return (value & T(1) << bit) != 0;
        }

        template<typename T>
        constexpr T SetBit(T value, int bit) {
            return value | T(1) << bit;
        }

        template<typename T>
        constexpr T ClearBit(T value, int bit) {
            return value & ~(T(1) << bit);
        }

        template<typename T>
        constexpr T ToggleBit(T value, int bit) {
            return value ^ T(1) << bit;
        }
    }

    // ========== Color Utilities ==========
    namespace ColorUtils {
        inline Vec4 FromRGBA32(u32 rgba) {
            return Vec4(
                (rgba >> 24 & 0xFF) / 255.0f,
                (rgba >> 16 & 0xFF) / 255.0f,
                (rgba >> 8 & 0xFF) / 255.0f,
                (rgba & 0xFF) / 255.0f
            );
        }

        inline u32 ToRGBA32(const Vec4 &color) {
            u32 r = static_cast<u32>(color.r * 255.0f) << 24;
            u32 g = static_cast<u32>(color.g * 255.0f) << 16;
            u32 b = static_cast<u32>(color.b * 255.0f) << 8;
            u32 a = static_cast<u32>(color.a * 255.0f);
            return r | g | b | a;
        }

        inline Vec4 FromHex(u32 hex) {
            return Vec4(
                (hex >> 16 & 0xFF) / 255.0f,
                (hex >> 8 & 0xFF) / 255.0f,
                (hex & 0xFF) / 255.0f,
                1.0f
            );
        }

        inline Vec3 HSVtoRGB(float h, float s, float v) {
            float c = v * s;
            float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
            float m = v - c;

            Vec3 rgb;
            if (h < 60) rgb = Vec3(c, x, 0);
            else if (h < 120) rgb = Vec3(x, c, 0);
            else if (h < 180) rgb = Vec3(0, c, x);
            else if (h < 240) rgb = Vec3(0, x, c);
            else if (h < 300) rgb = Vec3(x, 0, c);
            else rgb = Vec3(c, 0, x);

            return rgb + Vec3(m);
        }

        inline Vec3 RGBtoHSV(const Vec3 &rgb) {
            float cmax = std::max({rgb.r, rgb.g, rgb.b});
            float cmin = std::min({rgb.r, rgb.g, rgb.b});
            float delta = cmax - cmin;

            float h = 0.0f;
            if (delta > 0.0f) {
                if (cmax == rgb.r) {
                    h = 60.0f * std::fmod((rgb.g - rgb.b) / delta, 6.0f);
                } else if (cmax == rgb.g) {
                    h = 60.0f * ((rgb.b - rgb.r) / delta + 2.0f);
                } else {
                    h = 60.0f * ((rgb.r - rgb.g) / delta + 4.0f);
                }
            }

            float s = cmax > 0.0f ? delta / cmax : 0.0f;
            float v = cmax;

            return Vec3(h, s, v);
        }

        inline Vec4 Lerp(const Vec4 &a, const Vec4 &b, float t) {
            return a + (b - a) * t;
        }

        inline Vec4 Premultiply(const Vec4 &color) {
            return Vec4(color.r * color.a, color.g * color.a, color.b * color.a, color.a);
        }

        inline Vec4 Unpremultiply(const Vec4 &color) {
            if (color.a > 0.0f) {
                return Vec4(color.r / color.a, color.g / color.a, color.b / color.a, color.a);
            }
            return color;
        }
    }

    // ========== Enum Utilities ==========
    template<typename E>
    constexpr auto ToUnderlying(E e) noexcept {
        return static_cast<std::underlying_type_t<E>>(e);
    }

    template<typename E>
    constexpr bool HasFlag(E value, E flag) noexcept {
        using T = std::underlying_type_t<E>;
        return (static_cast<T>(value) & static_cast<T>(flag)) == static_cast<T>(flag);
    }

    template<typename E>
    constexpr E AddFlag(E value, E flag) noexcept {
        using T = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<T>(value) | static_cast<T>(flag));
    }

    template<typename E>
    constexpr E RemoveFlag(E value, E flag) noexcept {
        using T = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<T>(value) & ~static_cast<T>(flag));
    }

    template<typename E>
    constexpr E ToggleFlag(E value, E flag) noexcept {
        using T = std::underlying_type_t<E>;
        return static_cast<E>(static_cast<T>(value) ^ static_cast<T>(flag));
    }

    // ========== Debug Utilities ==========
    namespace DebugUtils {
        struct SourceLocation {
            const char *file;
            const char *function;
            int line;

            static SourceLocation Current(
                const char *file = __builtin_FILE(),
                const char *function = __builtin_FUNCTION(),
                int line = __builtin_LINE()
            ) {
                return {file, function, line};
            }
        };

        template<typename T>
        const char *TypeName() {
#ifdef __GNUG__
            return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
            return __FUNCSIG__;
#else
            return "Unknown";
#endif
        }

        inline String FormatBytes(size_t bytes) {
            const char *units[] = {"B", "KB", "MB", "GB", "TB"};
            int unitIndex = 0;
            double size = static_cast<double>(bytes);

            while (size >= 1024.0 && unitIndex < 4) {
                size /= 1024.0;
                ++unitIndex;
            }

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unitIndex]);
            return String(buffer);
        }

        inline String FormatDuration(double milliseconds) {
            if (milliseconds < 1.0) {
                return String(std::to_string(milliseconds * 1000.0) + " µs");
            } else if (milliseconds < 1000.0) {
                return String(std::to_string(milliseconds) + " ms");
            } else {
                return String(std::to_string(milliseconds / 1000.0) + " s");
            }
        }
    }

    // ========== Memory Utilities ==========
    namespace MemoryUtils {
        template<typename T>
        void ZeroMemory(T &obj) {
            std::memset(&obj, 0, sizeof(T));
        }

        template<typename T>
        void ZeroArray(T *array, size_t count) {
            std::memset(array, 0, sizeof(T) * count);
        }

        inline void *AlignedAlloc(size_t size, size_t alignment) {
#ifdef _WIN32
            return _aligned_malloc(size, alignment);
#else
            void *ptr = nullptr;
            posix_memalign(&ptr, alignment, size);
            return ptr;
#endif
        }

        inline void AlignedFree(void *ptr) {
#ifdef _WIN32
            _aligned_free(ptr);
#else
            free(ptr);
#endif
        }
    }

    // ========== Macro Helpers ==========
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

    // ========== Assert & Verify ==========
#ifdef ASH_DEBUG
#define ASH_ASSERT(condition, message) \
            do { \
                if (!(condition)) { \
                    /* Logger::Error("Assertion failed: {} at {}:{}", message, __FILE__, __LINE__); */ \
                    __debugbreak(); \
                } \
            } while(0)
#else
#define ASH_ASSERT(condition, message) ((void)0)
#endif

#define ASH_VERIFY(condition, message) \
        do { \
            if (!(condition)) { \
                /* Logger::Error("Verification failed: {} at {}:{}", message, __FILE__, __LINE__); */ \
            } \
        } while(0)
}

#endif // ASHEN_UTILITIES_H