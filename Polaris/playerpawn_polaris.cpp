#include "playerpawn_polaris.h"
#include "util.h"
#include "SDK.hpp"

#include <map>
#include <iostream>

// NOTE (irma) I couldn't move the loading into memory shit into their own classes. LMK if you can do it.

namespace polaris
{
	// Caching these the first time inithero is called, should reduce hitch when jumping out of battle bus.
	static SDK::UCustomCharacterPart* pCustomCharacterPartHead;
	static SDK::UCustomCharacterPart* pCustomCharacterPartBody;
	static SDK::UCustomCharacterPart* pCustomCharacterPartHat;

	std::map<std::string, std::string> mPickaxeAsWid
	{
		std::make_pair("BoltonPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_BoltOn_Athena_C_T01.WID_Harvest_Pickaxe_BoltOn_Athena_C_T01"),
		std::make_pair("DefaultPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"),
		std::make_pair("HalloweenScythe", "FortWeaponMeleeItemDefinition WID_Harvest_HalloweenScythe_Athena_C_T01.WID_Harvest_HalloweenScythe_Athena_C_T01"),
		std::make_pair("HappyPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Smiley_Athena_C_T01.WID_Harvest_Pickaxe_Smiley_Athena_C_T01"),
		std::make_pair("Pickaxe_Deathvalley", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Deathvalley_Athena_C_T01.WID_Harvest_Pickaxe_Deathvalley_Athena_C_T01"),
		std::make_pair("Pickaxe_Flamingo", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Flamingo_Athena_C_T01.WID_Harvest_Pickaxe_Flamingo_Athena_C_T01"),
		std::make_pair("Pickaxe_Lockjaw", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Lockjaw_Athena_C_T01.WID_Harvest_Pickaxe_Lockjaw_Athena_C_T01"),
		std::make_pair("SickleBatPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SickleBat_Athena_C_T01.WID_Harvest_Pickaxe_SickleBat_Athena_C_T01"),
		std::make_pair("SkiIcePickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_IcePick_Athena_C_T01.WID_Harvest_Pickaxe_IcePick_Athena_C_T01"),
		std::make_pair("SpikyPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Spikey_Athena_C_T01.WID_Harvest_Pickaxe_Spikey_Athena_C_T01")
	};

	// Search an object in the UObject cache.
	template<typename T>
	static T* FindObject(const std::string& sClassName, const std::string& sQuery)
	{
		for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
		{
			auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
			if (pObject != nullptr && pObject->GetFullName().find("F_Med_Head1") == std::string::npos)
			{
				if (pObject->GetFullName().rfind(sClassName, 0) == 0 && pObject->GetFullName().find(sQuery) != std::string::npos)
					return static_cast<T*>(pObject);
			}
		}

		return nullptr;
	}

	PlayerPawnPolaris::PlayerPawnPolaris()
	{
		// Summon a new PlayerPawn.
		std::string sPawnClassName = "PlayerPawn_Athena_C";
		Globals::gpPlayerController->CheatManager->Summon(SDK::FString(std::wstring(sPawnClassName.begin(), sPawnClassName.end()).c_str()));

		m_pPlayerPawn = static_cast<SDK::APlayerPawn_Athena_C*>(Util::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
		if (!m_pPlayerPawn)
		{
			MessageBox(0, L"Failed to spawn PlayerPawn_Athena_C.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		else
		{
			Globals::gpPlayerController->Possess(m_pPlayerPawn);

			// Tell the PlayerPawn that our customization was replicated.
			m_pPlayerPawn->OnRep_CustomizationLoadout();

			// Assign our PlayerPawn to a team.
			auto playerState = static_cast<SDK::AFortPlayerStateAthena*>(Globals::gpPlayerController->PlayerState);

			playerState->TeamIndex = SDK::EFortTeam::HumanPvP_Team1;
			playerState->OnRep_TeamIndex();

			// Give the player a pickaxe.
			EquipWeapon(mPickaxeAsWid[m_pPlayerPawn->CustomizationLoadout.Character->GetName()].c_str());
		}
	}

	// FIXME (irma) Replace this with a proper Skin Loader.
	void PlayerPawnPolaris::InitializeHero()
	{
		auto pPlayerState = static_cast<SDK::AFortPlayerStateAthena*>(Globals::gpPlayerController->PlayerState);
		
		if (!pCustomCharacterPartHead)
			pCustomCharacterPartHead = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Head");
		if (!pCustomCharacterPartBody)
			pCustomCharacterPartBody = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Body");
		if (!pCustomCharacterPartHat)
			pCustomCharacterPartHat = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Hat_");

		// Assign custom character parts to the player.
		pPlayerState->CharacterParts[0] = pCustomCharacterPartHead;
		pPlayerState->CharacterParts[1] = pCustomCharacterPartBody;
		pPlayerState->CharacterParts[3] = pCustomCharacterPartHat;

		// If no head was found, force Ramirez's head.
		if (!pPlayerState->CharacterParts[0])
			pPlayerState->CharacterParts[0] = SDK::UObject::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart F_Med_Head1.F_Med_Head1");
		
		// If no body was found, force Ramirez's body.
		if (!pPlayerState->CharacterParts[1])
			pPlayerState->CharacterParts[1] = SDK::UObject::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart F_Med_Soldier_01.F_Med_Soldier_01");


		static_cast<SDK::AFortPlayerStateAthena*>(Globals::gpPlayerController->PlayerState)->OnRep_CharacterParts();
	}

	// Equip a weapon.
	void PlayerPawnPolaris::EquipWeapon(const char* cItemDef, int iGuid)
	{
		SDK::FGuid guid;
		guid.A = iGuid;
		guid.B = iGuid;
		guid.C = iGuid;
		guid.D = iGuid;

		m_pPlayerPawn->EquipWeaponDefinition(SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>(cItemDef), guid)->SetOwner(m_pPlayerPawn);
	}
}