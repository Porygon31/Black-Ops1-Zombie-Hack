#pragma once
#include <cstdint>

// I'm still trying to find the good ViewMatrix Address

namespace Zombies
{
	uintptr_t AEntityList = 0x01BCE4D0; //0x01BCE4D0
	uintptr_t AViewMatrix = 0x00B488FC; //0x00B81058 0x007814B0 0x00B488FC

	namespace Offsets // CoD Black Ops Version 1.0
	{
		uintptr_t OLocation = 0x18; //0x18 //0x11C ??
		uintptr_t OHealth = 0x184;
		uintptr_t ODistanceBetween = 0x8C;
	}
}
