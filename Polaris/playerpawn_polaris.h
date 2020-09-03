#ifndef PLAYERPAWN_POLARIS_H
#define PLAYERPAWN_POLARIS_H

#include <Windows.h>
#include <string>

#include "SDK.hpp"
#include "core.h"
#include "util.h"

namespace polaris
{
	class PlayerPawnPolaris
	{
	public:
		SDK::AFortPlayerPawnAthena* m_pPlayerPawn;

		PlayerPawnPolaris();

		void InitializeHero();
		void EquipWeapon();
	};
}

#endif