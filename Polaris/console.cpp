#include "console.h"
#include "polarisflags.h"

#include <iostream>

HANDLE hConsole;

namespace polaris
{
	DWORD DrawAsciiArt(LPVOID lpParam)
	{
		for (int i = 0; i < 19; i++)
		{
			auto addOn = i == 10 ? VERSION.c_str() : "";
			auto appendedString = std::string(Console::cPolarisAscii[i]) + std::string(addOn);

			polaris::Console::LogRaw(appendedString.c_str(), 11);
		}
	}

	Console::Console()
	{
		Util::InitConsole();

		// Draw fancy thing :D
		CreateThread(0, 0, DrawAsciiArt, 0, 0, 0);
		Sleep(500);
	}

	// Log a message to console
	void Console::Log(const char* msg)
	{
		if(!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Make the console text cyan to differentiate Polaris from Fortnite logs
		SetConsoleTextAttribute(hConsole, 11);

		std::cout << "LogPolaris:";

		// Make the console text white again
		SetConsoleTextAttribute(hConsole, 15);

		std::cout << "Display: " << msg << std::endl;
	}

	// Log an error to console.
	void Console::LogError(const char* msg)
	{
		if (!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Make the console text cyan to differentiate Polaris from Fortnite logs
		SetConsoleTextAttribute(hConsole, 11);

		std::cout << "LogPolaris:";

		// Make the console text red, indicating an error.
		SetConsoleTextAttribute(hConsole, 12);

		std::cout << "Error: " << msg << std::endl;

		// Make the console text white again
		SetConsoleTextAttribute(hConsole, 15);
	}

	// Log a message to the console without the Polaris prefix.
	void Console::LogRaw(const char* msg, int color)
	{
		if (!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Make the console text any color specified by `color`
		SetConsoleTextAttribute(hConsole, color);

		std::cout << msg << std::endl;

		// Make the console text white again
		SetConsoleTextAttribute(hConsole, 15);
	}
}
