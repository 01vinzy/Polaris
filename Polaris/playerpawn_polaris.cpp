#include "playerpawn_polaris.h"

namespace polaris
{
	PlayerPawnPolaris::PlayerPawnPolaris()
	{
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