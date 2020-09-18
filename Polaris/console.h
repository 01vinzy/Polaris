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
		static inline const char* sAscii[19] = {
			"                                                  ",
			"                                                  ",
			"                     -/oyhhddhyso/.               ",
			"                  -ohhhhhhhhhhhhhhhh+.            ",
			"                -syyhhhhhhyyyyyyyyhhhho.          ",
			"               +yyyyyys/.`    `-/syyyyyy/         ",
			"              oyyyyyo.            -syyyyy/        ",
			"             :sssss+               `oyysss-       Welcome to Polaris!",
			"             ssssso                 .sssss+       They said to think back, but I think we went a bit too far.",
			"       .----:ssssso:::::`            ssssso       ",
			"        ./++ooosssssss+`            .yyyyyo       ",
			"          -/++oosssso-             `oyyyyy-       ",
			"           `:/+ooos/`             -ossyyy+        ",
			"             -/+++.           `.:ossssss/         ",
			"              `:-          `+oooooosss+.          ",
			"                           `+ooooooo/.            ",
			"                           `+++/:-.               ",
			"                                                  ",
			"                                                  "
		};

		Console();

		static void Log(const char* msg);
		static void LogError(const char* msg);
		static void LogRaw(const char* msg, int color = 15);
	};
}

#endif // CONSOLE_H
