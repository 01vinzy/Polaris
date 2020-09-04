#include "core.h"
#include "mainwindow.h"
#include "creditswindow.h"
#include "actorlist.h"
#include "athena.h"
#include "renderer.h"
#include "objectcache.h"
#include "watermark.h"

#include <imgui.h>

namespace polaris
{
	ActorList* pActorList;
	CreditsWindow* pCreditsWindow;
	ObjectCache* pObjectCache;
	Watermark* pWatermark;

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

		// Create window instances.
		pWatermark = new polaris::Watermark;
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
				if (ImGui::MenuItem("Exit"))
					ExitProcess(EXIT_SUCCESS);

				// Only give the user this option if experimental & debugging tools are enabled.
				if (ENABLE_DEBUGGING_TOOLS && ENABLE_EXPERIMENTAL_DEBUGGING_TOOLS)
					if (ImGui::MenuItem("Object Cache (Lags!)"))
						pObjectCache->bShowWindow = true;

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

#ifndef POLARIS_RELEASE

				if (ImGui::MenuItem("Irma Burger"))
					system("start https://www.youtube.com/watch?v=fC7oUOUEEi4");

#endif // !POLARIS_RELEASE

				ImGui::EndMenu();
			}

			ImGui::End();
		}
	}
}
