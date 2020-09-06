#ifndef UTIL_H
#define UTIL_H

#include <inttypes.h>
#include <Windows.h>
#include <psapi.h>
#include <stdio.h>

#include <MinHook.h>
#pragma comment(lib, "libMinHook.lib")

#include "core.h"

namespace polaris
{
    class Util
    {
    private:
        static BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
        {
            for (auto value = static_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
            {
                if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
                    return false;
            }

            return true;
        }

    public:
        // Initializes the console.
        static VOID InitConsole()
        {
            AllocConsole();

            FILE* pFile;
            freopen_s(&pFile, "CONOUT$", "w", stdout);
        }

        // Initializes the SDK.
        static VOID InitSdk()
        {
            auto pUWorldAddress = Util::FindPattern("\x48\x8B\x1D\x00\x00\x00\x00\x00\x00\x00\x10\x4C\x8D\x4D\x00\x4C", "xxx???????xxxx?x");
            if (!pUWorldAddress)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for UWorld has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            auto pUWorldOffset = *reinterpret_cast<uint32_t*>(pUWorldAddress + 3);

            polaris::Globals::pWorld = reinterpret_cast<SDK::UWorld**>(pUWorldAddress + 7 + pUWorldOffset);

            auto pGObjectAddress = Util::FindPattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD6", "xxx????x????x????x????xxx");
            if (!pGObjectAddress)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GObject has failed. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            auto pGObjectOffset = *reinterpret_cast<uint32_t*>(pGObjectAddress + 3);

            SDK::UObject::GObjects = reinterpret_cast<SDK::FUObjectArray*>(pGObjectAddress + 7 + pGObjectOffset);

            auto pGNameAddress = Util::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x50\xB9\x00\x00\x00\x00\x48\x89\x5C\x24", "xxx????xxxxxx????xxxx");
            if (!pGNameAddress)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GName has failed. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            auto pGNameOffset = *reinterpret_cast<uint32_t*>(pGNameAddress + 3);

            SDK::FName::GNames = *reinterpret_cast<SDK::TNameEntryArray**>(pGNameAddress + 7 + pGNameOffset);
        }

        // Initializes Polaris Core.
        static VOID InitCore()
        {
            uintptr_t pBaseAddress = Util::BaseAddress();
            if (!pBaseAddress)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"Finding BaseAddress has failed. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            if (!polaris::Globals::pWorld)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"The UWorld is not initialized. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            polaris::Globals::pLevel = (*polaris::Globals::pWorld)->PersistentLevel;

            polaris::Globals::pGameInstance = (*polaris::Globals::pWorld)->OwningGameInstance;

            polaris::Globals::pLocalPlayers = polaris::Globals::pGameInstance->LocalPlayers;
            polaris::Globals::pLocalPlayer = polaris::Globals::pLocalPlayers[0];

            polaris::Globals::pActors = &polaris::Globals::pLevel->Actors;

            polaris::Globals::pPlayerController = polaris::Globals::pLocalPlayer->PlayerController;
        }

        // Class patches.
        static VOID InitAddressPatches()
        {
            // Item ownership check patching - allows weapons and other GameplayAbilites to properly function.
            auto pAbilityPatchAddress = polaris::Util::FindPattern
            (
                "\xC0\x0F\x84\x3C\x02\x00\x00\x0F\x2F\xF7\x0F\x86\xF5\x00\x00\x00",
                "xxxxxxxxxxxxxxxx"
            );
            if (!pAbilityPatchAddress)
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"An error has occured while initalizing Polaris. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            DWORD curProtection;
            VirtualProtect(pAbilityPatchAddress, 16, PAGE_EXECUTE_READWRITE, &curProtection);

            reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[2] = 0x85;
            reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[11] = 0x8D;

            DWORD temp;
            VirtualProtect(pAbilityPatchAddress, 16, curProtection, &temp);
        }

        // Get the Base Address.
        static uintptr_t BaseAddress()
        {
            return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
        }

        // Find a pattern.
        static PBYTE FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
        {
            dwSize -= static_cast<DWORD>(strlen(lpMask));

            for (auto i = 0UL; i < dwSize; ++i)
            {
                auto pAddress = static_cast<PBYTE>(pBase) + i;

                if (Util::MaskCompare(pAddress, lpPattern, lpMask))
                    return pAddress;
            }

            return NULL;
        }
        static PBYTE FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
        {
            MODULEINFO info = { 0 };

            GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

            return Util::FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
        }

        // Find an actor in the current umap.
        static SDK::AActor* FindActor(SDK::UClass* pClass, int iSkip = 0)
        {
            for (int i = 0, j = 0; i < polaris::Globals::pActors->Num(); i++)
            {
                SDK::AActor* pActor = polaris::Globals::pActors->operator[](i);

                if (pActor != nullptr)
                {
                    if (pActor->IsA(pClass))
                    {
                        if (j >= iSkip)
                            return pActor;
                        else
                        {
                            j++;
                            continue;
                        }
                    }
                }
            }

            return nullptr;
        }
    };
}

#endif // UTIL_H
