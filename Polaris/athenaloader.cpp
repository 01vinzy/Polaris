/*
 * [irma] This class is unfinished, but I plan to have a system that loads the main Polaris behaviour when the player readies up
*/

#include "athenaloader.h"
#include "SDK.hpp";

namespace polaris 
{
	AthenaLoader::AthenaLoader() 
	{
		Console::Log("Starting Athena Thread");
		CreateThread(0, 0, AthenaMain, 0, 0, 0);
	}

	AthenaLoader::~AthenaLoader()
	{
		//TODO: Handle deconstruction.
	}

	DWORD WINAPI AthenaMain(LPVOID lpParam)
	{
		Console::Log("Loading Athena");
	}
}
