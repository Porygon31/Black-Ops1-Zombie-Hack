#pragma once

#include <Windows.h>
#include <chrono>

namespace bo1::core {

/**
 * @brief Configuration for the ESP system
 */
struct ESPConfig {
    // Display settings
    struct Display {
        bool showBoxes{true};
        bool showHealth{true};
        bool showDistance{false};
        bool showInjectionMessage{true};
        std::chrono::milliseconds injectionMessageDuration{400};

        COLORREF boxColor{RGB(158, 66, 244)};
        COLORREF textColor{RGB(255, 255, 255)};
        COLORREF healthColor{RGB(255, 255, 255)};
        COLORREF injectionMessageColor{RGB(255, 0, 0)};

        int boxThickness{2};
    } display;

    // Performance settings
    struct Performance {
        std::chrono::milliseconds updateInterval{1};
        int maxEntities{32};
    } performance;

    // Screen settings
    struct Screen {
        int width{1024};
        int height{768};
    } screen;

    // Game window settings
    struct Window {
        const char* title{"Call of Duty®: BlackOps"};
    } window;
};

} // namespace bo1::core
