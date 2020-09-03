#include "playerpawn_polaris.h"
#include "console.h"

namespace polaris
{
	SDK::UObject* (*StaticLoadObject)(SDK::UClass* ObjectClass, SDK::UObject* InOuter, const TCHAR* InName, const TCHAR* Filename, uint32_t LoadFlags, SDK::UPackageMap* Sandbox, bool bAllowObjectReconciliation) = nullptr;

	template<class T>
	T* LoadObject(SDK::UObject* Outer, const TCHAR* Name, const TCHAR* Filename = nullptr, uint32_t LoadFlags = 0, SDK::UPackageMap* Sandbox = nullptr)
	{
		return (T*)StaticLoadObject(T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox, true);
	}

	template<typename T>
	T* FindOrLoadObject(const std::string PathName)
	{
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
		StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(Util::BaseAddress() + 0x142E560);

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

			EquipWeapon("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		}
	}
	
	// FIXME(irma): Make better.
	void PlayerPawnPolaris::InitializeHero()
	{
		auto pCustomCharacterPartHead = Util::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Head");
		auto pCustomCharacterPartBody = Util::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Body");
		auto pCustomCharacterPartHat = Util::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Hat_");

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[0] = pCustomCharacterPartHead;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[1] = pCustomCharacterPartBody;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[3] = pCustomCharacterPartHat;

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->OnRep_CharacterParts();
		m_pPlayerPawn->OnCharacterPartsReinitialized();
	}

	void PlayerPawnPolaris::EquipWeapon(const char* cItemDef)
	{
		FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaMeleeWeapons.AthenaMeleeWeapons");
		FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaRangedWeapons.AthenaRangedWeapons");

		auto pItemDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>(cItemDef);

		m_pPlayerPawn->EquipWeaponDefinition(pItemDef, SDK::FGuid())->SetOwner(m_pPlayerPawn);
		m_pPlayerPawn->AbilitySystemComponent->TryActivateAbilityByClass(pItemDef->GetPrimaryFireAbility(), false);
	}
}