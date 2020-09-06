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

	/*
	// TODO(Cyuubi): Move this into a seperate class for patches.
	auto pAbilityPatchAddress = polaris::Util::FindPattern
	(
		"\xC0\x0F\x84\x3C\x02\x00\x00\x0F\x2F\xF7\x0F\x86\xF5\x00\x00\x00",
		"xxxxxxxxxxxxxxxx"
	);
	if (!pAbilityPatchAddress)
	{
		MessageBox(NULL, static_cast<LPCWSTR>(L"An error has occured, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}

	DWORD curProtection;
	VirtualProtect(pAbilityPatchAddress, 16, PAGE_EXECUTE_READWRITE, &curProtection);

	reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[2] = 0x85;
	reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[11] = 0x8D;

	DWORD temp;
	VirtualProtect(pAbilityPatchAddress, 16, curProtection, &temp);
	*/

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
