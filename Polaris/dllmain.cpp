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
VOID (*InitHandler)(SDK::UNetConnection*) = nullptr;

VOID InitHandlerHook(SDK::UNetConnection* what)
{
    // init
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

    /*auto pCreateNamedNetDriverOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x84\xC0\x75\x43\x80\x3D\x00\x00\x00\x00\x00", "x????xxxxxx?????");
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

    FindCollectionByType = reinterpret_cast<decltype(FindCollectionByType)>(pFindCollectionByTypeAddress);*/

    polaris::Console::LogRaw("Welcome to Polaris!", 11);
    /*new polaris::Renderer; // Initialize renderer.
    new polaris::Athena; // TEMP: Initialize Athena.*/

    /*Util::InitSdk();
    Util::InitCore();

    if (Listen())
        printf("listen success");*/

    auto pProcessEventAddress = Util::FindPattern("\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x50\x48\x8B\xF9\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x33\xED\x48\x85\xC0\x74\x0D\x48\x8B\xC8\xE8\x00\x00\x00\x00\x48\x8B\xC8\xEB\x03\x48\x8B\xCD\x48\x8D\xB7\x00\x00\x00\x00\x48\x8D\x44\x24\x00\x48\x3B\xF0\x74\x13\x4C\x8B\x06\x48\x89\x0E\x4D\x85\xC0\x74\x17\x49\x8B\x00\x49\x8B\xC8\xEB\x08\x48\x85\xC9\x74\x0A\x48\x8B\x01\xBA\x00\x00\x00\x00\xFF\x10\x48\x8B\x0E\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x8B\x47\x58", "xxxx?xxxx?xxxxxxxxx????x????xxxxxxxxxxx????xxxxxxxxxxx????xxxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxx????xxxx");
    if (!pProcessEventAddress)
    {
        MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for ProcessEvent has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    MH_CreateHook(static_cast<LPVOID>(pProcessEventAddress), InitHandlerHook, reinterpret_cast<LPVOID*>(&InitHandler));
    MH_EnableHook(static_cast<LPVOID>(pProcessEventAddress));

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
