#ifndef ASHEN_RANDOM_H
#define ASHEN_RANDOM_H

#include <random>

#include "Math.h"
#include "Ashen/Core/Types.h"

namespace ash {
    class Random {
    public:
        static void Init(const uint64_t seed) {
            thread_local_engine().seed(seed);
        }

        template<typename T,
            typename = std::enable_if_t<std::is_integral_v<T> > >
        static T Int(T min, T max) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(thread_local_engine());
        }

        static uint64_t UInt64() {
            return Int<uint64_t>(0, LimitMax<uint64_t>());
        }

        template<typename T,
            typename = std::enable_if_t<std::is_floating_point_v<T> > >
        static T Float(T min = T(0), T max = T(1)) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(thread_local_engine());
        }

        static bool Bool(const double probability = 0.5) {
            std::bernoulli_distribution dist(probability);
            return dist(thread_local_engine());
        }

        template<typename T>
        static T Choice(const std::vector<T> &vec) {
            if (vec.empty()) throw std::runtime_error("Random::Choice: container is empty");
            size_t idx = Int<size_t>(0, vec.size() - 1);
            return vec[idx];
        }

        static uint64_t UUID64() {
            return UInt64();
        }

    private:
        static std::mt19937_64 &thread_local_engine() {
            thread_local std::mt19937_64 engine(std::random_device{}());
            return engine;
        }
    };
}

#endif // ASHEN_RANDOM_H