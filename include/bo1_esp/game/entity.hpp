#pragma once

#include "../math/vector.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include <optional>

namespace bo1::game {

/**
 * @brief Represents a game entity (zombie, player, etc.)
 */
class Entity {
public:
    explicit Entity(uintptr_t address) : m_address(address) {}

    /**
     * @brief Get the entity's world position
     */
    [[nodiscard]] std::optional<math::Vec3f> getPosition() const noexcept {
        const auto x = Memory::read<float>(m_address + Offsets::Entity::POSITION_X);
        const auto y = Memory::read<float>(m_address + Offsets::Entity::POSITION_Y);
        const auto z = Memory::read<float>(m_address + Offsets::Entity::POSITION_Z);

        if (x && y && z) {
            return math::Vec3f{x.value(), y.value(), z.value()};
        }
        return std::nullopt;
    }

    /**
     * @brief Get the entity's head position
     */
    [[nodiscard]] std::optional<math::Vec3f> getHeadPosition() const noexcept {
        const auto x = Memory::read<float>(m_address + Offsets::Entity::HEAD_X);
        const auto y = Memory::read<float>(m_address + Offsets::Entity::HEAD_Y);
        const auto z = Memory::read<float>(m_address + Offsets::Entity::HEAD_Z);

        if (x && y && z) {
            return math::Vec3f{x.value(), y.value(), z.value()};
        }
        return std::nullopt;
    }

    /**
     * @brief Get the entity's health
     */
    [[nodiscard]] std::optional<int> getHealth() const noexcept {
        return Memory::read<int>(m_address + Offsets::Entity::HEALTH);
    }

    /**
     * @brief Check if the entity is alive
     */
    [[nodiscard]] bool isAlive() const noexcept {
        const auto health = getHealth();
        return health.has_value() && health.value() > 0;
    }

    /**
     * @brief Check if the entity is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return m_address != 0 && getPosition().has_value();
    }

    [[nodiscard]] uintptr_t getAddress() const noexcept { return m_address; }

private:
    uintptr_t m_address;
};

} // namespace bo1::game
