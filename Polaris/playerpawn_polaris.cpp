#include "playerpawn_polaris.h"

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
		StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(Util::BaseAddress() + 0x142E560); //stay blessed

		// Summon a new PlayerPawn.
		std::string sPawnClassName = "PlayerPawn_Athena_C";
		Core::pPlayerController->CheatManager->Summon(SDK::FString(std::wstring(sPawnClassName.begin(), sPawnClassName.end()).c_str()));

		// Summon a new FortMeleeWeapon.
		std::string sMeleeClassName = "B_Melee_Impact_Pickaxe_Athena_C";
		Core::pPlayerController->CheatManager->Summon(SDK::FString(std::wstring(sMeleeClassName.begin(), sMeleeClassName.end()).c_str()));

		pPawn = static_cast<SDK::APlayerPawn_Athena_C*>(Util::FindActor(SDK::APlayerPawn_Athena_C::StaticClass()));
		auto pFortWeapon = static_cast<SDK::AFortWeapon*>(Util::FindActor(SDK::AFortWeapon::StaticClass()));
		auto pFortWeaponMesh = SDK::UObject::FindObject<SDK::USkeletalMesh>("SkeletalMesh SK_Pick.SK_Pick");

		if (!pPawn)
		{
			MessageBox(0, L"Failed to spawn PlayerPawn_Athena_C.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		else
		{
			Util::Possess(pPawn);

			// Equip the newly spawned FortWeapon.
			if (!pFortWeapon)
			{
				MessageBox(0, L"Failed to spawn AFortWeapon.", L"Error", MB_ICONERROR);
				ExitProcess(EXIT_FAILURE);
			}
			else
			{

				pFortWeapon->WeaponMesh->SetSkeletalMesh(pFortWeaponMesh, true);
				EquipWeapon(pFortWeapon);
			}

			// Assign our new pawn to a team.
			static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->TeamIndex = SDK::EFortTeam::HumanPvP_Team1;
			static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->OnRep_TeamIndex();
		}
	}
	
	// FIXME: (irma) Make this better.
	void PlayerPawnPolaris::InitializeHero()
	{
		auto pCustomCharacterPartHead = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Head");
		auto pCustomCharacterPartBody = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Body");
		auto pCustomCharacterPartHat = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Hat_");

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[0] = pCustomCharacterPartHead;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[1] = pCustomCharacterPartBody;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[3] = pCustomCharacterPartHat;

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->OnRep_CharacterParts();
	}

	void PlayerPawnPolaris::EquipWeapon(SDK::AFortWeapon* weapon)
	{
		auto BuildingTool = FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Weapons/FORT_BuildingTools/Blueprints/DefaultBuildingTool.DefaultBuildingTool_C");
		if (BuildingTool)
			printf("LOADING OBJECT WORKED!!!");
		auto Shotgun = FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Weapons/FORT_Shotguns/Blueprints/B_Shotgun_Standard_Athena.B_Shotgun_Standard_Athena_C");
		if (Shotgun)
			printf("LOADING OBJECT WORKED!!!");

		weapon->ClientGivenTo(pPawn);
		pPawn->ClientInternalEquipWeapon(weapon);
	}

	/*void PlayerPawnPolaris::SetSkin(Skin* newSkin)
	{
		if (newSkin == nullptr)
		{
			Console::LogError("Trying to set skin to nullptr!");
			return;
		}

		pMySkin = newSkin;
		auto pCustomCharacterPartHead = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", newSkin->sBodyParts[0]);
		auto pCustomCharacterPartBody = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", newSkin->sBodyParts[1]);
		auto pCustomCharacterPartHat = Util::SearchObject<SDK::UCustomCharacterPart>("CustomCharacterPart", newSkin->sBodyParts[2]);

		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[0] = pCustomCharacterPartHead;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[1] = pCustomCharacterPartBody;
		static_cast<SDK::AFortPlayerStateAthena*>(Core::pPlayerController->PlayerState)->CharacterParts[2] = pCustomCharacterPartHat;
	}

	void PlayerPawnPolaris::SetSkin(std::string query)
	{
		auto pActualSkin = pSkinDB->FindSkin(query);
		
		if (pActualSkin == nullptr)
		{
			MessageBox(0, L"Failed load skin.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}
		else
		{
			SetSkin(pActualSkin);
		}
	}*/
}