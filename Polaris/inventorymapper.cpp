#include "inventorymapper.h"

polaris::InventoryMapper* gpInventoryMapper;

namespace polaris
{
	template<typename T>
	static std::list<T*> FindObject(const std::string& sClassName)
	{
		std::list<T*> results;
		for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
		{
			auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
			if (pObject != nullptr && pObject->GetFullName().find("FortniteGame") == std::string::npos)
			{
				if (pObject->GetFullName().rfind(sClassName, 0) == 0)
					results.push_back(static_cast<T*>(pObject));
			}
		}

		return results;
	}

	std::string InventoryMapper::GetRarityString(SDK::EFortRarity rarity)
	{
		switch (rarity)
		{
		case SDK::EFortRarity::Handmade:
			return "Common";

		case SDK::EFortRarity::Ordinary:
			return "Uncommon";

		case SDK::EFortRarity::Sturdy:
			return "Rare";
			
		case SDK::EFortRarity::Quality:
			return "Epic";

		case SDK::EFortRarity::Fine:
			return "Legendary";

		default:
			return "This is a poglaris moment";
		}
	}

	InventoryMapper::InventoryMapper()
	{
		if (gpInventoryMapper != nullptr)
		{
			Util::ThrowFatalError(L"InventoryMapper is already initialized!");
			return;
		}

		m_aInventoryItems[0] = "";
		m_aInventoryItems[1] = "FortWeaponRangedItemDefinition WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03";
		m_aInventoryItems[2] = "FortWeaponRangedItemDefinition WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03";
		m_aInventoryItems[3] = "FortWeaponRangedItemDefinition WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03";
		m_aInventoryItems[4] = "FortWeaponRangedItemDefinition WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03";
		m_aInventoryItems[5] = "FortWeaponRangedItemDefinition WID_Sniper_AMR_Athena_SR_Ore_T03.WID_Sniper_AMR_Athena_SR_Ore_T03";

		m_aInventoryItemNames[0] = "Pickaxe";
		m_aInventoryItemNames[1] = "Pump Shotgun";
		m_aInventoryItemNames[2] = "Assault Rifle";
		m_aInventoryItemNames[3] = "Tactical Shotgun";
		m_aInventoryItemNames[4] = "Jack-O-Launcher";
		m_aInventoryItemNames[5] = "Zapatron";

		gpInventoryMapper = this;
	}

	DWORD WINAPI LoadItemThread(LPVOID lpParam)
	{
		gpInventoryMapper->m_lItemsInMemory = FindObject<SDK::UFortWeaponRangedItemDefinition>("FortWeaponRangedItemDefinition");
	}

	void InventoryMapper::Draw()
	{
		ImGui::Begin("Inventory Mapper", &m_bShowWindow, ImGuiWindowFlags_NoResize);
		{
			for (int i = 0; i < 6; ++i)
			{
				char buffer[7];
				const char* windowTitle = "Slot %i";
				sprintf_s(buffer, sizeof(buffer), windowTitle, (i + 1));

				if (ImGui::Button(buffer, ImVec2(70, 70)) && i > 0)
				{
					m_bInteractable = false;
					m_bPickingNewWID = true;
					m_iPickingNewWIDFor = i;
				}

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(m_aInventoryItemNames[i].c_str());

				ImGui::SameLine();
			}
			ImGui::End();
		}

		if (m_iPickingNewWIDFor > 0)
		{
			if (m_lItemsInMemory.size() == 0)
				CreateThread(0, 0, LoadItemThread, 0, 0, 0);
		
			// Hacky code for the window title.
			char buffer[26];
			const char* windowTitle = "Select an item for Slot %i";
			sprintf_s(buffer, sizeof(buffer), windowTitle, (m_iPickingNewWIDFor + 1));

			ImGui::SetNextWindowSize(ImVec2(540, 525), ImGuiCond_Appearing);
			ImGui::Begin(buffer, &m_bPickingNewWID, ImGuiWindowFlags_NoResize);
			{
				for (int i = 0; i < m_lItemsInMemory.size(); ++i)
				{
					std::string fullName = (*std::next(m_lItemsInMemory.begin(), i))->GetFullName();
					std::string name = (*std::next(m_lItemsInMemory.begin(), i))->GetName() + " (" + GetRarityString((*std::next(m_lItemsInMemory.begin(), i))->GetRarity()) + ")";

					// Block out StW released weapons, cus piracy n shit.
					// The || condition at the end just prevents dev weapons from being excluded.
					if (fullName.find("WID") != std::string::npos && (fullName.find("Athena") != std::string::npos || fullName.find("Test") != std::string::npos) || fullName.find("WID") == std::string::npos)
					{
						std::list<SDK::UFortWeaponRangedItemDefinition*>::iterator wimIterator = m_lItemsInMemory.begin();
						std::advance(wimIterator, i);

						if (ImGui::Selectable(name.c_str()))
						{
							m_aInventoryItems[m_iPickingNewWIDFor] = (*wimIterator)->GetFullName();
							m_aInventoryItemNames[m_iPickingNewWIDFor] = (*wimIterator)->GetName();

							m_bInteractable = true;
							m_bPickingNewWID = false;
							m_iPickingNewWIDFor = 0;
						}
					}
				}
				ImGui::End();
			}
		}
	}
}