#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <inttypes.h>
#include <psapi.h>
#include <stdio.h>
#include <MinHook.h>

#pragma comment(lib, "minhook.lib")

#include "globals.h";
#include "common.h"

namespace polaris
{
	class Utils
	{
	private:
		static DWORD PossessThread(LPVOID lpParam);
		static BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask);

	public:
		static void OpenTwitter();
		static void InitializeHook();
		static void InitializeSDK();
		static void Possess(SDK::AActor* pPawn);
		static uintptr_t BaseAddress();
		static PBYTE FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask);
		static SDK::AActor* FindActor(SDK::UClass* pClass, int iSkip = 0);
		static SDK::FString ToFString(std::string string);
	};
}

#endif // UTILS_H
