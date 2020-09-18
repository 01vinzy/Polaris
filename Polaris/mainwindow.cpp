#include "mainwindow.h"
#include "globals.h"
#include "creditswindow.h"
#include "actorlist.h"
#include "renderer.h"
#include "objectcache.h"
#include "watermark.h"

#include <imgui.h>

namespace polaris
{
	// These are 
	ActorList* pActorList = nullptr;
	CreditsWindow* pCreditsWindow = nullptr;
	ObjectCache* pObjectCache = nullptr;
	Watermark* pWatermark = nullptr;

	MainWindow::MainWindow()
	{
		Console::Log("Initializing UI");

		// Only enable these tools if debugging tools are enabled.
		if (ENABLE_DEBUGGING_TOOLS)
		{
			pActorList = new polaris::ActorList;
			pCreditsWindow = new polaris::CreditsWindow;

			// This is an highly experimental tool.
			if(ENABLE_EXPERIMENTAL_DEBUGGING_TOOLS)
				pObjectCache = new polaris::ObjectCache;
		}

		if (gpMainWindow)
		{
			MessageBox(0, L"Athena is already initialized.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}

		gpMainWindow = this;

		// Create window instances.
		pWatermark = new polaris::Watermark;
	}

	void MainWindow::Draw()
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("Player"))
			{
				// Only give the user this option if experimental & debugging tools are enabled.
				if (ENABLE_DEBUGGING_TOOLS && ENABLE_EXPERIMENTAL_DEBUGGING_TOOLS)
					if (ImGui::MenuItem("Object Cache (Experimental)"))
						pObjectCache->bShowWindow = true;

				if (!Globals::gpLocalPlayer->ViewportClient->ViewportConsole)
				{
					if (ImGui::MenuItem("Enable Console"))
					{
						auto pConsole = SDK::UConsole::StaticClass()->CreateDefaultObject<SDK::UConsole>();

						pConsole->Outer = Globals::gpLocalPlayer->ViewportClient;

						Globals::gpLocalPlayer->ViewportClient->ViewportConsole = pConsole;
					}
				}
				else
				{
					if (ImGui::MenuItem("Disable Console"))
						Globals::gpLocalPlayer->ViewportClient->ViewportConsole = nullptr;
				}

				if (ImGui::MenuItem("Exit"))
					ExitProcess(EXIT_SUCCESS);

				ImGui::EndMenu();
			}

			if(ENABLE_DEBUGGING_TOOLS)
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

				if (ImGui::MenuItem("Irma Burger"))
					system("start https://www.youtube.com/watch?v=fC7oUOUEEi4");

				ImGui::EndMenu();
			}

			ImGui::End();
		}
	}
}
