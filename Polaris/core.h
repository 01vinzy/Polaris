#ifndef CORE_H
#define CORE_H

#include "SDK.hpp"

namespace polaris
{
	class Globals
	{
	public:
		static inline SDK::UWorld** pWorld;
		static inline SDK::ULevel* pLevel;
		static inline SDK::TArray<SDK::AActor*>* pActors;

		static inline SDK::UGameInstance* pGameInstance;

		static inline SDK::TArray<SDK::ULocalPlayer*> pLocalPlayers;
		static inline SDK::ULocalPlayer* pLocalPlayer;

		static inline SDK::APlayerController* pPlayerController;
	};
};

#endif // CORE_H
