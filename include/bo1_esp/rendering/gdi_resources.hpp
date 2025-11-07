#pragma once

#include <Windows.h>
#include <memory>

namespace bo1::rendering {

/**
 * @brief RAII wrapper for GDI device context (HDC)
 */
class DeviceContext {
public:
    DeviceContext(HWND hwnd)
        : m_hwnd(hwnd)
        , m_hdc(::GetDC(hwnd))
    {}

    ~DeviceContext() {
        if (m_hdc && m_hwnd) {
            ::ReleaseDC(m_hwnd, m_hdc);
        }
    }

    // Delete copy operations
    DeviceContext(const DeviceContext&) = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;

    // Allow move operations
    DeviceContext(DeviceContext&& other) noexcept
        : m_hwnd(other.m_hwnd)
        , m_hdc(other.m_hdc)
    {
        other.m_hdc = nullptr;
        other.m_hwnd = nullptr;
    }

    DeviceContext& operator=(DeviceContext&& other) noexcept {
        if (this != &other) {
            if (m_hdc && m_hwnd) {
                ::ReleaseDC(m_hwnd, m_hdc);
            }
            m_hdc = other.m_hdc;
            m_hwnd = other.m_hwnd;
            other.m_hdc = nullptr;
            other.m_hwnd = nullptr;
        }
        return *this;
    }

    [[nodiscard]] HDC get() const noexcept { return m_hdc; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_hdc != nullptr; }
    [[nodiscard]] operator HDC() const noexcept { return m_hdc; }

private:
    HWND m_hwnd{};
    HDC m_hdc{};
};

/**
 * @brief RAII wrapper for GDI brush
 */
class Brush {
public:
    explicit Brush(COLORREF color)
        : m_brush(::CreateSolidBrush(color))
    {}

    ~Brush() {
        if (m_brush) {
            ::DeleteObject(m_brush);
        }
    }

    // Delete copy operations
    Brush(const Brush&) = delete;
    Brush& operator=(const Brush&) = delete;

    // Allow move operations
    Brush(Brush&& other) noexcept : m_brush(other.m_brush) {
        other.m_brush = nullptr;
    }

    Brush& operator=(Brush&& other) noexcept {
        if (this != &other) {
            if (m_brush) {
                ::DeleteObject(m_brush);
            }
            m_brush = other.m_brush;
            other.m_brush = nullptr;
        }
        return *this;
    }

    [[nodiscard]] HBRUSH get() const noexcept { return m_brush; }
    [[nodiscard]] operator HBRUSH() const noexcept { return m_brush; }

private:
    HBRUSH m_brush{};
};

/**
 * @brief RAII wrapper for GDI font
 */
class Font {
public:
    Font(int height, int weight, const char* faceName) {
        m_font = ::CreateFontA(
            height, 0, 0, 0, weight,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            faceName
        );
    }

    ~Font() {
        if (m_font) {
            ::DeleteObject(m_font);
        }
    }

    // Delete copy operations
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    // Allow move operations
    Font(Font&& other) noexcept : m_font(other.m_font) {
        other.m_font = nullptr;
    }

    Font& operator=(Font&& other) noexcept {
        if (this != &other) {
            if (m_font) {
                ::DeleteObject(m_font);
            }
            m_font = other.m_font;
            other.m_font = nullptr;
        }
        return *this;
    }

    [[nodiscard]] HFONT get() const noexcept { return m_font; }
    [[nodiscard]] operator HFONT() const noexcept { return m_font; }

private:
    HFONT m_font{};
};

/**
 * @brief RAII wrapper for selecting GDI objects into a DC
 */
class ScopedSelectObject {
public:
    ScopedSelectObject(HDC hdc, HGDIOBJ obj)
        : m_hdc(hdc)
        , m_oldObj(::SelectObject(hdc, obj))
    {}

    ~ScopedSelectObject() {
        if (m_oldObj && m_hdc) {
            ::SelectObject(m_hdc, m_oldObj);
        }
    }

    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;

private:
    HDC m_hdc{};
    HGDIOBJ m_oldObj{};
};

} // namespace bo1::rendering
