#include "athena.h"
#include "console.h"
#include "util.h"

#include "SDK.hpp"

namespace polaris
{
    // Listens to functions called in game, use this to hook code to functions in question.
    PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;
    PVOID ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
    {
        if (pObject && pFunction)
        {
            if (pFunction->GetName().find("StartMatch") != std::string::npos)
            {
                if (!gpAthena->m_pPlayerPawnPolaris)
                {
                    // Create a new Player Pawn.
                    gpAthena->m_pPlayerPawnPolaris = new PlayerPawnPolaris;
                    gpAthena->m_pPlayerPawnPolaris->InitializeHero();

                    // Tell the client that we are ready to start the match, this allows the loading screen to drop.
                    static_cast<SDK::AAthena_PlayerController_C*>(Globals::pPlayerController)->ServerReadyToStartMatch();
                    static_cast<SDK::AGameMode*>((*Globals::pWorld)->AuthorityGameMode)->StartMatch();
                }
            }

            if (pFunction->GetName().find("ServerAttemptAircraftJump") != std::string::npos)
            {
                // Reset the pawn rotation, due to weird summon properties.
                SDK::AFortPlayerPawnAthena* playerPawn = gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn;
                SDK::FRotator actorRotation = gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->K2_GetActorRotation();

                actorRotation.Pitch = 0;
                actorRotation.Roll = 0;
                playerPawn->K2_SetActorLocationAndRotation(playerPawn->K2_GetActorLocation(), actorRotation, false, true, new SDK::FHitResult());

                // Create a new player pawn.
                gpAthena->m_pPlayerPawnPolaris = new PlayerPawnPolaris;
                gpAthena->m_pPlayerPawnPolaris->InitializeHero();

                return NULL;
            }
        }

        return ProcessEvent(pObject, pFunction, pParams);
    }

    // Update loop, runs at 60hz, 1 tick every 16ms.
    DWORD UpdateThread(LPVOID lpParam)
    {
        while (1)
        {
            if (gpAthena->m_pPlayerPawnPolaris != nullptr)
            {
                // Keybind to jump (only run if not skydiving, might need to fix this more though):
                if (GetKeyState(VK_SPACE) & 0x8000 && gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn && !gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->IsSkydiving())
                {
                    if (!gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->IsJumpProvidingForce())
                        gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->Jump();
                }

                // Keybind to sprint (only run if not skydiving):
                if (static_cast<SDK::AAthena_PlayerController_C*>(Globals::pPlayerController)->bWantsToSprint && gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn)
                    gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
                else if (gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn)
                    gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Running;
            }

            // Update thread only runs at 60hz, so we don't rape CPUs.
            Sleep(1000 / 60);
        }

        return NULL;
    }

    // Initializes the ProcessEventHook and starts the Update thread.
    DWORD WINAPI LoaderThread(LPVOID lpParam)
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

        CreateThread(0, 0, LoaderThread, 0, 0, 0);
	}
}
