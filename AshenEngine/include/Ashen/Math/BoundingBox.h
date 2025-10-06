#ifndef RECT_RECT2_H
#define RECT_RECT2_H

#include <glm/glm.hpp>

namespace ash {
    template<typename T, int Dim>
    struct BoundingBox {
        glm::vec<Dim, T> min;
        glm::vec<Dim, T> max;

        BoundingBox()
            : min(glm::vec < Dim, T > (std::numeric_limits<T>::max())),
              max(glm::vec < Dim, T > (std::numeric_limits<T>::lowest())) {
        }

        BoundingBox(const glm::vec<Dim, T> &minVal, const glm::vec<Dim, T> &maxVal)
            : min(minVal), max(maxVal) {
        }

        static BoundingBox fromCenterAndSize(const glm::vec<Dim, T> &center, const glm::vec<Dim, T> &size) {
            glm::vec < Dim, T > halfSize = size * static_cast<T>(0.5);
            return BoundingBox(center - halfSize, center + halfSize);
        }

        static BoundingBox fromBlock(const glm::vec<Dim, int> &blockPos) {
            return BoundingBox(
                glm::vec < Dim, T > (blockPos),
                glm::vec < Dim, T > (blockPos) + glm::vec < Dim, T > (static_cast<T>(1))
            );
        }

        void Encapsulate(const glm::vec<Dim, T> &point) {
            min = glm::min(min, point);
            max = glm::max(max, point);
        }

        bool Contains(const glm::vec<Dim, T> &point) const {
            return glm::all(glm::lessThanEqual(min, point)) &&
                   glm::all(glm::lessThanEqual(point, max));
        }

        bool Intersects(const BoundingBox &other) const {
            for (int i = 0; i < Dim; ++i) {
                if (max[i] < other.min[i] || min[i] > other.max[i])
                    return false;
            }
            return true;
        }

        glm::vec<Dim, T> GetCenter() const {
            return (min + max) * static_cast<T>(0.5);
        }

        glm::vec<Dim, T> GetSize() const {
            return max - min;
        }

        void Translate(const glm::vec<Dim, T> &offset) {
            min += offset;
            max += offset;
        }

        BoundingBox Offset(const glm::vec<Dim, T> &offset) const {
            return BoundingBox(min + offset, max + offset);
        }

        void Expand(const glm::vec<Dim, T> &amount) {
            min -= amount;
            max += amount;
        }

        BoundingBox Expand(T amount) const {
            glm::vec < Dim, T > expansion(amount);
            return BoundingBox(min - expansion, max + expansion);
        }
    };

    using BoundingBox2D = BoundingBox<float, 2>;
    using BoundingBox3D = BoundingBox<float, 3>;
}

#endif //RECT_RECT2_H