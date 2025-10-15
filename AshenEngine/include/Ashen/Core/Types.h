#ifndef ASHEN_TYPES_H
#define ASHEN_TYPES_H

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <string_view>

namespace ash {
    using String = std::string;
    using StringView = std::string_view;

    template<typename T>
     constexpr decltype(auto) Move(T&& t) noexcept {
        return std::move(t);
    }

    template<typename T>
     using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    [[nodiscard]] constexpr Ref<T> MakeRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Own = std::unique_ptr<T>;

    template<typename T, typename... Args>
    [[nodiscard]] constexpr Own<T> MakeOwn(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using WeakRef = std::weak_ptr<T>;

    template<typename T>
    using Vector = std::vector<T>;

    template<typename K, typename V>
    using Map = std::map<K, V>;

    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    template<typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    template<typename T>
    using Optional = std::optional<T>;

    template<typename... Ts>
    using Variant = std::variant<Ts...>;
}

#endif //ASHEN_TYPES_H
