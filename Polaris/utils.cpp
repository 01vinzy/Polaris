#include "utils.h"

namespace polaris
{
    /* ImGui utils*/
	void Utils::OpenTwitter()
	{
		std::string twitterUrl = "https://twitter.com/ProjectPolaris_";
		std::cin >> twitterUrl;

		system(std::string("start " + twitterUrl).c_str());
	}

    /* SDK utils */
    DWORD Utils::PossessThread(LPVOID lpParam)
    {
        globals::pPlayerController->Possess(static_cast<SDK::APawn*>(lpParam));

        return NULL;
    }
    BOOL Utils::MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
    {
        for (auto value = static_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
        {
            if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
                return false;
        }

        return true;
    }

	void Utils::InitializeSDK()
	{
        auto pUWorldAddress = Utils::FindPattern("\x48\x8B\x1D\x00\x00\x00\x00\x00\x00\x00\x10\x4C\x8D\x4D\x00\x4C", "xxx???????xxxx?x");
        if (!pUWorldAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for UWorld has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pUWorldOffset = *reinterpret_cast<uint32_t*>(pUWorldAddress + 3);

        globals::pWorld = reinterpret_cast<SDK::UWorld**>(pUWorldAddress + 7 + pUWorldOffset);

        auto pGObjectAddress = Utils::FindPattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD6", "xxx????x????x????x????xxx");
        if (!pGObjectAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GObject has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pGObjectOffset = *reinterpret_cast<uint32_t*>(pGObjectAddress + 3);

        SDK::UObject::GObjects = reinterpret_cast<SDK::FUObjectArray*>(pGObjectAddress + 7 + pGObjectOffset);

        auto pGNameAddress = Utils::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x50\xB9\x00\x00\x00\x00\x48\x89\x5C\x24", "xxx????xxxxxx????xxxx");
        if (!pGNameAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for GName has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        auto pGNameOffset = *reinterpret_cast<uint32_t*>(pGNameAddress + 3);

        SDK::FName::GNames = *reinterpret_cast<SDK::TNameEntryArray**>(pGNameAddress + 7 + pGNameOffset);
	}
    void Utils::InitializeHook()
    {
        if (MH_Initialize() != MH_STATUS::MH_OK)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Initializing MinHook has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        uintptr_t pBaseAddress = Utils::BaseAddress();
        if (!pBaseAddress)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"Finding BaseAddress has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        if (!globals::pWorld)
        {
            MessageBox(NULL, static_cast<LPCWSTR>(L"The UWorld is not initialized, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }

        globals::pLevel = (*globals::pWorld)->PersistentLevel;

        globals::pGameInstance = (*globals::pWorld)->OwningGameInstance;

        globals::pLocalPlayers = globals::pGameInstance->LocalPlayers;
        globals::pLocalPlayer = globals::pLocalPlayers[0];

        globals::pActors = &globals::pLevel->Actors;

        globals::pPlayerController = globals::pLocalPlayer->PlayerController;
    }
    void Utils::Possess(SDK::AActor* pPawn)
    {
        CreateThread(0, 0, Utils::PossessThread, pPawn, 0, 0);
    }
    uintptr_t Utils::BaseAddress()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    }
    PBYTE Utils::FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
    {
        dwSize -= static_cast<DWORD>(strlen(lpMask));

        for (auto index = 0UL; index < dwSize; ++index)
        {
            auto pAddress = static_cast<PBYTE>(pBase) + index;

            if (Utils::MaskCompare(pAddress, lpPattern, lpMask))
                return pAddress;
        }

        return NULL;
    }
    SDK::AActor* Utils::FindActor(SDK::UClass* pClass, int iSkip = 0)
    {
        for (int i = 0, j = 0; i < globals::pActors->Num(); i++)
        {
            SDK::AActor* pActor = globals::pActors->operator[](i);

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
    SDK::FString Utils::ToFString(std::string string)
    {
        return SDK::FString(std::wstring(string.begin(), string.end()).c_str());
    }
}
