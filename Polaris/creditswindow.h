#ifndef CREDITSWINDOW_H
#define CREDITSWINDOW_H

#include "common.h"
#include "window.h"

namespace polaris
{
	class CreditsWindow : public Window
	{
	public:
		const char* credits[19] = {
			"Owner:",
			"Cyuubi",
			"",
			"Programming Assistance:",
			"BigBoiTaj2005",
			"Chase",
			"irma",
			"kanner",
			"pivotman319",
			"Samicc",
			"wiktorwiktor12",
			"",
			"Special Thanks:",
			"Crush",
			"Darkblade",
			"Slushia"
			"",
			"",
			"- HIGHLY UNFINISHED! -",
			"Just thought this needed an update."
		};

		CreditsWindow();
		~CreditsWindow();
		void Draw() override;
	};
}

#endif // CREDITSWINDOW_H
