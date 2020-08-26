#include "actorlist.h"

#include "core.h"

namespace polaris
{
	ActorList::ActorList()
	{
		Console::Log("Initializing ActorList");
	}

	void ActorList::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(820, 440), ImGuiCond_Appearing);
		ImGui::Begin("Actors", &bShowWindow, ImGuiWindowFlags_Modal);
		{
			// Actor Hierarchy
			static int selected = 0;
			{
				ImGui::BeginChild("Hierarchy", ImVec2(300, 0), true);
				{
					for (int i = 0; i < Core::pLevel->Actors.Num(); i++)
					{
						// Absolutely make sure the actor exists, Unreal Engine is very prone to crashing
						if (Core::pLevel->Actors.IsValidIndex(i) && Core::pLevel->Actors[i])
						{
							// Create a button for the actor, and set the selected actor once clicked
							if (ImGui::Selectable(Core::pLevel->Actors[i]->GetName().c_str(), selected == i))
								selected = i;
						}
					}
					ImGui::EndChild();
				}
			}

			auto selectedActor = Core::pLevel->Actors[selected];

			// Inspector
			// Only draw the inspector screen if the selected actor exists
			if (selectedActor)
			{
				ImGui::SameLine();

				ImGui::BeginGroup();
				ImGui::BeginChild("Inspector", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				ImGui::Text(selectedActor->GetName().c_str());
				ImGui::Separator();
				ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None);
				{
					if (ImGui::BeginTabItem("Properties"))
					{
						ImGui::TextWrapped("ID: %s", selectedActor->GetFullName().c_str());

						// Position and Rotation field
						ImGui::TextWrapped("Position: (%d, %d, %d)", selectedActor->K2_GetActorLocation().X, selectedActor->K2_GetActorLocation().Y, selectedActor->K2_GetActorLocation().Z);
						ImGui::TextWrapped("Rotation: (%d, %d, %d)", selectedActor->K2_GetActorRotation().Pitch, selectedActor->K2_GetActorRotation().Roll, selectedActor->K2_GetActorRotation().Yaw);

						// Owner field
						auto ownerLabel = selectedActor->Owner != nullptr ? selectedActor->Owner->GetName().c_str() : "None";
						ImGui::TextWrapped("Owner: %s", ownerLabel);
						
						// Authority field
						auto authorityLabel = selectedActor->HasAuthority() ? "Yes" : "No";
						ImGui::TextWrapped("Has Authority: %s", authorityLabel);

						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::EndChild();

				if (ImGui::Button("Destroy")) 
					selectedActor->K2_DestroyActor();

				ImGui::EndGroup();
			}
		}
		ImGui::End();
	}
}