#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <iostream>

#include "common.h"

namespace polaris
{
	class Console
	{
	public:
		static void Log(const char* msg);
		static void LogError(const char* msg);
		static void LogRaw(const char* msg, int color = 15);
	};
}

#endif // CONSOLE_H
