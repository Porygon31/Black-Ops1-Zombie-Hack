#pragma once

#include "gdi_resources.hpp"
#include "../math/vector.hpp"
#include <Windows.h>
#include <string>
#include <optional>

namespace bo1::rendering {

/**
 * @brief Modern GDI renderer with RAII resource management
 */
class Renderer {
public:
    struct Config {
        COLORREF defaultTextColor{RGB(255, 255, 255)};
        COLORREF backgroundColor{RGB(0, 0, 0)};
        int fontSize{14};
        int fontWeight{FW_NORMAL};
        std::string fontName{"Arial"};
    };

    explicit Renderer(HWND hwnd, Config config = {})
        : m_hwnd(hwnd)
        , m_config(std::move(config))
        , m_font(m_config.fontSize, m_config.fontWeight, m_config.fontName.c_str())
    {}

    /**
     * @brief Begin a new frame (acquires DC)
     */
    void beginFrame() {
        m_dc = DeviceContext(m_hwnd);
    }

    /**
     * @brief End the current frame (releases DC)
     */
    void endFrame() {
        m_dc = DeviceContext(nullptr);
    }

    /**
     * @brief Draw a filled rectangle
     */
    void drawFilledRect(int x, int y, int width, int height, COLORREF color) {
        if (!m_dc) return;

        Brush brush(color);
        RECT rect{x, y, x + width, y + height};
        ::FillRect(m_dc.get(), &rect, brush.get());
    }

    /**
     * @brief Draw a rectangle border
     */
    void drawBorder(int x, int y, int width, int height, int thickness, COLORREF color) {
        if (!m_dc) return;

        Brush brush(color);

        // Top
        RECT top{x, y, x + width, y + thickness};
        ::FillRect(m_dc.get(), &top, brush.get());

        // Left
        RECT left{x, y, x + thickness, y + height};
        ::FillRect(m_dc.get(), &left, brush.get());

        // Right
        RECT right{x + width - thickness, y, x + width, y + height};
        ::FillRect(m_dc.get(), &right, brush.get());

        // Bottom
        RECT bottom{x, y + height - thickness, x + width, y + height};
        ::FillRect(m_dc.get(), &bottom, brush.get());
    }

    /**
     * @brief Draw a line
     */
    void drawLine(int x1, int y1, int x2, int y2) {
        if (!m_dc) return;

        ::MoveToEx(m_dc.get(), x1, y1, nullptr);
        ::LineTo(m_dc.get(), x2, y2);
    }

    /**
     * @brief Draw text at the specified position
     */
    void drawText(int x, int y, const std::string& text,
                  std::optional<COLORREF> color = std::nullopt,
                  UINT alignment = TA_CENTER | TA_NOUPDATECP) {
        if (!m_dc || text.empty()) return;

        const COLORREF textColor = color.value_or(m_config.defaultTextColor);

        ::SetTextAlign(m_dc.get(), alignment);
        ::SetBkMode(m_dc.get(), TRANSPARENT);
        ::SetBkColor(m_dc.get(), m_config.backgroundColor);
        ::SetTextColor(m_dc.get(), textColor);

        ScopedSelectObject selectFont(m_dc.get(), m_font.get());
        ::TextOutA(m_dc.get(), x, y, text.c_str(), static_cast<int>(text.length()));
    }

    /**
     * @brief Draw a box with border and optional text
     */
    void drawBox(int x, int y, int width, int height, COLORREF borderColor,
                 const std::optional<std::string>& text = std::nullopt) {
        drawBorder(x, y, width, height, 2, borderColor);

        if (text.has_value()) {
            drawText(x + width / 2, y - 15, text.value());
        }
    }

    [[nodiscard]] bool isValid() const noexcept {
        return m_hwnd != nullptr && static_cast<bool>(m_dc);
    }

    [[nodiscard]] HWND getWindow() const noexcept { return m_hwnd; }
    [[nodiscard]] const Config& getConfig() const noexcept { return m_config; }

private:
    HWND m_hwnd{};
    Config m_config;
    DeviceContext m_dc{nullptr};
    Font m_font;
};

} // namespace bo1::rendering
