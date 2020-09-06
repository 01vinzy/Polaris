#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <cstdio>
#include <io.h>

#include "SDK.hpp"
#include "renderer.h"
#include "common.h"
#include "athena.h"
#include "console.h"
#include "util.h"

// The main thread for Polaris. Called when the Polaris DLL has successfully been injected.
DWORD WINAPI Main(LPVOID lpParam)
{
	// If MinHook failed to properly initialize, prompt the user to restart the game.
	if (MH_Initialize() != MH_OK)
	{
		MessageBox(0, L"Failed to initialize MinHook.", L"Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}

	// Start the Polaris modules.
	new polaris::Console(); // Initialize the debug console.
	new polaris::Renderer; // Initialize the Polaris renderer.

	new polaris::Athena; // TEMP Start Athena.

	return FALSE;
}

// The entry point of the DLL. Called when the DLL is injected.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, Main, hModule, 0, 0);
		break;
	}

	return TRUE;
}
