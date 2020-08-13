#include "mainwindow.h"

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
		ImGui::Begin("Polaris", reinterpret_cast<bool*>(true), ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit Polaris", "Ctrl+W")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Twitter")) 
				{
					Utils::OpenTwitter();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Plot some values
		const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();

		ImGui::End();
	}
}
