#ifndef ASHEN_TYPES_H
#define ASHEN_TYPES_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <deque>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <functional>
#include <span>
#include <type_traits>

namespace ash {
    // ========== Primitive Types ==========
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    using byte = unsigned char;
    using Size = std::size_t;

    // ========== String Types ==========
    using String = std::string;
    using StringView = std::string_view;

    // ========== Smart Pointers ==========
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T>
    using Own = std::unique_ptr<T>;

    template<typename T>
    using WeakRef = std::weak_ptr<T>;

    // Smart pointer creation with perfect forwarding
    template<typename T, typename... Args>
    [[nodiscard]] constexpr Ref<T> MakeRef(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr Own<T> MakeOwn(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Smart pointer utilities
    template<typename T>
    constexpr T *RawPtr(const Ref<T> &ptr) noexcept {
        return ptr.get();
    }

    template<typename T>
    constexpr T *RawPtr(const Own<T> &ptr) noexcept {
        return ptr.get();
    }

    template<typename T>
    constexpr decltype(auto) MovePtr(T &&ptr) noexcept {
        return std::move(ptr);
    }

    // ========== Container Types ==========
    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using Deque = std::deque<T>;

    template<typename T, Size N>
    using Array = std::array<T, N>;

    template<typename T>
    using Span = std::span<T>;

    template<typename K, typename V>
    using Map = std::map<K, V>;

    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    template<typename T>
    using Set = std::set<T>;

    template<typename T>
    using HashSet = std::unordered_set<T>;

    template<typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    // ========== Optional & Variant ==========
    template<typename T>
    using Optional = std::optional<T>;

    template<typename... Ts>
    using Variant = std::variant<Ts...>;

    template<typename T>
    using Function = std::function<T>;

    // ========== Type Traits Helpers ==========
    template<typename T>
    using RemoveRef = std::remove_reference_t<T>;

    template<typename T>
    using RemoveConst = std::remove_const_t<T>;

    template<typename T>
    using RemoveCVRef = std::remove_cvref_t<T>;

    template<typename From, typename To>
    inline constexpr bool IsConvertible = std::is_convertible_v<From, To>;

    template<typename Base, typename Derived>
    inline constexpr bool IsBaseOf = std::is_base_of_v<Base, Derived>;

    template<typename T>
    inline constexpr bool IsPointer = std::is_pointer_v<T>;

    template<typename T>
    inline constexpr bool IsIntegral = std::is_integral_v<T>;

    template<typename T>
    inline constexpr bool IsFloating = std::is_floating_point_v<T>;

    // ========== Callback Types ==========
    template<typename... Args>
    using Callback = Function<void(Args...)>;

    template<typename T>
    using Predicate = Function<bool(const T &)>;

    template<typename T, typename R>
    using Mapper = Function<R(const T &)>;
} // namespace ash

#endif // ASHEN_TYPES_H