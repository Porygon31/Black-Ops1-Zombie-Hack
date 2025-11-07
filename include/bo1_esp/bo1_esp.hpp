#pragma once

/**
 * @file bo1_esp.hpp
 * @brief Main include file for the BO1 ESP library
 *
 * Include this single header to use the entire BO1 ESP library.
 *
 * Example usage:
 * @code
 * #include <bo1_esp/bo1_esp.hpp>
 *
 * int main() {
 *     bo1::core::ESPConfig config;
 *     config.display.showBoxes = true;
 *     config.display.showHealth = true;
 *
 *     bo1::core::ESPManager esp(config);
 *     if (!esp.initialize()) {
 *         return 1;
 *     }
 *
 *     while (esp.isRunning()) {
 *         esp.update();
 *         std::this_thread::sleep_for(std::chrono::milliseconds(1));
 *     }
 *
 *     return 0;
 * }
 * @endcode
 */

// Math
#include "math/vector.hpp"
#include "math/matrix.hpp"

// Rendering
#include "rendering/gdi_resources.hpp"
#include "rendering/renderer.hpp"

// Game
#include "game/memory.hpp"
#include "game/offsets.hpp"
#include "game/entity.hpp"

// Core
#include "core/config.hpp"
#include "core/esp_manager.hpp"

namespace bo1 {
    /**
     * @brief Library version information
     */
    struct Version {
        static constexpr int MAJOR = 2;
        static constexpr int MINOR = 0;
        static constexpr int PATCH = 0;

        static constexpr const char* STRING = "2.0.0";
        static constexpr const char* NAME = "Modern C++ Refactor";
    };
}
