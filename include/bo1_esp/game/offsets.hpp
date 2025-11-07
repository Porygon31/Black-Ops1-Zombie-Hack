#pragma once

#include <cstdint>

namespace bo1::game {

/**
 * @brief Game-specific memory offsets
 * These offsets are specific to Black Ops 1 Zombies
 */
struct Offsets {
    // Entity list
    static constexpr uintptr_t ENTITY_LIST = 0x01BCE4D0;
    static constexpr uintptr_t ENTITY_SIZE = 0x8C;
    static constexpr int MAX_ENTITIES = 32;

    // View matrix
    static constexpr uintptr_t VIEW_MATRIX = 0x00B81060;

    // Entity offsets
    struct Entity {
        static constexpr uintptr_t POSITION_X = 0x18;
        static constexpr uintptr_t POSITION_Y = 0x1C;
        static constexpr uintptr_t POSITION_Z = 0x20;

        static constexpr uintptr_t HEAD_X = 0x110;
        static constexpr uintptr_t HEAD_Y = 0x114;
        static constexpr uintptr_t HEAD_Z = 0x118;

        static constexpr uintptr_t HEALTH = 0x184;
    };

    // Screen dimensions (configurable)
    static constexpr int SCREEN_WIDTH = 1024;
    static constexpr int SCREEN_HEIGHT = 768;
};

} // namespace bo1::game
