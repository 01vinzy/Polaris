#include "core.h"
#include "mainwindow.h"
#include "creditswindow.h"

#include <imgui.h>

namespace polaris
{
	MainWindow::MainWindow()
	{
		Console::Log("Initializing MainWindow");
	}

	MainWindow::~MainWindow()
	{
		//TODO: unload MainWindow
	}

	void MainWindow::Draw()
	{
		int slomo;
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("Player"))
			{
				if (ImGui::MenuItem("Exit", "Ctrl+W")) { }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("World"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Credits"))
					new polaris::CreditsWindow;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
