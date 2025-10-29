#ifndef ASHEN_UUID_H
#define ASHEN_UUID_H

#include "Ashen/Math/Random.h"
#include <iomanip>

namespace ash {
    class UUID {
    public:
        UUID() noexcept;
        explicit constexpr UUID(u64 id) noexcept;

        explicit operator u64() const noexcept { return m_ID; }

        constexpr bool operator==(const UUID& other) const noexcept { return m_ID == other.m_ID; }
        constexpr bool operator!=(const UUID& other) const noexcept { return m_ID != other.m_ID; }

        u64 Get() const noexcept { return m_ID; }

    private:
        u64 m_ID = 0;
    };
} // namespace ash

template<>
struct std::hash<ash::UUID> {
    std::size_t operator()(const ash::UUID& id) const noexcept {
        return id.Get();
    }
};

#endif // ASHEN_UUID_H