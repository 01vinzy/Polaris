#include "playerpawn_polaris.h"
#include "util.h"

namespace polaris
{
	static SDK::UObject* (*StaticLoadObject)(SDK::UClass* ObjectClass, SDK::UObject* InOuter, const TCHAR* InName, const TCHAR* Filename, uint32_t LoadFlags, SDK::UPackageMap* Sandbox, bool bAllowObjectReconciliation);

	// Search an object in the Object Cache.
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

	// Load an object in memory.
	template<class T>
	static T* LoadObject(SDK::UObject* Outer, const TCHAR* Name, const TCHAR* Filename = nullptr, uint32_t LoadFlags = 0, SDK::UPackageMap* Sandbox = nullptr)
	{
		if (!StaticLoadObject)
			StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(Util::BaseAddress() + 0x142E560);

		return (T*)StaticLoadObject(T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox, true);
	}

	// Find an object in cache, load it if it's not loaded.
	template<typename T>
	static T* FindOrLoadObject(const std::string PathName)
	{
		if (!StaticLoadObject)
			StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(Util::BaseAddress() + 0x142E560);

		SDK::UClass* Class = T::StaticClass();
		Class->CreateDefaultObject();

		T* ObjectPtr = LoadObject<T>(NULL, std::wstring(PathName.begin(), PathName.end()).c_str());
		if (ObjectPtr)
		{
			SDK::UObject::GObjects
				->ObjObjects.GetItemByIndex(ObjectPtr->InternalIndex)->Flags |= int32_t(SDK::FUObjectItem::ObjectFlags::RootSet);
		}

		return ObjectPtr;
	}

	PlayerPawnPolaris::PlayerPawnPolaris()
	{
		// Summon a new PlayerPawn.
		std::string sPawnClassName = "PlayerPawn_Athena_C";
		Core::pPlayerController->CheatManager->Summon(SDK::FString(std::wstring(sPawnClassName.begin(), sPawnClassName.end()).c_str()));

		m_pPlayerPawn = static_cast<SDK::APlayerPawn_Athena_C*>(Util::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
		if (!m_pPlayerPawn)
		{
			MessageBox(0, L"Failed to spawn PlayerPawn_Athena_C.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		else
		{
			Core::pPlayerController->Possess(m_pPlayerPawn);

			// Initialize pawn
			m_pPlayerPawn->ExecuteUbergraph(0);
			m_pPlayerPawn->OnRep_PawnUniqueID();
			m_pPlayerPawn->OnRep_CustomizationLoadout();
			m_pPlayerPawn->ReceiveBeginPlay();

			// Assign our PlayerPawn to a team.
			static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->TeamIndex = SDK::EFortTeam::HumanPvP_Team1;
			static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->OnRep_TeamIndex();

			// Give the player a pickaxe.
			EquipWeapon("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		}
	}

	// FIXME: (irma) Replace this with a proper Skin Loader.
	void PlayerPawnPolaris::InitializeHero()
	{
		auto pPlayerState = static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState);
		auto pCustomCharacterPartHead = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Head");
		auto pCustomCharacterPartBody = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Body");
		auto pCustomCharacterPartHat = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Hat_");

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

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->OnRep_CharacterParts();
		m_pPlayerPawn->OnCharacterPartsReinitialized();
	}

	void PlayerPawnPolaris::EquipWeapon(const char* cItemDef)
	{
		// Load the weapon datatables
		FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaMeleeWeapons.AthenaMeleeWeapons");
		FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaRangedWeapons.AthenaRangedWeapons");

		auto pItemDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>(cItemDef);
		auto pFortWeapon = m_pPlayerPawn->EquipWeaponDefinition(pItemDef, SDK::FGuid());

		pFortWeapon->SetOwner(m_pPlayerPawn);
		m_pPlayerPawn->AbilitySystemComponent->TryActivateAbilityByClass(pItemDef->GetPrimaryFireAbility(), false);
	}
}