#ifndef CORE_H
#define CORE_H

#include "SDK.hpp"

namespace Core {
	inline const std::string SKELETAL_MESH = "SkeletalMesh F_SML_Starter_Epic.F_SML_Starter_Epic"; // Here to make Irma happy, IDGAF.

	inline SDK::UWorld** pWorld;
	inline SDK::ULevel* pLevel;

	inline SDK::UGameInstance* pGameInstance;

	inline SDK::TArray<SDK::ULocalPlayer*> pLocalPlayers;
	inline SDK::ULocalPlayer* pLocalPlayer;

	inline SDK::TArray<SDK::AActor*>* pActors;

	inline SDK::APlayerController* pPlayerController;
};

#endif // CORE_H
