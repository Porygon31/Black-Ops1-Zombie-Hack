#pragma once

#include <Windows.h>
#include <cstdint>
#include <optional>
#include <type_traits>

namespace bo1::game {

/**
 * @brief Safe memory reading utilities
 */
class Memory {
public:
    /**
     * @brief Safely read a value from memory
     * @tparam T Type to read (must be trivially copyable)
     * @param address Memory address to read from
     * @return Value if read was successful, std::nullopt otherwise
     */
    template<typename T>
    [[nodiscard]] static std::optional<T> read(uintptr_t address) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "T must be trivially copyable");

        if (!address || ::IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(T))) {
            return std::nullopt;
        }

        try {
            return *reinterpret_cast<T*>(address);
        } catch (...) {
            return std::nullopt;
        }
    }

    /**
     * @brief Read multiple values into an array
     */
    template<typename T, size_t N>
    [[nodiscard]] static bool readArray(uintptr_t address, T (&buffer)[N]) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "T must be trivially copyable");

        if (!address || ::IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(T) * N)) {
            return false;
        }

        try {
            std::memcpy(buffer, reinterpret_cast<void*>(address), sizeof(T) * N);
            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * @brief Read a pointer and dereference it
     */
    template<typename T>
    [[nodiscard]] static std::optional<T> readPointer(uintptr_t baseAddress, uintptr_t offset = 0) noexcept {
        const auto ptr = read<uintptr_t>(baseAddress);
        if (!ptr.has_value() || ptr.value() == 0) {
            return std::nullopt;
        }
        return read<T>(ptr.value() + offset);
    }
};

} // namespace bo1::game
