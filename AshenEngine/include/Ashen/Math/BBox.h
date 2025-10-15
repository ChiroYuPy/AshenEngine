#ifndef RECT_RECT2_H
#define RECT_RECT2_H

#include <glm/glm.hpp>

namespace ash {

    template<int Dim, typename T>
    struct BBox {
        glm::vec<Dim, T> min;
        glm::vec<Dim, T> max;

        constexpr BBox()
                : min(glm::vec<Dim, T>(std::numeric_limits<T>::max())),
                  max(glm::vec<Dim, T>(std::numeric_limits<T>::lowest())) {}

        constexpr BBox(const glm::vec<Dim, T>& minVal, const glm::vec<Dim, T>& maxVal)
                   : min(minVal), max(maxVal) {}

        static constexpr BBox fromCenterAndSize(const glm::vec<Dim, T>& center, const glm::vec<Dim, T>& size) {
            glm::vec<Dim, T> half = size * static_cast<T>(0.5);
            return BBox(center - half, center + half);
        }

        static constexpr BBox fromBlock(const glm::vec<Dim, int>& blockPos) {
            return BBox(
                glm::vec<Dim, T>(blockPos),
                glm::vec<Dim, T>(blockPos) + glm::vec<Dim, T>(1)
            );
        }

        constexpr void encapsulate(const glm::vec<Dim, T>& p) {
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        [[nodiscard]] constexpr bool contains(const glm::vec<Dim, T>& p) const {
            return glm::all(glm::lessThanEqual(min, p)) &&
                   glm::all(glm::lessThanEqual(p, max));
        }

        [[nodiscard]] constexpr bool intersects(const BBox& other) const {
            for (int i = 0; i < Dim; ++i)
                if (max[i] < other.min[i] || min[i] > other.max[i])
                    return false;
            return true;
        }

        [[nodiscard]] constexpr glm::vec<Dim, T> center() const {
            return (min + max) * static_cast<T>(0.5);
        }

        [[nodiscard]] constexpr glm::vec<Dim, T> size() const {
            return max - min;
        }

        constexpr void translate(const glm::vec<Dim, T>& offset) {
            min += offset;
            max += offset;
        }

        [[nodiscard]] constexpr BBox translated(const glm::vec<Dim, T>& offset) const {
            return BBox(min + offset, max + offset);
        }

        constexpr void expand(const glm::vec<Dim, T>& amount) {
            min -= amount;
            max += amount;
        }

        [[nodiscard]] constexpr BBox expanded(T amount) const {
            glm::vec<Dim, T> e(amount);
            return BBox(min - e, max + e);
        }

        [[nodiscard]] constexpr BBox unite(const BBox& other) const {
            return BBox(glm::min(min, other.min), glm::max(max, other.max));
        }

        [[nodiscard]] constexpr BBox intersected(const BBox& other) const {
            return BBox(glm::max(min, other.min), glm::min(max, other.max));
        }

        [[nodiscard]] constexpr bool empty() const {
            return glm::any(glm::lessThan(max, min));
        }

        [[nodiscard]] constexpr glm::vec<Dim, T> pos() const { return min; }
    };

    using BBox2 = BBox<2, float>;
    using BBox3 = BBox<3, float>;
}

#endif //RECT_RECT2_H