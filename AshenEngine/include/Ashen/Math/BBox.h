#ifndef ASHEN_BBOX_H
#define ASHEN_BBOX_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include <glm/glm.hpp>

namespace ash {
    template<int Dim, typename T>
    struct BBox {
        using VecType = glm::vec<Dim, T>;

        VecType min;
        VecType max;

        // ===== Constructeurs =====
        constexpr BBox()
            : min(std::numeric_limits<T>::max())
              , max(std::numeric_limits<T>::lowest()) {
        }

        constexpr BBox(const VecType &minVal, const VecType &maxVal)
            : min(minVal), max(maxVal) {
        }

        // ===== Static Constructors =====
        static BBox FromCenterAndSize(const VecType &center, const VecType &size) {
            VecType halfSize = size * T(0.5);
            return BBox(center - halfSize, center + halfSize);
        }

        static BBox FromCenterAndExtents(const VecType &center, const VecType &extents) {
            return BBox(center - extents, center + extents);
        }

        static BBox FromPoints(const Vector<VecType> &points) {
            BBox bbox;
            for (const auto &p: points) {
                bbox.Encapsulate(p);
            }
            return bbox;
        }

        static BBox FromSphere(const VecType &center, T radius) {
            VecType offset(radius);
            return BBox(center - offset, center + offset);
        }

        // For 3D voxels/blocks
        static BBox FromBlock(const glm::vec<Dim, int> &blockPos) {
            return BBox(VecType(blockPos), VecType(blockPos) + VecType(T(1)));
        }

        static BBox Invalid() {
            return BBox(); // Returns inverted bounds
        }

        static BBox Infinite() {
            return BBox(
                VecType(std::numeric_limits<T>::lowest()),
                VecType(std::numeric_limits<T>::max())
            );
        }

        // ===== Properties =====
        [[nodiscard]] VecType Center() const {
            return (min + max) * T(0.5);
        }

        [[nodiscard]] VecType Size() const {
            return max - min;
        }

        [[nodiscard]] VecType Extents() const {
            return (max - min) * T(0.5);
        }

        [[nodiscard]] T Volume() const {
            if constexpr (Dim == 2) {
                VecType size = Size();
                return size.x * size.y;
            } else if constexpr (Dim == 3) {
                VecType size = Size();
                return size.x * size.y * size.z;
            }
            return T(0);
        }

        [[nodiscard]] T SurfaceArea() const {
            if constexpr (Dim == 3) {
                VecType size = Size();
                return T(2) * (size.x * size.y + size.y * size.z + size.z * size.x);
            } else if constexpr (Dim == 2) {
                VecType size = Size();
                return T(2) * (size.x + size.y); // Perimeter
            }
            return T(0);
        }

        [[nodiscard]] bool IsValid() const {
            return glm::all(glm::lessThanEqual(min, max));
        }

        [[nodiscard]] bool IsEmpty() const {
            return !IsValid() || glm::any(glm::equal(min, max));
        }

        [[nodiscard]] bool IsInfinite() const {
            return glm::any(glm::equal(min, VecType(std::numeric_limits<T>::lowest()))) ||
                   glm::any(glm::equal(max, VecType(std::numeric_limits<T>::max())));
        }

        // ===== Queries =====
        [[nodiscard]] bool Contains(const VecType &point) const {
            return glm::all(glm::greaterThanEqual(point, min)) &&
                   glm::all(glm::lessThanEqual(point, max));
        }

        [[nodiscard]] bool Contains(const BBox &other) const {
            return glm::all(glm::lessThanEqual(min, other.min)) &&
                   glm::all(glm::greaterThanEqual(max, other.max));
        }

        [[nodiscard]] bool Intersects(const BBox &other) const {
            return glm::all(glm::lessThanEqual(min, other.max)) &&
                   glm::all(glm::greaterThanEqual(max, other.min));
        }

        [[nodiscard]] bool IntersectsStrict(const BBox &other) const {
            return glm::all(glm::lessThan(min, other.max)) &&
                   glm::all(glm::greaterThan(max, other.min));
        }

        // Ray intersection (for 3D)
        template<int D = Dim>
        [[nodiscard]] typename std::enable_if<D == 3, bool>::type
        IntersectsRay(const VecType &origin, const VecType &direction,
                      T *tMin = nullptr, T *tMax = nullptr) const {
            VecType invDir = VecType(T(1)) / direction;
            VecType t0 = (min - origin) * invDir;
            VecType t1 = (max - origin) * invDir;

            VecType tNear = glm::min(t0, t1);
            VecType tFar = glm::max(t0, t1);

            T nearDist = std::max({tNear.x, tNear.y, tNear.z});
            T farDist = std::min({tFar.x, tFar.y, tFar.z});

            if (tMin) *tMin = nearDist;
            if (tMax) *tMax = farDist;

            return nearDist <= farDist && farDist >= T(0);
        }

        // ===== Modifications =====
        void Encapsulate(const VecType &point) {
            min = glm::min(min, point);
            max = glm::max(max, point);
        }

        void Encapsulate(const BBox &other) {
            if (other.IsValid()) {
                min = glm::min(min, other.min);
                max = glm::max(max, other.max);
            }
        }

        void Expand(const VecType &amount) {
            min -= amount;
            max += amount;
        }

        void Expand(T amount) {
            Expand(VecType(amount));
        }

        void Translate(const VecType &offset) {
            min += offset;
            max += offset;
        }

        void Scale(const VecType &scale) {
            VecType center = Center();
            VecType halfSize = Extents() * scale;
            min = center - halfSize;
            max = center + halfSize;
        }

        void Scale(T uniformScale) {
            Scale(VecType(uniformScale));
        }

        void Reset() {
            min = VecType(std::numeric_limits<T>::max());
            max = VecType(std::numeric_limits<T>::lowest());
        }

        // ===== Immutable Operations =====
        [[nodiscard]] BBox Expanded(const VecType &amount) const {
            return BBox(min - amount, max + amount);
        }

        [[nodiscard]] BBox Expanded(T amount) const {
            return Expanded(VecType(amount));
        }

        [[nodiscard]] BBox Translated(const VecType &offset) const {
            return BBox(min + offset, max + offset);
        }

        [[nodiscard]] BBox Scaled(const VecType &scale) const {
            VecType center = Center();
            VecType halfSize = Extents() * scale;
            return BBox(center - halfSize, center + halfSize);
        }

        [[nodiscard]] BBox Scaled(T uniformScale) const {
            return Scaled(VecType(uniformScale));
        }

        // ===== Set Operations =====
        [[nodiscard]] BBox Union(const BBox &other) const {
            if (!IsValid()) return other;
            if (!other.IsValid()) return *this;
            return BBox(glm::min(min, other.min), glm::max(max, other.max));
        }

        [[nodiscard]] BBox Intersection(const BBox &other) const {
            if (!Intersects(other)) return Invalid();
            return BBox(glm::max(min, other.min), glm::min(max, other.max));
        }

        // ===== Distance Queries =====
        [[nodiscard]] T DistanceToPoint(const VecType &point) const {
            VecType closest = ClosestPoint(point);
            return glm::length(closest - point);
        }

        [[nodiscard]] T DistanceSquaredToPoint(const VecType &point) const {
            VecType closest = ClosestPoint(point);
            VecType diff = closest - point;
            return glm::dot(diff, diff);
        }

        [[nodiscard]] VecType ClosestPoint(const VecType &point) const {
            return glm::clamp(point, min, max);
        }

        // ===== Subdivision (useful for spatial partitioning) =====
        [[nodiscard]] Vector<BBox> Subdivide() const {
            VecType center = Center();
            Vector<BBox> result;

            if constexpr (Dim == 2) {
                result.reserve(4);
                // Bottom-left, bottom-right, top-right, top-left
                result.emplace_back(min, center);
                result.emplace_back(VecType(center.x, min.y), VecType(max.x, center.y));
                result.emplace_back(center, max);
                result.emplace_back(VecType(min.x, center.y), VecType(center.x, max.y));
            } else if constexpr (Dim == 3) {
                result.reserve(8);
                for (int i = 0; i < 8; ++i) {
                    VecType childMin(
                        i & 1 ? center.x : min.x,
                        i & 2 ? center.y : min.y,
                        i & 4 ? center.z : min.z
                    );
                    VecType childMax(
                        i & 1 ? max.x : center.x,
                        i & 2 ? max.y : center.y,
                        i & 4 ? max.z : center.z
                    );
                    result.emplace_back(childMin, childMax);
                }
            }

            return result;
        }

        // ===== Corner Points =====
        [[nodiscard]] Vector<VecType> GetCorners() const {
            Vector<VecType> corners;

            if constexpr (Dim == 2) {
                corners.reserve(4);
                corners.emplace_back(min.x, min.y);
                corners.emplace_back(max.x, min.y);
                corners.emplace_back(max.x, max.y);
                corners.emplace_back(min.x, max.y);
            } else if constexpr (Dim == 3) {
                corners.reserve(8);
                for (int i = 0; i < 8; ++i) {
                    corners.emplace_back(
                        i & 1 ? max.x : min.x,
                        i & 2 ? max.y : min.y,
                        i & 4 ? max.z : min.z
                    );
                }
            }

            return corners;
        }

        template<int D = Dim>
        [[nodiscard]] std::enable_if_t<D == 3, BBox>
        Transformed(const Mat4 &matrix) const {
            Vector<VecType> corners = GetCorners();
            BBox result = Invalid();

            for (const auto &corner: corners) {
                Vec4 transformed = matrix * Vec4(corner, T(1));
                result.Encapsulate(VecType(transformed) / transformed.w);
            }

            return result;
        }

        BBox &operator+=(const VecType &offset) {
            Translate(offset);
            return *this;
        }

        BBox &operator-=(const VecType &offset) {
            Translate(-offset);
            return *this;
        }

        BBox operator+(const VecType &offset) const {
            return Translated(offset);
        }

        BBox operator-(const VecType &offset) const {
            return Translated(-offset);
        }

        bool operator==(const BBox &other) const {
            return glm::all(glm::equal(min, other.min)) &&
                   glm::all(glm::equal(max, other.max));
        }

        bool operator!=(const BBox &other) const {
            return !(*this == other);
        }

        [[nodiscard]] VecType Pos() const { return min; }

        [[nodiscard]] VecType GetMin() const { return min; }
        [[nodiscard]] VecType GetMax() const { return max; }
    };

    using BBox2 = BBox<2, float>;
    using BBox3 = BBox<3, float>;
    using BBox2i = BBox<2, int>;
    using BBox3i = BBox<3, int>;
    using BBox2d = BBox<2, double>;
    using BBox3d = BBox<3, double>;
}

#endif // ASHEN_BBOX_H