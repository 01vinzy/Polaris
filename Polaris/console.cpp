#include "console.h";

HANDLE hConsole;

namespace polaris
{
	// log a message to console
	void Console::Log(const char* msg)
	{
		if(!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, 11);

		std::cout << "LogPolaris:";

		SetConsoleTextAttribute(hConsole, 15);

		std::cout << "Display: " << msg << std::endl;
	}

	// log an error to console
	void Console::LogError(const char* msg)
	{
		if (!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, 11);

		std::cout << "LogPolaris:";

		SetConsoleTextAttribute(hConsole, 12);

		std::cout << "Error: " << msg << std::endl;

		SetConsoleTextAttribute(hConsole, 15);
	}

	// log a message to the console without the Polaris prefix.
	void Console::LogRaw(const char* msg, int color)
	{
		if (!hConsole)
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, color);

		std::cout << msg << std::endl;

		SetConsoleTextAttribute(hConsole, 15);
	}
}