#include <windows.h>

#include "common.h"
#include "renderer.h"
#include "window.h"
#include "frontendmanager.h"
#include "athena.h"
#include "mainwindow.h"
#include "watermark.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include <cstdio>

#include "SDK.hpp"
#include "util.h"
#include "core.h"
#include "polarisflags.h"

BOOL (*CreateNamedNetDriver)(SDK::UEngine*, SDK::UWorld*, SDK::FName, SDK::FName) = nullptr;
SDK::UNetDriver* (*FindNamedNetDriver)(SDK::UEngine*, SDK::UWorld*, SDK::FName) = nullptr;
VOID (*SetWorld)(SDK::UNetDriver*, SDK::UWorld*) = nullptr;
BOOL (*InitListen)(SDK::UNetDriver*, PVOID, SDK::FURL&, BOOL, SDK::FString&) = nullptr;
SDK::FLevelCollection* (*FindCollectionByType)(SDK::UWorld*, INT) = nullptr;
VOID (*Tick)(SDK::UWorld*, unsigned int, float) = nullptr;
VOID(*Broadcast0)(__int64 a1, __int64 a2, __int64 a3, __int64 a4) = nullptr;
VOID(*Broadcast1)(__int64 a1, __int64 a2, __int64 a3, __int64 a4) = nullptr;
VOID(*TickDispatch)(SDK::UIpNetDriver*, float) = nullptr;

SDK::UIpNetDriver* driver = nullptr;

bool bIsTick = false;
bool bRunOnce = false;

float mydelta = 0;

VOID TickHook(SDK::UWorld* world, unsigned int ticktype, float delta)
{
    bIsTick = true;

    mydelta = delta;

    Tick(world, ticktype, delta);

    bIsTick = false;
}

VOID Broadcast0Hook(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
{
    if (!bIsTick && bRunOnce)
    {
        Broadcast0(a1, a2, a3, a4);

        bRunOnce = false;
    }
    else
        bRunOnce = true;
    //else
    {
        /*if (driver)
        {
            TickDispatch(driver, mydelta);

            //bRunOnce = true;

            //SDK::GetVFunction<void(*)(SDK::UNetDriver*, float)>(driver, 664)(driver, mydelta);
        }*/
    }
}

VOID Broadcast1Hook(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
{
    if (!bIsTick)
        Broadcast1(a1, a2, a3, a4);
    else
    {
        //if (driver)
            //SDK::GetVFunction<void(*)(SDK::UNetDriver*, float)>(driver, 672)(driver, mydelta);
    }
}

BOOL Listen()
{
    if ((*Core::pWorld)->NetDriver)
        return FALSE;

    auto GEngine = SDK::UObject::FindObject<SDK::UFortEngine>("FortEngine Transient.FortEngine_1");
    if (CreateNamedNetDriver(GEngine, (*Core::pWorld), SDK::FName("GameNetDriver"), SDK::FName("GameNetDriver")))
    {
        (*Core::pWorld)->NetDriver = FindNamedNetDriver(GEngine, (*Core::pWorld), SDK::FName("GameNetDriver"));
        SetWorld((*Core::pWorld)->NetDriver, (*Core::pWorld));
        SDK::FLevelCollection* SourceCollection = FindCollectionByType((*Core::pWorld), 0);
        if (SourceCollection)
            SourceCollection->NetDriver = (*Core::pWorld)->NetDriver;
        SDK::FLevelCollection* StaticCollection = FindCollectionByType((*Core::pWorld), 2);
		if (StaticCollection)
			StaticCollection->NetDriver = (*Core::pWorld)->NetDriver;
    }

    if (!(*Core::pWorld)->NetDriver)
        return FALSE;

    driver = reinterpret_cast<SDK::UIpNetDriver*>((*Core::pWorld)->NetDriver);

    printf("%s\n", (*Core::pWorld)->NetDriver->GetFullName().c_str());

    SDK::FString Error;
    if (!InitListen((*Core::pWorld)->NetDriver, (*Core::pWorld), Core::pLevel->URL, FALSE, Error))
    {
        SetWorld((*Core::pWorld)->NetDriver, NULL);
        (*Core::pWorld)->NetDriver = NULL;
        SDK::FLevelCollection* SourceCollection = FindCollectionByType((*Core::pWorld), 0);
        if (SourceCollection)
            SourceCollection->NetDriver = NULL;
        SDK::FLevelCollection* StaticCollection = FindCollectionByType((*Core::pWorld), 2);
        if (StaticCollection)
            StaticCollection->NetDriver = NULL;
        return FALSE;
    }
     
    static bool LanPlay = /*FParse::Param(FCommandLine::Get(), TEXT("lanplay"))*/FALSE;
    if (!LanPlay && ((*Core::pWorld)->NetDriver->MaxInternetClientRate < (*Core::pWorld)->NetDriver->MaxClientRate) && ((*Core::pWorld)->NetDriver->MaxInternetClientRate > 2500))
        (*Core::pWorld)->NetDriver->MaxClientRate = (*Core::pWorld)->NetDriver->MaxInternetClientRate;

    *(FLOAT*)((*Core::pWorld) + 0x960) = (*Core::pWorld)->NetDriver->ServerTravelPause; //lazy

    return TRUE;
}

DWORD WINAPI Main(LPVOID lpParam)
{
    Util::InitConsole();

    if (MH_Initialize() != MH_OK)
    {
        MessageBox(0, L"Failed to initialize MinHook.", L"Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    auto pCreateNamedNetDriverOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x84\xC0\x75\x43\x80\x3D\x00\x00\x00\x00\x00", "x????xxxxxx?????");
    auto pCreateNamedNetDriverAddress = pCreateNamedNetDriverOffset + 5 + *reinterpret_cast<int32_t*>(pCreateNamedNetDriverOffset + 1);

    CreateNamedNetDriver = reinterpret_cast<decltype(CreateNamedNetDriver)>(pCreateNamedNetDriverAddress);

    auto pFindNamedNetDriverOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x49\x8B\xD6\x48\x85\xC0", "x????xxxxxx");
    auto pFindNamedNetDriverAddress = pFindNamedNetDriverOffset + 5 + *reinterpret_cast<int32_t*>(pFindNamedNetDriverOffset + 1);

    FindNamedNetDriver = reinterpret_cast<decltype(FindNamedNetDriver)>(pFindNamedNetDriverAddress);

    auto pSetWorldOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x4C\x89\x75\xBF\x48\x8D\x57\x28", "x????xxxxxxxx");
    auto pSetWorldAddress = pSetWorldOffset + 5 + *reinterpret_cast<int32_t*>(pSetWorldOffset + 1);

    SetWorld = reinterpret_cast<decltype(SetWorld)>(pSetWorldAddress);

    auto pInitListenAddress = Util::FindPattern("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x50\x48\x8B\xBC\x24\x00\x00\x00\x00\x49\x8B\xF0", "xxxx?xxxx?xxxxxxxxx????xxx");

    InitListen = reinterpret_cast<decltype(InitListen)>(pInitListenAddress);

    auto pFindCollectionByTypeOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x4C\x8B\xF0\x4C\x8B\xFB", "x????xxxxxx");
    auto pFindCollectionByTypeAddress = pFindCollectionByTypeOffset + 5 + *reinterpret_cast<int32_t*>(pFindCollectionByTypeOffset + 1);

    FindCollectionByType = reinterpret_cast<decltype(FindCollectionByType)>(pFindCollectionByTypeAddress);

    /*auto pTickOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x49\x8B\x8D\x00\x00\x00\x00\x8B\x81\x00\x00\x00\x00", "x????xxx????xx????");
    auto pTickAddress = pTickOffset + 5 + *reinterpret_cast<int32_t*>(pTickOffset + 1);

    MH_CreateHook(static_cast<LPVOID>(pTickAddress), TickHook, reinterpret_cast<LPVOID*>(&Tick));
    MH_EnableHook(static_cast<LPVOID>(pTickAddress));

    auto pTickDispatchAddress = Util::FindPattern("\x4C\x8B\xDC\x55\x53\x49\x8D\xAB\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x49\x89\x73\x18\x49\x89\x7B\xE8\x4D\x89\x63\xE0\x45\x33\xE4", "xxxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxxxxxx");

    TickDispatch = reinterpret_cast<decltype(TickDispatch)>(pTickDispatchAddress);*/

    /*auto pBroadcast0Offset = Util::FindPattern("\xE8\x00\x00\x00\x00\x49\x63\x7E\x0C", "x????xxxx");
    auto pBroadcast0Address = pBroadcast0Offset + 5 + *reinterpret_cast<int32_t*>(pBroadcast0Offset + 1);

    MH_CreateHook(static_cast<LPVOID>(pBroadcast0Address), Broadcast0Hook, reinterpret_cast<LPVOID*>(&Broadcast0));
    MH_EnableHook(static_cast<LPVOID>(pBroadcast0Address));*/

    /*auto pBroadcast1Offset = Util::FindPattern("\xE8\x00\x00\x00\x00\x87\x7B\x70", "x????xxx");
    auto pBroadcast1Address = pBroadcast1Offset + 5 + *reinterpret_cast<int32_t*>(pBroadcast1Offset + 1);

    MH_CreateHook(static_cast<LPVOID>(pBroadcast1Address), Broadcast1Hook, reinterpret_cast<LPVOID*>(&Broadcast1));
    MH_EnableHook(static_cast<LPVOID>(pBroadcast1Address));*/

    polaris::Console::LogRaw("Welcome to Polaris!", 11);
    /*new polaris::Renderer; // Initialize renderer.
    new polaris::Athena; // TEMP: Initialize Athena.*/

    Util::InitSdk();
    Util::InitCore();

    if (Listen())
        printf("listen success\n");

    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
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
