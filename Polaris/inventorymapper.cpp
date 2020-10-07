#include "inventorymapper.h"

namespace polaris
{
	InventoryMapper::InventoryMapper()
	{
		inventoryItems.push_back(gpAthena->m_pPlayerPawnPolaris->m_pPlayerPawn->CustomizationLoadout.Character->GetFullName());
		inventoryItems.push_back("FortWeaponRangedItemDefinition WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03");
		inventoryItems.push_back("FortWeaponRangedItemDefinition WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");
		inventoryItems.push_back("FortWeaponRangedItemDefinition WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03");
		inventoryItems.push_back("FortWeaponRangedItemDefinition WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03");
		inventoryItems.push_back("FortWeaponRangedItemDefinition WID_Sniper_AMR_Athena_SR_Ore_T03.WID_Sniper_AMR_Athena_SR_Ore_T03");
	}

	void InventoryMapper::Draw()
	{
		ImGui::Begin("Inventory Mapper", &m_bShowWindow);
		{
			ImGui::Button("Slot 1", ImVec2(70, 70));
			ImGui::SameLine();
			ImGui::Button("Slot 2", ImVec2(70, 70));
			ImGui::SameLine();
			ImGui::Button("Slot 3", ImVec2(70, 70));
			ImGui::SameLine();
			ImGui::Button("Slot 4", ImVec2(70, 70));
			ImGui::SameLine();
			ImGui::Button("Slot 5", ImVec2(70, 70));
			ImGui::SameLine();
			ImGui::Button("Slot 6", ImVec2(70, 70));
			ImGui::End();
		}
	}
}