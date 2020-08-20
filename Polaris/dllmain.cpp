#include <windows.h>

#include "common.h"
#include "renderer.h"
#include "ui.h"
#include "frontendmanager.h"
#include "mainwindow.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include <cstdio>

#include "SDK.hpp"
#include "util.h"
#include "core.h"

SDK::APlayerPawn_Athena_C* pPlayerPawn_Athena_C;

PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;

PVOID ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams) {
    if (pObject && pFunction) {
        if (pFunction->GetName().find("ServerAttemptAircraftJump") != std::string::npos) {
            // Use BugItGo, so that the PlayerPawn isn't messed up.
            SDK::FVector actorLocation = Core::pPlayerController->K2_GetActorLocation();
            Core::pPlayerController->CheatManager->BugItGo(actorLocation.X, actorLocation.Y, actorLocation.Z, 0, 0, 0);

            // Summon a new PlayerPawn.
            std::string sClassName = "PlayerPawn_Athena_C";
            Core::pPlayerController->CheatManager->Summon(SDK::FString(std::wstring(sClassName.begin(), sClassName.end()).c_str()));

            // Find our newly summoned PlayerPawn.
            pPlayerPawn_Athena_C = static_cast<SDK::APlayerPawn_Athena_C*>(Util::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
            if (!pPlayerPawn_Athena_C)
                printf("Finding PlayerPawn_Athena_C has failed, bailing-out immediately!\n");
            else {
                // Find our SkeletalMesh in UObject cache.
                auto pSkeletalMesh = SDK::UObject::FindObject<SDK::USkeletalMesh>(Core::SKELETAL_MESH);
                if (pSkeletalMesh == nullptr)
                    printf("Finding SkeletalMesh has failed, bailing-out immediately!\n");
                else {
                    pPlayerPawn_Athena_C->Mesh->SetSkeletalMesh(pSkeletalMesh, true);

                    Core::pPlayerController->Possess(pPlayerPawn_Athena_C);
                }
            }
        }

        // HACK: This will probably cause a crash, but it's worth a try.
        if (pFunction->GetName().find("StopHoldProgress") != std::string::npos)
            return NULL;
    }

    return ProcessEvent(pObject, pFunction, pParams);
}

DWORD UpdateThread(LPVOID lpParam) {
    while (1) {
        // Keybind to jump (only run if not skydiving, might need to fix this more though):
        if (GetKeyState(VK_SPACE) & 0x8000 && pPlayerPawn_Athena_C && !pPlayerPawn_Athena_C->IsSkydiving()) {
            if (!pPlayerPawn_Athena_C->IsJumpProvidingForce())
                pPlayerPawn_Athena_C->Jump();
        }

        // Keybind to sprint (only run if not skydiving):
        if (GetKeyState(VK_SHIFT) & 0x8000 && pPlayerPawn_Athena_C && !pPlayerPawn_Athena_C->IsSkydiving())
            pPlayerPawn_Athena_C->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
        else if (pPlayerPawn_Athena_C)
            pPlayerPawn_Athena_C->CurrentMovementStyle = SDK::EFortMovementStyle::Running;

        // Keybind to equip weapon:
        if (GetKeyState(VK_END) & 0x8000 && pPlayerPawn_Athena_C) {
            auto pFortWeapon = static_cast<SDK::AFortWeapon*>(Util::FindActor(SDK::AFortWeapon::StaticClass()));
            if (!pFortWeapon)
                printf("Finding FortWeapon has failed, bailing-out immediately!\n");
            else {
                pFortWeapon->ClientGivenTo(pPlayerPawn_Athena_C);

                pPlayerPawn_Athena_C->ClientInternalEquipWeapon(pFortWeapon);
            }
        }

        // Update thread only runs at 60hz, so we don't rape CPUs.
        Sleep(1000 / 60);
    }

    return NULL;
}

DWORD WINAPI Main(LPVOID lpParam)
{
    Util::InitConsole();

    if (MH_Initialize() != MH_OK)
    {
        MessageBox(0, L"Failed to initialize MinHook.", L"Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    polaris::Console::LogRaw("Welcome to Polaris!", 11);
    new polaris::Renderer; // Initialize renderer.
    new polaris::MainWindow; // Initialize UI.

    Util::InitSdk();
    Util::InitCore();

    auto pProcessEventAddress = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
    if (!pProcessEventAddress) {
        MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for ProcessEvent has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    MH_CreateHook(static_cast<LPVOID>(pProcessEventAddress), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
    MH_EnableHook(static_cast<LPVOID>(pProcessEventAddress));

    // Find our PlayerPawn.
    pPlayerPawn_Athena_C = static_cast<SDK::APlayerPawn_Athena_C*>(Util::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
    if (!pPlayerPawn_Athena_C)
        printf("Finding PlayerPawn_Athena_C has failed, bailing-out immediately!\n");
    else {
        // Find our SkeletalMesh in UObject cache.
        auto pSkeletalMesh = SDK::UObject::FindObject<SDK::USkeletalMesh>(Core::SKELETAL_MESH);
        if (!pSkeletalMesh)
            printf("Finding SkeletalMesh has failed, bailing-out immediately!\n");
        else {
            pPlayerPawn_Athena_C->Mesh->SetSkeletalMesh(pSkeletalMesh, true);

            Util::Possess(pPlayerPawn_Athena_C); // Possess our PlayerPawn.

            CreateThread(0, 0, UpdateThread, 0, 0, 0); // Create thread to handle input, etc...

            Sleep(2000); // Wait for everything to be ready.

            // Tell the client that we are ready to start the match, this allows the loading screen to drop.
            static_cast<SDK::AAthena_PlayerController_C*>(Core::pPlayerController)->ServerReadyToStartMatch();

            auto pAuthorityGameMode = static_cast<SDK::AFortGameModeAthena*>((*Core::pWorld)->AuthorityGameMode);
            pAuthorityGameMode->StartMatch();
        }
    }

    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason,
    LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, hModule, 0, 0);
        break;

    case DLL_PROCESS_DETACH:
        // TODO: Handle deconstruction.
        break;
    }

    return TRUE;
}
