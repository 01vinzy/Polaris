#ifndef CORE_H
#define CORE_H

#include "SDK.hpp"

namespace Core
{
	inline SDK::UWorld** pWorld;
	inline SDK::ULevel* pLevel;
	inline SDK::TArray<SDK::AActor*>* pActors;

	inline SDK::UGameInstance* pGameInstance;

	inline SDK::TArray<SDK::ULocalPlayer*> pLocalPlayers;
	inline SDK::ULocalPlayer* pLocalPlayer;

	inline SDK::APlayerController* pPlayerController;
};

#endif // CORE_H
