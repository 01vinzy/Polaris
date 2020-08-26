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
		Console::Log("Initializing MainWindow");

		// Create window instances we reuse.
		// We want to reuse these to not hog up memory.
		pWatermark = new polaris::Watermark;
		pActorList = new polaris::ActorList;
		pCreditsWindow = new polaris::CreditsWindow;
		pObjectCache = new polaris::ObjectCache;
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

				if (ImGui::MenuItem("Object Cache (Lags!)"))
					pObjectCache->bShowWindow = true;

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

				if (ImGui::MenuItem("Irma Burger"))
					system("start https://www.youtube.com/watch?v=fC7oUOUEEi4");

				ImGui::EndMenu();
			}

			ImGui::End();
		}
	}
}
