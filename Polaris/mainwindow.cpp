#include "core.h"
#include "mainwindow.h"
#include "creditswindow.h"
#include "actorlist.h"
#include "athena.h"
#include "encryptedflags.h"
#include "base64.h"
#include "renderer.h"

#include <imgui.h>

namespace polaris
{
	ActorList* pActorList;
	CreditsWindow* pCreditsWindow;

	MainWindow::MainWindow()
	{
		Console::Log("Initializing MainWindow");

		pActorList = new ActorList;
		pCreditsWindow = new CreditsWindow;
		bShowWindow = true;
	}

	MainWindow::~MainWindow()
	{
		//TODO: unload MainWindow
	}

	void MainWindow::Draw()
	{
		std::string decryptedTitle = base64_decode(ENCRYPTED_NAME).c_str();

		ImGui::Begin(decryptedTitle.c_str());
		{
			ImGui::TextWrapped("This window's title is encrypted!");
			ImGui::Checkbox("Actor Inspector", &pActorList->bShowWindow);
			ImGui::Checkbox("Credits", &pCreditsWindow->bShowWindow);
			if (ImGui::Button("Athena Test"))
			{
				new polaris::Athena;
			}

			/*if (ImGui::BeginMenu("Player"))
			{
				if (ImGui::MenuItem("Exit", "Ctrl+W")) { }
				if(gpAthena == nullptr)
					if (ImGui::MenuItem("Load Athena", "Ctrl+A")) { new polaris::Athena; }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("World"))
			{
				if (ImGui::MenuItem("Actor Inspector"))
					new polaris::ActorList;
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Credits"))
					new polaris::CreditsWindow;

				ImGui::EndMenu();
			}*/

			ImGui::End();
		}
	}
}
