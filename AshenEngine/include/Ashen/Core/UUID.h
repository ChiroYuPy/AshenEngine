#ifndef ASHEN_UUID_H
#define ASHEN_UUID_H

#include "Ashen/Math/Random.h"
#include <iomanip>
#include <sstream>

namespace ash {
    class UUID {
    public:
        UUID() noexcept : m_Id(Random::UUID64()) {
        }

        explicit constexpr UUID(const uint64_t id) noexcept : m_Id(id) {
        }

        [[nodiscard]] constexpr uint64_t Get() const noexcept { return m_Id; }

        [[nodiscard]] std::string ToString() const {
            std::ostringstream ss;
            ss << std::hex << std::setw(16) << std::setfill('0') << m_Id;
            return ss.str();
        }

        constexpr bool operator==(const UUID &other) const noexcept { return m_Id == other.m_Id; }
        constexpr bool operator!=(const UUID &other) const noexcept { return m_Id != other.m_Id; }
        constexpr bool operator<(const UUID &other) const noexcept { return m_Id < other.m_Id; }

    private:
        uint64_t m_Id;
    };
} // namespace ash

template<>
struct std::hash<ash::UUID> {
    size_t operator()(const ash::UUID &id) const noexcept {
        return std::hash<uint64_t>{}(id.Get());
    }
};

#endif // ASHEN_UUID_H