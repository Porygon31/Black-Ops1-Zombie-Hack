#pragma once
#include "DataTypes.h"


void ConvertToRange(Vec2& Point)
{
	Point.X /= 1024.0f;
	Point.X *= 2.0f;
	Point.X -= 1.0f;

	Point.Y /= 768.0f;
	Point.Y *= 2.0f;
	Point.Y -= 1.0f;
}

bool WorldToScreen(const Vec3& VecOrigin, Vec2& VecScreen, float* Matrix)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection Matrix = clipCoords
	VecScreen.X = VecOrigin.X * Matrix[0] + VecOrigin.Y * Matrix[1] + VecOrigin.Z * Matrix[2] + Matrix[3];
	VecScreen.Y = VecOrigin.X * Matrix[4] + VecOrigin.Y * Matrix[5] + VecOrigin.Z * Matrix[6] + Matrix[7];
	float W = VecOrigin.X * Matrix[12] + VecOrigin.Y * Matrix[13] + VecOrigin.Z * Matrix[14] + Matrix[15];

	if (W < 0.1f)
		return false;

	//perspective division, dividing by clip.W = NormaliZed Device Coordinates
	Vec2 NDC;
	NDC.X = VecScreen.X / W;
	NDC.Y = VecScreen.Y / W;

	VecScreen.X = (1024 / 2 * NDC.X) + (NDC.X + 1024 / 2);
	VecScreen.Y = (768 / 2 * NDC.Y) + (NDC.Y + 768 / 2);

	ConvertToRange(VecScreen);
		
	return true;
}
