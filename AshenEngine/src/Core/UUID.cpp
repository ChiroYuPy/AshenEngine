#include "Ashen/Core/UUID.h"

namespace ash {
    UUID::UUID() noexcept : m_ID(Random::UUID64()) {
    }

    constexpr UUID::UUID(const u64 id) noexcept : m_ID(id) {
    }
}
