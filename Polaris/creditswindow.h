#ifndef CREDITSWINDOW_H
#define CREDITSWINDOW_H

#include "common.h"
#include "window.h"

namespace polaris
{
	class CreditsWindow : public Window
	{
	public:
		const char* credits[17] = {
			"Polaris Owner:",
			"@uguuCyuubi",
			"",
			"Developer:",
			"@omairma",
			"",
			"Contributors:",
			"@kann3r",
			"@pivotman319",
			"@wiktorwiktor12a",
			"",
			"Special Thanks:",
			"@crusherrrz",
			"@DarkbladeEU",
			"@doshisgone",
			"@notsamiccc",
			"@Slushia"
		};

		CreditsWindow();
		~CreditsWindow();
		void Draw() override;
	};
}

#endif // CREDITSWINDOW_H
