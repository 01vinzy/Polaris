#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDK.hpp"

namespace globals
{
	SDK::UWorld** pWorld;
	SDK::ULevel* pLevel;

	SDK::UGameInstance* pGameInstance;

	SDK::TArray<SDK::ULocalPlayer*> pLocalPlayers;
	SDK::ULocalPlayer* pLocalPlayer;

	SDK::TArray<SDK::AActor*>* pActors;

	SDK::APlayerController* pPlayerController;
	SDK::APlayerPawn_Athena_C* pPlayerPawn_Athena_C;
}

#endif // GLOBALS_H