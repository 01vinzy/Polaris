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

	InventoryMapper::InventoryMapper()
	{
		if (gpInventoryMapper != nullptr)
		{
			Util::ThrowFatalError(L"InventoryMapper is already initialized!");
			return;
		}

		m_lInventoryItems.push_back("Pickaxe");
		m_lInventoryItems.push_back("FortWeaponRangedItemDefinition WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03");
		m_lInventoryItems.push_back("FortWeaponRangedItemDefinition WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");
		m_lInventoryItems.push_back("FortWeaponRangedItemDefinition WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03");
		m_lInventoryItems.push_back("FortWeaponRangedItemDefinition WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03");
		m_lInventoryItems.push_back("FortWeaponRangedItemDefinition WID_Sniper_AMR_Athena_SR_Ore_T03.WID_Sniper_AMR_Athena_SR_Ore_T03");

		gpInventoryMapper = this;
	}

	void InventoryMapper::Draw()
	{
		ImGui::Begin("Inventory Mapper", &m_bShowWindow, ImGuiWindowFlags_NoResize);
		{
			for (int i = 0; i < m_lInventoryItems.size(); ++i)
			{
				if (ImGui::Button((*std::next(m_lInventoryItems.begin(), i)).c_str(), ImVec2(70, 70)) && i > 0)
				{
					m_iPickingNewWIDFor = i;
				}
				ImGui::SameLine();
			}
			ImGui::End();
		}

		if (m_iPickingNewWIDFor > 0)
		{
			if (m_lItemsInMemory.size() == 0)
				m_lItemsInMemory = FindObject<SDK::UFortWeaponRangedItemDefinition>("FortWeaponRangedItemDefinition");

			ImGui::Begin("Pick a new item...");
			{
				for (int i = 0; i < m_lItemsInMemory.size(); ++i)
				{
					if (ImGui::Selectable((*std::next(m_lItemsInMemory.begin(), i))->GetName().c_str()))
					{
						std::list<SDK::UFortWeaponRangedItemDefinition*>::iterator dick = m_lItemsInMemory.begin();
						std::list<std::string>::iterator ass = m_lInventoryItems.begin();
						std::advance(dick, i);
						std::advance(ass, m_iPickingNewWIDFor);

						m_lInventoryItems.erase(ass);
						m_lInventoryItems.insert(ass, (*dick)->GetFullName());

						m_iPickingNewWIDFor = 0;
					}
				}
				ImGui::End();
			}
		}
	}
}