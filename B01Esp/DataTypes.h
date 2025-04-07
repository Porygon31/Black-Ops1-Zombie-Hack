#pragma once



struct Vec2
{
	float X, Y;
};

struct Vec3
{
	float X, Y, Z;
};

struct ViewMatrix
{
	float Matrix[16];
};

//// structure to store 2d position
//typedef struct vec2
//{
//	float x, y;
//} Vec2;
//
//// structure to store 3d position
//typedef struct vec3
//{
//	float x, y, z;
//} Vec3;
//
//// structure to store 4d position 
//typedef struct vec4
//{
//	float x, y, z, w;
//} Vec4;
//
//// structure to sotre entity informations
//typedef struct entity
//{
//	Vec3* entityPos;
//	Vec3* eyePos;
//	int* iHealth;
//	int* iMaxHealth;
//} Entity;
//
//static bool WorldToScreen(Vec3 pos, Vec2& screen, float matrix[16], int windowWidth, int windowHeight);