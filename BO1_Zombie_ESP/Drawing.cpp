#include "Drawing.h"


void DrawFilledRect(int x, int y, int w, int h)
{
    RECT rect = { x, y, x + w, y + h };
    FillRect(hdcBlackops1, &rect, Brush);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
    DrawFilledRect(x, y, w, thickness);
    DrawFilledRect(x, y, thickness, h);
    DrawFilledRect((x + w), y, thickness, h);
    DrawFilledRect(x, y + h, w + thickness, thickness);
}

void DrawLine(int bottomW, int bottomH, int width)
{
    MoveToEx(hdcBlackops1, bottomW, bottomH, NULL);
    LineTo(hdcBlackops1, bottomW + width, bottomH);
}

void DrawString(int x, int y, COLORREF color, const char* text)
{
    SetTextAlign(hdcBlackops1, TA_CENTER | TA_NOUPDATECP);
    SetBkColor(hdcBlackops1, RGB(0, 0, 0));
    SetBkMode(hdcBlackops1, TRANSPARENT);
    SetTextColor(hdcBlackops1, color);
    SelectObject(hdcBlackops1, Font);
    TextOutA(hdcBlackops1, x, y, text, strlen(text));
    DeleteObject(Font);
}
