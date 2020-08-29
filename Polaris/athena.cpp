#include "athena.h"
#include "console.h"
#include "util.h"
#include "playerpawn_polaris.h"

#include "SDK.hpp"

namespace polaris
{
    PlayerPawnPolaris* pPlayerPawnPolaris;

    PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;

    PVOID ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
    {
        if (pObject && pFunction)
        {
            if (pFunction->GetName().find("StartMatch") != std::string::npos)
            {
                if (!pPlayerPawnPolaris)
                {
                    // Create a new Player Pawn.
                    pPlayerPawnPolaris = new PlayerPawnPolaris;
                    pPlayerPawnPolaris->InitializeHero();
                    
                    Sleep(2000); // Wait for everything to be ready.

                    // Tell the client that we are ready to start the match, this allows the loading screen to drop.
                    static_cast<SDK::AAthena_PlayerController_C*>(Core::pPlayerController)->ServerReadyToStartMatch();

                    auto pAuthorityGameMode = static_cast<SDK::AFortGameModeAthena*>((*Core::pWorld)->AuthorityGameMode);
                    pAuthorityGameMode->StartMatch();
                }
            }

            if (pFunction->GetName().find("ServerAttemptAircraftJump") != std::string::npos)
            {
                // Use BugItGo, so that the PlayerPawn isn't messed up.
                SDK::FVector actorLocation = Core::pPlayerController->K2_GetActorLocation();
                Core::pPlayerController->CheatManager->BugItGo(actorLocation.X, actorLocation.Y, actorLocation.Z, 0, 0, 0);

                // Create a new player pawn.
                pPlayerPawnPolaris = new PlayerPawnPolaris;
                pPlayerPawnPolaris->InitializeHero();
            }

            // HACK: This will probably cause a crash, but it's worth a try.
            if (pFunction->GetName().find("StopHoldProgress") != std::string::npos)
                return NULL;
        }

        return ProcessEvent(pObject, pFunction, pParams);
    }

    DWORD UpdateThread(LPVOID lpParam)
    {
        while (1)
        {
            if (pPlayerPawnPolaris != nullptr)
            {
                // Keybind to jump (only run if not skydiving, might need to fix this more though):
                if (GetKeyState(VK_SPACE) & 0x8000 && pPlayerPawnPolaris->pPawn && !pPlayerPawnPolaris->pPawn->IsSkydiving())
                {
                    if (!pPlayerPawnPolaris->pPawn->IsJumpProvidingForce())
                        pPlayerPawnPolaris->pPawn->Jump();
                }

                // Keybind to sprint (only run if not skydiving):
                if (GetKeyState(VK_SHIFT) & 0x8000 && pPlayerPawnPolaris->pPawn && !pPlayerPawnPolaris->pPawn->IsSkydiving())
                    pPlayerPawnPolaris->pPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
                else if (pPlayerPawnPolaris->pPawn)
                    pPlayerPawnPolaris->pPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Running;

                // Keybind to equip weapon:
                if (GetKeyState(VK_END) & 0x8000 && pPlayerPawnPolaris->pPawn)
                {
                    auto pFortWeapon = static_cast<SDK::AFortWeapon*>(Util::FindActor(SDK::AFortWeapon::StaticClass()));
                    if (!pFortWeapon)
                        printf("Finding FortWeapon has failed, bailing-out immediately!\n");
                    else
                        pPlayerPawnPolaris->EquipWeapon(pFortWeapon);
                }
            }

            // Update thread only runs at 60hz, so we don't rape CPUs.
            Sleep(1000 / 60);
        }

        return NULL;
    }

    DWORD WINAPI Main(LPVOID lpParam)
    {
        Util::InitSdk();
        Util::InitCore();

        auto pProcessEventAddress = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
        if (!pProcessEventAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for ProcessEvent has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        MH_CreateHook(static_cast<LPVOID>(pProcessEventAddress), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
        MH_EnableHook(static_cast<LPVOID>(pProcessEventAddress));

        CreateThread(0, 0, UpdateThread, 0, 0, 0); // Create thread to handle input, etc...

        return FALSE;
    }

	Athena::Athena()
	{
        // Check if athena is already initialized.
        if (gpAthena)
        {
            MessageBox(0, L"Athena is already initialized.", L"Error", MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        gpAthena = this;
		Console::Log("Loading Athena");
        CreateThread(0, 0, Main, 0, 0, 0);
	}

	Athena::~Athena()
	{
		// TODO: Handle deconstruction.
	}
}
