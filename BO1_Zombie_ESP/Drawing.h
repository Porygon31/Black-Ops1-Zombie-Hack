#pragma once

#include <Windows.h>
#include <string>

void DrawFilledRect(int x, int y, int w, int h);
void DrawBorderBox(int x, int y, int w, int h, int thickness);
void DrawLine(int bottomW, int bottomH, int width);
void DrawString(int x, int y, COLORREF color, const char* text);

extern HBRUSH Brush;
extern HDC hdcBlackops1;
extern HFONT Font;
