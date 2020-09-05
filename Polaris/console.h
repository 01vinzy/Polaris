#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"
#include "util.h"
#include <string>
#include <iostream>

namespace polaris
{
	class Console
	{
	public:
		Console();
		static void Log(const char* msg);
		static void LogError(const char* msg);
		static void LogRaw(const char* msg, int color = 15);
	};
}

#endif // CONSOLE_H
