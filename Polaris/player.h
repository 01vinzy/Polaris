#ifndef PLAYER_H
#define PLAYER_H

#include <Windows.h>
#include <string>

#include "SDK.hpp"
#include "globals.h"

namespace polaris
{
	class Player
	{
	public:
		SDK::AFortPlayerPawnAthena* m_pPlayerPawn;

		Player();

		void InitializeHero();
		void EquipWeapon(const char* cItemDef, int guid = 0);
	};
}

#endif // PLAYER_H