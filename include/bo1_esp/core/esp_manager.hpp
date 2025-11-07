#pragma once

#include "config.hpp"
#include "../game/entity.hpp"
#include "../game/memory.hpp"
#include "../game/offsets.hpp"
#include "../math/matrix.hpp"
#include "../rendering/renderer.hpp"

#include <Windows.h>
#include <memory>
#include <vector>
#include <chrono>
#include <string>

namespace bo1::core {

/**
 * @brief Main ESP manager class - orchestrates all ESP functionality
 */
class ESPManager {
public:
    explicit ESPManager(ESPConfig config = {})
        : m_config(std::move(config))
        , m_injectionTime(std::chrono::steady_clock::now())
    {}

    /**
     * @brief Initialize the ESP system
     * @return true if initialization was successful
     */
    bool initialize() {
        m_hwnd = ::FindWindowA(nullptr, m_config.window.title);
        if (!m_hwnd) {
            return false;
        }

        // Initialize renderer
        rendering::Renderer::Config rendererConfig;
        rendererConfig.defaultTextColor = m_config.display.textColor;
        m_renderer = std::make_unique<rendering::Renderer>(m_hwnd, rendererConfig);

        return true;
    }

    /**
     * @brief Main update loop - call this repeatedly
     */
    void update() {
        if (!m_renderer) {
            return;
        }

        m_renderer->beginFrame();

        // Show injection message if within duration
        if (m_config.display.showInjectionMessage) {
            const auto elapsed = std::chrono::steady_clock::now() - m_injectionTime;
            if (elapsed < m_config.display.injectionMessageDuration) {
                m_renderer->drawText(100, 20, "DLL Injected!",
                                   m_config.display.injectionMessageColor);
            }
        }

        // Read view matrix
        if (!updateViewMatrix()) {
            m_renderer->endFrame();
            return;
        }

        // Process entities
        processEntities();

        m_renderer->endFrame();
    }

    /**
     * @brief Check if the manager is running
     */
    [[nodiscard]] bool isRunning() const noexcept {
        return m_running;
    }

    /**
     * @brief Stop the ESP manager
     */
    void stop() noexcept {
        m_running = false;
    }

    /**
     * @brief Get the current configuration
     */
    [[nodiscard]] const ESPConfig& getConfig() const noexcept {
        return m_config;
    }

    /**
     * @brief Update configuration at runtime
     */
    void setConfig(ESPConfig config) {
        m_config = std::move(config);
    }

private:
    /**
     * @brief Update the view matrix from game memory
     */
    bool updateViewMatrix() {
        float matrixData[16];
        if (!game::Memory::readArray(game::Offsets::VIEW_MATRIX, matrixData)) {
            return false;
        }
        m_viewMatrix = math::Matrix4x4(matrixData);
        return true;
    }

    /**
     * @brief Process and render all entities
     */
    void processEntities() {
        for (int i = 0; i < m_config.performance.maxEntities; ++i) {
            const uintptr_t entityListAddress = game::Offsets::ENTITY_LIST +
                                                 i * game::Offsets::ENTITY_SIZE;

            const auto entityAddress = game::Memory::read<uintptr_t>(entityListAddress);
            if (!entityAddress.has_value() || entityAddress.value() == 0) {
                continue;
            }

            game::Entity entity(entityAddress.value());
            renderEntity(entity);
        }
    }

    /**
     * @brief Render a single entity
     */
    void renderEntity(const game::Entity& entity) {
        if (!entity.isAlive()) {
            return;
        }

        const auto position = entity.getPosition();
        const auto headPosition = entity.getHeadPosition();

        if (!position.has_value() || !headPosition.has_value()) {
            return;
        }

        // Project to screen
        const auto screenPos = m_viewMatrix.worldToScreen(
            position.value(),
            m_config.screen.width,
            m_config.screen.height
        );

        const auto screenHead = m_viewMatrix.worldToScreen(
            headPosition.value(),
            m_config.screen.width,
            m_config.screen.height
        );

        if (!screenPos.has_value() || !screenHead.has_value()) {
            return;
        }

        // Calculate box dimensions
        const float height = screenPos->y - screenHead->y;
        const float width = height / 2.0f;
        const float centerOffset = -width / 2.0f;

        const int boxX = static_cast<int>(screenPos->x + centerOffset);
        const int boxY = static_cast<int>(screenHead->y);
        const int boxWidth = static_cast<int>(width);
        const int boxHeight = static_cast<int>(height - 5.0f);

        // Draw box
        if (m_config.display.showBoxes) {
            m_renderer->drawBorder(
                boxX, boxY, boxWidth, boxHeight,
                m_config.display.boxThickness,
                m_config.display.boxColor
            );
        }

        // Draw health
        if (m_config.display.showHealth) {
            const auto health = entity.getHealth();
            if (health.has_value()) {
                const std::string healthText = "Health: " + std::to_string(health.value());
                m_renderer->drawText(
                    static_cast<int>(screenPos->x),
                    static_cast<int>(screenPos->y),
                    healthText,
                    m_config.display.healthColor
                );
            }
        }
    }

    ESPConfig m_config;
    HWND m_hwnd{};
    std::unique_ptr<rendering::Renderer> m_renderer;
    math::Matrix4x4 m_viewMatrix;
    std::chrono::steady_clock::time_point m_injectionTime;
    bool m_running{true};
};

} // namespace bo1::core
