#ifndef ASHEN_UID_H
#define ASHEN_UID_H

#include <iomanip>
#include <random>
#include <sstream>

namespace ash {

    class UID {
    public:
        UID() : m_id(Generate()) {}
        explicit UID(const uint64_t id) : m_id(id) {}

        uint64_t Get() const { return m_id; }

        bool operator==(const UID& other) const { return m_id == other.m_id; }
        bool operator!=(const UID& other) const { return m_id != other.m_id; }
        bool operator<(const UID& other) const { return m_id < other.m_id; }

        std::string ToString() const {
            std::stringstream ss;
            ss << std::hex << std::setw(16) << std::setfill('0') << m_id;
            return ss.str();
        }

        static UID FromString(const std::string& str) {
            if (str.size() != 16)
                throw std::invalid_argument("UID string must have exactly 16 hex characters.");

            uint64_t id = 0;
            for (const char c : str)
                if (!std::isxdigit(static_cast<unsigned char>(c)))
                    throw std::invalid_argument("UID string contains non-hexadecimal characters.");

            std::stringstream ss(str);
            ss >> std::hex >> id;
            return UID(id);
        }

        static uint64_t Generate() {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<uint64_t> dis;
            return dis(gen);
        }

    private:
        uint64_t m_id;
    };

}

#endif //ASHEN_UID_H