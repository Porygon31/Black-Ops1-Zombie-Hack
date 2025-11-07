#pragma once

#include <cmath>
#include <array>

namespace bo1::math {

/**
 * @brief Vector 2D template class with modern C++ features
 */
template<typename T>
struct Vector2 {
    T x{}, y{};

    constexpr Vector2() = default;
    constexpr Vector2(T x, T y) : x(x), y(y) {}

    // Operators
    constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return {x + other.x, y + other.y};
    }

    constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return {x - other.x, y - other.y};
    }

    constexpr Vector2 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar};
    }

    constexpr Vector2 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar};
    }

    constexpr Vector2& operator+=(const Vector2& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector2& operator-=(const Vector2& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    [[nodiscard]] constexpr T dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] T length() const noexcept {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr T lengthSquared() const noexcept {
        return x * x + y * y;
    }

    [[nodiscard]] Vector2 normalized() const noexcept {
        const T len = length();
        return len > T(0) ? *this / len : Vector2{};
    }
};

/**
 * @brief Vector 3D template class with modern C++ features
 */
template<typename T>
struct Vector3 {
    T x{}, y{}, z{};

    constexpr Vector3() = default;
    constexpr Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

    // Operators
    constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return {x - other.x, y - other.y, z - other.z};
    }

    constexpr Vector3 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr Vector3 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar, z / scalar};
    }

    constexpr Vector3& operator+=(const Vector3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    [[nodiscard]] constexpr T dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] constexpr Vector3 cross(const Vector3& other) const noexcept {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    [[nodiscard]] T length() const noexcept {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] constexpr T lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] Vector3 normalized() const noexcept {
        const T len = length();
        return len > T(0) ? *this / len : Vector3{};
    }
};

/**
 * @brief Vector 4D template class for homogeneous coordinates
 */
template<typename T>
struct Vector4 {
    T x{}, y{}, z{}, w{};

    constexpr Vector4() = default;
    constexpr Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    constexpr Vector4(const Vector3<T>& v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}

    [[nodiscard]] constexpr T dot(const Vector4& other) const noexcept {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
};

// Type aliases for common use cases
using Vec2f = Vector2<float>;
using Vec3f = Vector3<float>;
using Vec4f = Vector4<float>;

using Vec2i = Vector2<int>;
using Vec3i = Vector3<int>;

} // namespace bo1::math
