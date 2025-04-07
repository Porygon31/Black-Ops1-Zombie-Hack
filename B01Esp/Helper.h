#pragma once
#include "DataTypes.h"
#include <thread>
#include <chrono>
#include <iostream>

void ConvertToRange(Vec2& Point)
{
   /* std::cout << "[ConvertToRange] Before: X=" << Point.X << " Y=" << Point.Y << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    Point.X /= 1024.0f;
    Point.X *= 2.0f;
    Point.X -= 1.0f;

    Point.Y /= 768.0f;
    Point.Y *= 2.0f;
    Point.Y -= 1.0f;

    /*std::cout << "[ConvertToRange] After: X=" << Point.X << " Y=" << Point.Y << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/
}

bool WorldToScreen(const Vec3& VecOrigin, Vec2& VecScreen, float* Matrix)
{
    //std::cout << "[WorldToScreen] Input VecOrigin: X=" << VecOrigin.X << " Y=" << VecOrigin.Y << " Z=" << VecOrigin.Z << std::endl;

    float clipX = VecOrigin.X * Matrix[0] + VecOrigin.Y * Matrix[1] + VecOrigin.Z * Matrix[2] + Matrix[3];
    float clipY = VecOrigin.X * Matrix[4] + VecOrigin.Y * Matrix[5] + VecOrigin.Z * Matrix[6] + Matrix[7];
    float clipW = VecOrigin.X * Matrix[12] + VecOrigin.Y * Matrix[13] + VecOrigin.Z * Matrix[14] + Matrix[15];

   /* std::cout << "[WorldToScreen] clipX=" << clipX << " clipY=" << clipY << " clipW=" << clipW << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    if (clipW < 0.1f) {
        /*std::cout << "[WorldToScreen] clipW too small, skipping entity" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));*/
        return false;
    }

    float NDC_X = clipX / clipW;
    float NDC_Y = clipY / clipW;

   /* std::cout << "[WorldToScreen] NDC_X=" << NDC_X << " NDC_Y=" << NDC_Y << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    VecScreen.X = (1024.0f / 2.0f) * (NDC_X + 1.0f);
    VecScreen.Y = (768.0f / 2.0f) * (1.0f - NDC_Y);  // Inverser l'axe Y pour l'écran

   /* std::cout << "[WorldToScreen] Output VecScreen: X=" << VecScreen.X << " Y=" << VecScreen.Y << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    ConvertToRange(VecScreen);

    //std::cout << "[ConvertToRange] Normalized VecScreen: X=" << VecScreen.X << " Y=" << VecScreen.Y << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return true;
}


//#include "Utils.h"

//void ConvertToRange(Vec2& Point)
//{
//	Point.X /= 1024.0f;
//	Point.X *= 2.0f;
//	Point.X -= 1.0f;
//
//	Point.Y /= 768.0f;
//	Point.Y *= 2.0f;
//	Point.Y -= 1.0f;
//}

/**
 * World to screen method, it's convert 3d position to 2d position to draw on screen.
 * \param pos : entity vec3 (3d) position
 * \param screen : structure to store the position to draw on screen
 * \param matrix : address of the view matrix
 * \param windowWidth : width of the game window
 * \param windowHeight : height of the game window
 * \return : boolean true if success else false
 */
//bool worldtoscreen(const vec3 pos, vec2& screen, float matrix[16], const int windowwidth, const int windowheight)
//{
//	vec4 clipcoords = {};
//	clipcoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
//	clipcoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
//	clipcoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
//	clipcoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];
//
//	if (clipcoords.w < 0.1f)
//		return false;
//
//	vec3 ndc = {};
//	ndc.x = clipcoords.x / clipcoords.w;
//	ndc.y = clipcoords.y / clipcoords.w;
//	ndc.z = clipcoords.z / clipcoords.w;
//
//	screen.x = ((float)windowwidth / static_cast<float>(2) * ndc.x) + (ndc.x + (float)windowwidth / static_cast<float>(2));
//	screen.y = -((float)windowheight / static_cast<float>(2) * ndc.y) + (ndc.y + (float)windowheight / static_cast<float>(2));
//	return true;
//}