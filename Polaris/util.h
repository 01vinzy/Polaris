#ifndef UTIL_H
#define UTIL_H

#include "SDK.hpp"

#include <Windows.h>
#include <inttypes.h>
#include <psapi.h>
#include <stdio.h>

#include <MinHook.h>
#pragma comment(lib, "libMinHook.lib")

#include "core.h"

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
    static VOID InitConsole()
    {
        AllocConsole();

        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }

    static VOID InitSdk()
    {
        auto pUWorldAddress = Util::FindPattern("\x48\x8B\x1D\x00\x00\x00\x00\x00\x00\x00\x10\x4C\x8D\x4D\x00\x4C", "xxx???????xxxx?x");
        if (!pUWorldAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for UWorld has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pUWorldOffset = *reinterpret_cast<uint32_t*>(pUWorldAddress + 3);

        Core::pWorld = reinterpret_cast<SDK::UWorld**>(pUWorldAddress + 7 + pUWorldOffset);

        auto pGObjectAddress = Util::FindPattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD6", "xxx????x????x????x????xxx");
        if (!pGObjectAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GObject has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pGObjectOffset = *reinterpret_cast<uint32_t*>(pGObjectAddress + 3);

        SDK::UObject::GObjects = reinterpret_cast<SDK::FUObjectArray*>(pGObjectAddress + 7 + pGObjectOffset);

        auto pGNameAddress = Util::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x50\xB9\x00\x00\x00\x00\x48\x89\x5C\x24", "xxx????xxxxxx????xxxx");
        if (!pGNameAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GName has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pGNameOffset = *reinterpret_cast<uint32_t*>(pGNameAddress + 3);

        SDK::FName::GNames = *reinterpret_cast<SDK::TNameEntryArray**>(pGNameAddress + 7 + pGNameOffset);
    }

    static VOID InitCore()
    {
        uintptr_t pBaseAddress = Util::BaseAddress();
        if (!pBaseAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding BaseAddress has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        if (!Core::pWorld)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"The UWorld is not initialized, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        Core::pLevel = (*Core::pWorld)->PersistentLevel;

        Core::pGameInstance = (*Core::pWorld)->OwningGameInstance;

        Core::pLocalPlayers = Core::pGameInstance->LocalPlayers;
        Core::pLocalPlayer = Core::pLocalPlayers[0];

        Core::pActors = &Core::pLevel->Actors;

        Core::pPlayerController = Core::pLocalPlayer->PlayerController;
    }

    static uintptr_t BaseAddress()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    }

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

    static SDK::AActor* FindActor(SDK::UClass* pClass, int iSkip = 0)
    {
        for (int i = 0, j = 0; i < Core::pActors->Num(); i++)
        {
            SDK::AActor* pActor = Core::pActors->operator[](i);

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

    template<typename T>
    static T* FindObject(const std::string& sClassName, const std::string& sQuery)
    {
        for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
        {
            auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
            if (pObject != nullptr && pObject->GetFullName().find("F_Med_Head1") == std::string::npos)
            {
                if (pObject->GetFullName().rfind(sClassName, 0) == 0 && pObject->GetFullName().find(sQuery) != std::string::npos)
                    return static_cast<T*>(pObject);
            }
        }

        return nullptr;
    }
};

#endif // UTIL_H
