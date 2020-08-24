#ifndef CREDITSWINDOW_H
#define CREDITSWINDOW_H

#include "common.h"
#include "window.h"

namespace polaris
{
	class CreditsWindow : public Window
	{
	public:
		const char* credits[18] = {
			"Polaris Lead:",
			"Cyuubi",
			"",
			"Developers:",
			"kanner",
			"",
			"Contributors:",
			"BigBoiTaj2005",
			"Chase",
			"irma",
			"pivotman319",
			"Samicc",
			"wiktorwiktor12",
			"",
			"Relation:",
			"Crush",
			"Darkblade",
			"Slushia"
		};

		CreditsWindow();
		~CreditsWindow();
		void Draw() override;
	};
}

#endif // CREDITSWINDOW_H
