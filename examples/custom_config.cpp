/**
 * @file custom_config.cpp
 * @brief Example showing how to customize ESP configuration
 */

#include <bo1_esp/bo1_esp.hpp>
#include <iostream>
#include <thread>

int main() {
    std::cout << "BO1 ESP Library - Custom Configuration Example\n\n";

    // Create custom configuration
    bo1::core::ESPConfig config;

    // Display settings
    config.display.showBoxes = true;
    config.display.showHealth = true;
    config.display.showDistance = false;
    config.display.showInjectionMessage = true;

    // Custom colors
    config.display.boxColor = RGB(255, 100, 0);      // Orange boxes
    config.display.textColor = RGB(0, 255, 255);     // Cyan text
    config.display.healthColor = RGB(0, 255, 0);     // Green health

    // Performance settings
    config.performance.updateInterval = std::chrono::milliseconds(5);
    config.performance.maxEntities = 32;

    // Screen settings
    config.screen.width = 1920;
    config.screen.height = 1080;

    // Create ESP manager with custom config
    bo1::core::ESPManager esp(config);

    if (!esp.initialize()) {
        std::cerr << "Error: Failed to initialize ESP.\n";
        return 1;
    }

    std::cout << "ESP initialized with custom configuration!\n";
    std::cout << "Configuration:\n";
    std::cout << "  - Box Color: Orange\n";
    std::cout << "  - Text Color: Cyan\n";
    std::cout << "  - Health Color: Green\n";
    std::cout << "  - Update Interval: 5ms\n";
    std::cout << "  - Resolution: 1920x1080\n\n";
    std::cout << "Press END key to exit...\n";

    while (esp.isRunning()) {
        esp.update();
        std::this_thread::sleep_for(config.performance.updateInterval);

        if (::GetAsyncKeyState(VK_END) & 1) {
            esp.stop();
        }
    }

    return 0;
}
