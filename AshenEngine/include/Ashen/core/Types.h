#ifndef ASHEN_TYPES_H
#define ASHEN_TYPES_H

#include <memory>

namespace ash {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr Scope<T> MakeScope(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr Ref<T> MakeRef(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    typedef uint32_t GL_ID;
}

#endif //ASHEN_TYPES_H
