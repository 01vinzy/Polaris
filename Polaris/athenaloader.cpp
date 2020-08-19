/*
 * [irma] This class is unfinished, but I plan to have a system that loads the main Polaris behaviour when the player readies up
*/

#include "athenaloader.h";
#include "common.h";

namespace polaris
{
	AthenaLoader::AthenaLoader()
	{
		Console::Log("Starting Athena Thread");

		Utils::InitializeSDK();
		Utils::InitializeHook();

		CreateThread(0, 0, AthenaMain, 0, 0, 0);
	}

	AthenaLoader::~AthenaLoader()
	{
		//TODO: Handle deconstruction.
	}

	DWORD WINAPI AthenaMain(LPVOID lpParam)
	{
		Console::Log("Loading Athena");

		// Find a Player pawn.
		globals::pPlayerPawn_Athena_C = static_cast<SDK::APlayerPawn_Athena_C*>(Utils::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
		if (!globals::pPlayerPawn_Athena_C)
		{
			MessageBox(0, L"Failed to find Player pawn.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}

		// Find a SkeletalMesh in the UObject cache.
		SDK::USkeletalMesh* pSkeletalMesh = SDK::UObject::FindObject<SDK::USkeletalMesh>("SkeletalMesh F_SML_Starter_Epic.F_SML_Starter_Epic");
		if (!pSkeletalMesh)
		{
			MessageBox(0, L"Failed to find SkeletalMesh.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}

		// Set our player's skeletal mesh and possess it.
		globals::pPlayerPawn_Athena_C->Mesh->SetSkeletalMesh(pSkeletalMesh, true);
		Utils::Possess(globals::pPlayerPawn_Athena_C);

		// Tell the client that the match is ready to start, this drops the loading screen.
		static_cast<SDK::AAthena_PlayerController_C*>(globals::pPlayerController)->ServerReadyToStartMatch();

		SDK::AFortGameModeAthena* pAuthorityGameMode = static_cast<SDK::AFortGameModeAthena*>((*globals::pWorld)->AuthorityGameMode);
		pAuthorityGameMode->StartMatch();
	}
}
