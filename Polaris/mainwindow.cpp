#include "core.h"
#include "mainwindow.h"
#include "creditswindow.h"
#include "actorlist.h"
#include "athena.h"
#include "renderer.h"

#include <imgui.h>

namespace polaris
{
	ActorList* pActorList;
	CreditsWindow* pCreditsWindow;

	MainWindow::MainWindow()
	{
		Console::Log("Initializing MainWindow");

		// Create window instances we reuse.
		// We want to reuse these to not hog up memory.
		pActorList = new polaris::ActorList;
		pCreditsWindow = new polaris::CreditsWindow;
	}

	MainWindow::~MainWindow()
	{
		//TODO: unload MainWindow
	}

	void MainWindow::Draw()
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("Player"))
			{
				if (ImGui::MenuItem("Exit", "Ctrl+W"))
					ExitProcess(EXIT_SUCCESS);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("World"))
			{
				if (ImGui::MenuItem("Actor Inspector"))
					pActorList->bShowWindow = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Credits"))
					pCreditsWindow->bShowWindow = true;

				// Get stickbugged LOL
				if (ImGui::MenuItem("Irma Burger"))
					system("start https://www.youtube.com/watch?v=fC7oUOUEEi4");

				ImGui::EndMenu();
			}

			ImGui::End();
		}
	}
}
