#pragma once
#include <cstdint>


namespace Zombies
{
	uintptr_t AEntityList = 0x01BCE4D0; //0x01BCE4D0
	uintptr_t AViewMatrix = 0x008E870C; //0x00B81058 0x007814B0 0x00B488FC
	namespace Offsets
	{
		uintptr_t OLocation = 0x18;
		uintptr_t OHealth = 0x184;
		uintptr_t ODistanceBetween = 0x8C;
	}
}
