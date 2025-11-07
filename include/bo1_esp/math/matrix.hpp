#pragma once

#include "vector.hpp"
#include <array>
#include <optional>

namespace bo1::math {

/**
 * @brief 4x4 Matrix class for 3D transformations
 */
class Matrix4x4 {
public:
    constexpr Matrix4x4() = default;

    explicit Matrix4x4(const float* data) {
        if (data) {
            std::copy(data, data + 16, m_data.begin());
        }
    }

    [[nodiscard]] const float* data() const noexcept { return m_data.data(); }
    [[nodiscard]] float* data() noexcept { return m_data.data(); }

    [[nodiscard]] float operator[](size_t index) const noexcept {
        return m_data[index];
    }

    [[nodiscard]] float& operator[](size_t index) noexcept {
        return m_data[index];
    }

    /**
     * @brief Transform a 3D point using this matrix
     */
    [[nodiscard]] Vec4f transform(const Vec3f& point) const noexcept {
        return Vec4f{
            point.x * m_data[0] + point.y * m_data[1] + point.z * m_data[2] + m_data[3],
            point.x * m_data[4] + point.y * m_data[5] + point.z * m_data[6] + m_data[7],
            point.x * m_data[8] + point.y * m_data[9] + point.z * m_data[10] + m_data[11],
            point.x * m_data[12] + point.y * m_data[13] + point.z * m_data[14] + m_data[15]
        };
    }

    /**
     * @brief Project 3D world coordinates to 2D screen coordinates
     * @return Screen coordinates if projection is valid, std::nullopt otherwise
     */
    [[nodiscard]] std::optional<Vec2f> worldToScreen(
        const Vec3f& worldPos,
        int screenWidth,
        int screenHeight
    ) const noexcept {
        const auto clipCoords = transform(worldPos);

        // Check if behind camera
        if (clipCoords.w < 0.1f) {
            return std::nullopt;
        }

        // Normalized Device Coordinates
        const Vec3f ndc{
            clipCoords.x / clipCoords.w,
            clipCoords.y / clipCoords.w,
            clipCoords.z / clipCoords.w
        };

        // Convert to screen coordinates
        const float screenX = (screenWidth / 2.0f * ndc.x) + (ndc.x + screenWidth / 2.0f);
        const float screenY = -(screenHeight / 2.0f * ndc.y) + (ndc.y + screenHeight / 2.0f);

        return Vec2f{screenX, screenY};
    }

private:
    std::array<float, 16> m_data{};
};

} // namespace bo1::math
