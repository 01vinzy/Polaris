#include "console.h";

HANDLE hConsole;

namespace polaris
{
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
