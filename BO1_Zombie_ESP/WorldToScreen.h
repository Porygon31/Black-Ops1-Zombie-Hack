#pragma once
#include "Vectors.h"

bool WorldToScreen(Vec3 pos, Vec2& screen, float matrix[16], int windowWidth, int windowHeight);
