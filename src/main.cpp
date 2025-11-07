/**
 * @file main.cpp
 * @brief Modern C++ implementation of the ESP main thread
 */

#include <bo1_esp/bo1_esp.hpp>
#include <Windows.h>
#include <thread>
#include <chrono>

using namespace bo1;

/**
 * @brief Main ESP thread function
 *
 * This function initializes and runs the ESP system in a loop.
 * It demonstrates the modern C++ API with proper RAII and error handling.
 *
 * @param lpParam Unused parameter (required by Windows API)
 * @return DWORD Always returns 0
 */
DWORD WINAPI mainThread(LPVOID lpParam) {
    // Configure ESP settings
    core::ESPConfig config;
    config.display.showBoxes = true;
    config.display.showHealth = true;
    config.display.showInjectionMessage = true;
    config.display.boxColor = RGB(158, 66, 244);
    config.display.textColor = RGB(255, 255, 255);
    config.performance.updateInterval = std::chrono::milliseconds(1);

    // Create and initialize ESP manager
    core::ESPManager espManager(config);

    if (!espManager.initialize()) {
        // Failed to find game window
        ::MessageBoxA(nullptr,
                      "Failed to initialize ESP: Game window not found",
                      "BO1 ESP Error",
                      MB_OK | MB_ICONERROR);
        return 1;
    }

    // Main loop
    while (espManager.isRunning()) {
        espManager.update();

        // Sleep to avoid consuming too much CPU
        std::this_thread::sleep_for(config.performance.updateInterval);

        // Optional: Check for exit key (e.g., END key to unload)
        if (::GetAsyncKeyState(VK_END) & 1) {
            espManager.stop();
            break;
        }
    }

    return 0;
}

/**
 * @brief DLL entry point
 *
 * Called when the DLL is loaded into the game process.
 * Creates the main ESP thread and detaches it.
 *
 * @param hModule Handle to the DLL module
 * @param dwReason Reason for calling the function
 * @param lpReserved Reserved parameter
 * @return BOOL TRUE if initialization succeeded
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        // Disable thread attach/detach notifications
        ::DisableThreadLibraryCalls(hModule);

        // Create the main ESP thread
        const HANDLE hThread = ::CreateThread(
            nullptr,                    // Default security attributes
            0,                          // Default stack size
            mainThread,                 // Thread function
            nullptr,                    // No parameters
            0,                          // Start immediately
            nullptr                     // Don't need thread ID
        );

        if (hThread) {
            // Close handle since we don't need to wait for it
            ::CloseHandle(hThread);
        }
    }

    return TRUE;
}
