/**
 * @file basic_usage.cpp
 * @brief Basic example of using the BO1 ESP library
 */

#include <bo1_esp/bo1_esp.hpp>
#include <iostream>
#include <thread>

int main() {
    std::cout << "BO1 ESP Library v" << bo1::Version::STRING << "\n";
    std::cout << "Example: Basic Usage\n\n";

    // Create ESP manager with default configuration
    bo1::core::ESPManager esp;

    if (!esp.initialize()) {
        std::cerr << "Error: Failed to initialize ESP. Make sure the game is running.\n";
        return 1;
    }

    std::cout << "ESP initialized successfully!\n";
    std::cout << "Press END key to exit...\n\n";

    // Main loop
    while (esp.isRunning()) {
        esp.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Check for exit key
        if (::GetAsyncKeyState(VK_END) & 1) {
            esp.stop();
        }
    }

    std::cout << "ESP stopped.\n";
    return 0;
}
