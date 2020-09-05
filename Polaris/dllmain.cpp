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

	// Test
	HWND hWnd = FindWindowA("UnrealWindow", "Fortnite ");
	if (!hWnd)
	{
		MessageBox(0, L"Failed to find Fortnite window.", L"Error", MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
	}
	else
	{
		int byte1 = 0x85;
		int byte2 = 0x8D;

		DWORD PID;
		GetWindowThreadProcessId(hWnd, &PID);
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, PID);

		if (!hProc)
		{
			MessageBox(0, L"Cannot open process.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		else
		{
			int override1 = WriteProcessMemory(hProc, (LPVOID)0x0028F9A2, &byte1, (DWORD)sizeof(&byte1), NULL);
			int override2 = WriteProcessMemory(hProc, (LPVOID)0x0028F9AB, &byte2, (DWORD)sizeof(&byte2), NULL);

			if (override1 > 0)
				polaris::Console::Log("Success 1!");
			else
				polaris::Console::LogError("No dice.");

			if (override2 > 0)
				polaris::Console::Log("Success 2!");
			else
				polaris::Console::LogError("No dice.");

			CloseHandle(hProc);
		}
	}

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
