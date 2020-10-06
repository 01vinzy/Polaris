#ifndef ATHENA_H
#define ATHENA_H

#include "common.h"
#include "playerpawn_polaris.h"

namespace polaris
{
	class Athena
	{
	public:
		// Variables for the ProcessEventHook
		bool m_bIsInFrontend = true;
		bool m_bIsWaitingForLoadingScreen = false;
		bool m_bGameOver;

		// Variables for Athena_Terrain
		PlayerPawnPolaris* m_pPlayerPawnPolaris;
		SDK::UFortWeaponItemDefinition* m_pTacticalShotgunDefinition;
		SDK::UFortWeaponItemDefinition* m_pHarvestingToolDefinition;
		SDK::UFortWeaponItemDefinition* m_pJackOLauncherDefinition;
		SDK::UFortWeaponItemDefinition* m_pPumpShotgunDefinition;
		SDK::UFortWeaponItemDefinition* m_pZapatronDefinition;
		SDK::UFortWeaponItemDefinition* m_pScarDefinition;

		Athena();
	};
}

static polaris::Athena* gpAthena;

#endif // ATHENA_H
