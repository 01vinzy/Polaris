#include <windows.h>

#include "common.h"
#include "renderer.h"
#include "ui.h"
#include "frontendmanager.h"

#include <cstdio>

DWORD WINAPI Main(LPVOID lpParam)
{
    AllocConsole();

    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);

    if (MH_Initialize() != MH_OK)
    {
        MessageBox(0, L"Failed to initialize MinHook.", L"Error", MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    polaris::Console::LogRaw("Welcome to Polaris!", 11);
    new polaris::Renderer; // Initialize renderer.
    //new polaris::Ui; // Initialize UI.
    //new polaris::FrontendManager; // Initialize Frontend Manager. ([irma] UNFINISHED!)

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

