#pragma warning(disable:26495)

#include "athena.h"

namespace polaris
{
	bool bPreloadWeapons = false;
	static SDK::UObject* (*StaticLoadObject)(SDK::UClass* ObjectClass, SDK::UObject* InOuter, const TCHAR* InName, const TCHAR* Filename, uint32_t LoadFlags, SDK::UPackageMap* Sandbox, bool bAllowObjectReconciliation);
	template<class T>
	static T* LoadObject(SDK::UObject* Outer, const TCHAR* Name, const TCHAR* Filename = nullptr, uint32_t LoadFlags = 0, SDK::UPackageMap* Sandbox = nullptr)
	{
		return (T*)StaticLoadObject(T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox, true);
	}
	template<typename T>
	static T* FindOrLoadObject(const std::string PathName)
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

	// Listens to functions called in game, use this to hook code to functions in question.
	PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;
	PVOID ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
	{

		if (pObject && pFunction)
		{
			// Hooks for Frontend
			if (gpAthena->m_bIsInFrontend)
			{
				// Load Athena_Terrain once the player readies up.
				if (pFunction->GetName().find("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature") != std::string::npos)
				{
					gpAthena->m_bIsWaitingForLoadingScreen = true;
					Globals::gpPlayerController->SwitchLevel(TEXT("Athena_Terrain"));
				}

				// This gets called once the loading screen is ready to drop.
				// We reinitialize the SDK, Core and update memory to fix the abilities.
				if (pFunction->GetName().find("ReadyToStartMatch") != std::string::npos && gpAthena->m_bIsWaitingForLoadingScreen)
				{
					gpAthena->m_bIsInFrontend = false;
					gpAthena->m_bIsWaitingForLoadingScreen = false;

					// Reinitialize the core, there's missing references because of Frontend unloading.
					Util::InitSdk();
					Util::InitCore();
					Util::InitPatches();

					StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(Util::BaseAddress() + 0x142E560);
					//load le shit
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Weapons/FORT_BuildingTools/Blueprints/DefaultBuildingTool.DefaultBuildingTool_C");
					FindOrLoadObject<SDK::UDataTable>("/Game/Items/Datatables/AthenaTraps.AthenaTraps");
					FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaMeleeWeapons.AthenaMeleeWeapons");
					FindOrLoadObject<SDK::UDataTable>("/Game/Athena/Items/Weapons/AthenaRangedWeapons.AthenaRangedWeapons");
					FindOrLoadObject<SDK::UDataTable>("/Game/Items/Datatables/RangedWeapons.RangedWeapons");
					FindOrLoadObject<SDK::UDataTable>("/Game/Items/Datatables/MeleeWeapons.MeleeWeapons");
					FindOrLoadObject<SDK::UDataTable>("/Game/Items/Datatables/Traps.Traps");
					FindOrLoadObject<SDK::UCurveTable>("/Game/Items/Datatables/AthenaProjectiles.AthenaProjectiles");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn.HuskPawn_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Beehive.HuskPawn_Beehive_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell.HuskPawn_Bombshell_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell_Poison.HuskPawn_Bombshell_Poison_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf.HuskPawn_Dwarf_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Fire.HuskPawn_Dwarf_Fire_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Ice.HuskPawn_Dwarf_Ice_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Lightning.HuskPawn_Dwarf_Lightning_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Fire.HuskPawn_Fire_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Ice.HuskPawn_Ice_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Lightning.HuskPawn_Lightning_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Pitcher.HuskPawn_Pitcher_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Sploder.HuskPawn_Sploder_C");
					FindOrLoadObject<SDK::UBlueprintGeneratedClass>("/Game/Missions/Secondary/Mimic/HuskPawn_Mimic.HuskPawn_Mimic_C");

					if (!gpAthena->m_pPlayer)
					{
						// Create a new Player Pawn.
						gpAthena->m_pPlayer = new Player;
						gpAthena->m_pPlayer->InitializeHero();

						// Load preset item definitions (Prevents a hitch during weapon swapping)
						gpAthena->m_pHarvestingToolDefinition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>(gpAthena->m_pPlayer->m_pPlayerPawn->CustomizationLoadout.Character->GetFullName());
						gpAthena->m_pSlot2Definition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortWeaponRangedItemDefinition WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03");
						gpAthena->m_pSlot3Definition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortWeaponRangedItemDefinition WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");
						gpAthena->m_pSlot4Definition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortWeaponRangedItemDefinition WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03");
						gpAthena->m_pSlot5Definition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortWeaponRangedItemDefinition WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03");
						gpAthena->m_pSlot6Definition = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortWeaponRangedItemDefinition WID_Sniper_AMR_Athena_SR_Ore_T03.WID_Sniper_AMR_Athena_SR_Ore_T03");

						// load building tools from memory (pivotman)
						gpAthena->m_pEditToolDef = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");
						gpAthena->m_pWallBuildDef = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
						gpAthena->m_pFloorBuildDef = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");
						gpAthena->m_pStairBuildDef = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
						gpAthena->m_pRoofBuildDef = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");

						// Tell the client that we are ready to start the match, this allows the loading screen to drop.
						static_cast<SDK::AAthena_PlayerController_C*>(Globals::gpPlayerController)->ServerReadyToStartMatch();
						static_cast<SDK::AGameMode*>((*Globals::gpWorld)->AuthorityGameMode)->StartMatch();

						// Turn on basic cheats
						static_cast<SDK::UFortCheatManager*>(Globals::gpPlayerController->CheatManager)->ToggleInfiniteAmmo();

						auto minimapWidget = SDK::UObject::FindObject<SDK::UFortLegacySlateBridgeWidget>("FortLegacySlateBridgeWidget AthenaDeathWidget.WidgetArchetype.WidgetTree_1.MinimapContainer.WidgetTree_16.FortSlateWidgetMinimap");
						auto minimapBrush = SDK::UObject::FindObject<SDK::USlateBrushAsset>("SlateBrushAsset TutorialGlow_Brush.TutorialGlow_Brush");

						minimapWidget->Visibility = SDK::ESlateVisibility::Hidden;
						minimapWidget->UpdateSlateWidget(SDK::EFortLegacySlateWidget::AthenaMiniMap);
					}
				}
			}

			// Hooks for Athena_Terrain
			else
			{
				SDK::EFortQuickBars qbPrimary = SDK::EFortQuickBars::Primary;
				SDK::EFortQuickBars qbSecondary = SDK::EFortQuickBars::Secondary;
				if (pFunction->GetName() == "Fly") {
					switch (gpAthena->m_pPlayer->m_pPlayerPawn->CharacterMovement->MovementMode) {
					default:
						gpAthena->m_pPlayer->m_pPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Flying, 0);
						break;
					case SDK::EMovementMode::MOVE_Flying:
						gpAthena->m_pPlayer->m_pPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Walking, 0);
						break;
					}
				}
				// Called every frame.
				if (pFunction->GetName().find("Tick") != std::string::npos)
				{
					//Consume the cum chalice v2
					//Preload Default items to prevent hitch for equipping default items
					if (bPreloadWeapons == false) {
						SDK::FGuid guid;
						guid.A = 1;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot2Definition, guid);
						guid.A = 2;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot3Definition, guid);
						guid.A = 3;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot4Definition, guid);
						guid.A = 4;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot5Definition, guid);
						guid.A = 5;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot6Definition, guid);
						guid.A = 0;
						guid.B = 0;
						guid.C = 0;
						guid.D = 0;
						gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pHarvestingToolDefinition, guid);
						bPreloadWeapons = true;
					}
					if (gpAthena->m_pPlayer && gpAthena->m_pPlayer->m_pPlayerPawn && !static_cast<SDK::AFortPlayerControllerAthena*>(Globals::gpPlayerController)->IsInAircraft() && gpInventoryMapper != nullptr)
					{

						if (GetAsyncKeyState('1') & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 0;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;
							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pHarvestingToolDefinition, guid);
							// it will hitch the first time called but after it will befine
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 0); // changes quickbar focus
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 0);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1"); // do i exist
								if (temp) { // yes i exist
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1"); // set me
									}
								}
							}


						}
						if (GetAsyncKeyState('2') & 0x8000)
						{
							if (gpAthena->m_pSlot2Definition->GetFullName() != gpInventoryMapper->m_aInventoryItems[1].c_str())
							{
								auto newItemDef = SDK::UObject::FindObject<SDK::UFortWeaponRangedItemDefinition>(gpInventoryMapper->m_aInventoryItems[1]);

								gpAthena->m_pSlot2Definition = newItemDef;
								gpAthena->m_iSlot2Revision++;
							}

							SDK::FGuid guid;
							guid.A = 1;
							guid.B = gpAthena->m_iSlot2Revision;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot2Definition, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 1);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 1);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
							
						}
						if (GetAsyncKeyState('3') & 0x8000)
						{
							if (gpAthena->m_pSlot3Definition->GetFullName() != gpInventoryMapper->m_aInventoryItems[2].c_str())
							{
								auto newItemDef = SDK::UObject::FindObject<SDK::UFortWeaponRangedItemDefinition>(gpInventoryMapper->m_aInventoryItems[2]);

								gpAthena->m_pSlot3Definition = newItemDef;
								gpAthena->m_iSlot4Revision++;
							}

							SDK::FGuid guid;
							guid.A = 2;
							guid.B = gpAthena->m_iSlot3Revision;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot3Definition, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 2);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 2);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1"); 
									}
								}
							}

						}
						if (GetAsyncKeyState('4') & 0x8000)
						{
							if (gpAthena->m_pSlot4Definition->GetFullName() != gpInventoryMapper->m_aInventoryItems[3].c_str())
							{
								auto newItemDef = SDK::UObject::FindObject<SDK::UFortWeaponRangedItemDefinition>(gpInventoryMapper->m_aInventoryItems[3]);
								gpAthena->m_pSlot4Definition = newItemDef;
								gpAthena->m_iSlot4Revision++;
							}

							SDK::FGuid guid;
							guid.A = 3;
							guid.B = gpAthena->m_iSlot4Revision;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot4Definition, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 3);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 3);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
							
						}
						if (GetAsyncKeyState('5') & 0x8000)
						{
							{
								auto newItemDef = SDK::UObject::FindObject<SDK::UFortWeaponRangedItemDefinition>(gpInventoryMapper->m_aInventoryItems[4]);

								gpAthena->m_pSlot5Definition = newItemDef;
								gpAthena->m_iSlot5Revision++;
							}

							SDK::FGuid guid;
							guid.A = 4;
							guid.B = gpAthena->m_iSlot5Revision;
							guid.C = 0;
							guid.D = 0;

							if (gpAthena->m_pSlot5Definition->GetFullName() != gpInventoryMapper->m_aInventoryItems[4].c_str())
							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot5Definition, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 4);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 4);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
							
						}
						if (GetAsyncKeyState('6') & 0x8000)
						{
							if (gpAthena->m_pSlot6Definition->GetFullName() != gpInventoryMapper->m_aInventoryItems[5].c_str())
							{
								auto newItemDef = SDK::UObject::FindObject<SDK::UFortWeaponRangedItemDefinition>(gpInventoryMapper->m_aInventoryItems[5]);

								gpAthena->m_pSlot6Definition = newItemDef;
								gpAthena->m_iSlot6Revision++;
							}

							SDK::FGuid guid;
							guid.A = 5;
							guid.B = gpAthena->m_iSlot6Revision;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pSlot6Definition, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarPrimary->OnQuickbarSlotFocusChanged(qbPrimary, 5);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbPrimary, 5);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}

						// Building tool keybinds
						if (GetAsyncKeyState(VK_F1) & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 6;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pWallBuildDef, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarSecondary->OnQuickbarSlotFocusChanged(qbSecondary, 0);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbSecondary, 0);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}
						if (GetAsyncKeyState(VK_F2) & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 7;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pFloorBuildDef, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarSecondary->OnQuickbarSlotFocusChanged(qbSecondary, 1);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbSecondary, 1);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}
						if (GetAsyncKeyState(VK_F3) & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 8;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pStairBuildDef, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarSecondary->OnQuickbarSlotFocusChanged(qbSecondary, 2);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbSecondary, 2);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}
						if (GetAsyncKeyState(VK_F4) & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 9;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pRoofBuildDef, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarSecondary->OnQuickbarSlotFocusChanged(qbSecondary, 3);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbSecondary, 3);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}
						if (GetAsyncKeyState(VK_F5) & 0x8000)
						{
							SDK::FGuid guid;
							guid.A = 10;
							guid.B = 0;
							guid.C = 0;
							guid.D = 0;

							gpAthena->m_pPlayer->m_pPlayerPawn->EquipWeaponDefinition(gpAthena->m_pRoofBuildDef, guid);
							if (gpAthena->pAthenaHud) {
								gpAthena->pAthenaHud->QuickbarSecondary->OnQuickbarSlotFocusChanged(qbSecondary, 4);
								gpAthena->pAthenaHud->HandleQuickbarSlotFocusSlotChanged(qbSecondary, 4);
							}
							else {
								auto temp = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
								if (temp) {
									if (!gpAthena->pAthenaHud) {
										gpAthena->pAthenaHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
									}
								}
							}
						}

						if (GetAsyncKeyState(VK_END) & 0x8000 && !gpAthena->m_bGameOver)
						{
							gpAthena->m_bGameOver = true;
							static_cast<SDK::AFortPlayerControllerAthena*>(Globals::gpPlayerController)->ClientNotifyWon();
							static_cast<SDK::AFortPlayerControllerAthena*>(Globals::gpPlayerController)->PlayWinEffects();
						}
					}
				}

				// Called once the player jumps from the battle bus, or when they're supposed to be kicked out.
				if (pFunction->GetName().find("ServerAttemptAircraftJump") != std::string::npos || 
					pFunction->GetName().find("OnAircraftExitedDropZone") != std::string::npos)
				{
					if (static_cast<SDK::AFortPlayerControllerAthena*>(Globals::gpPlayerController)->IsInAircraft())
					{
						//Destroy all playerpawns.
						Globals::gpPlayerController->CheatManager->DestroyPawns(SDK::APlayerPawn_Athena_C::StaticClass());
						// Create a new player pawn.
						gpAthena->m_pPlayer = new Player;
						gpAthena->m_pPlayer->InitializeHero(); // This causes a massive hitch.

						// Reset the pawn rotation, due to weird summon properties.
						SDK::AFortPlayerPawnAthena* playerPawn = gpAthena->m_pPlayer->m_pPlayerPawn;
						SDK::FRotator actorRotation = gpAthena->m_pPlayer->m_pPlayerPawn->K2_GetActorRotation();

						actorRotation.Pitch = 0;
						actorRotation.Roll = 0;
						playerPawn->K2_SetActorLocationAndRotation(playerPawn->K2_GetActorLocation(), actorRotation, false, true, new SDK::FHitResult());

						// Don't return null if the aircraft exited drop zone, or the storm won't start.
						if (pFunction->GetName().find("OnAircraftExitedDropZone") == std::string::npos)
							return NULL;
					}
				}
			}
		}

		return ProcessEvent(pObject, pFunction, pParams);
	}

	// Update loop, runs at 60hz, 1 tick every 16ms.
	DWORD UpdateThread(LPVOID lpParam)
	{
		while (1)
		{
			if (gpAthena->m_pPlayer != nullptr && gpAthena->m_pPlayer->m_pPlayerPawn && !static_cast<SDK::AFortPlayerControllerAthena*>(Globals::gpPlayerController)->IsInAircraft())
			{
				// Keybind to jump (only run if not skydiving, might need to fix this more though):
				if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !gpAthena->m_pPlayer->m_pPlayerPawn->IsSkydiving() && !gpAthena->m_pPlayer->m_pPlayerPawn->IsJumpProvidingForce())
				{
					gpAthena->m_pPlayer->m_pPlayerPawn->Jump();
					Sleep(200);
				}
				else if (GetAsyncKeyState(VK_SPACE) & 0x8000 && gpAthena->m_pPlayer->m_pPlayerPawn->IsSkydiving() && !gpAthena->m_pPlayer->m_pPlayerPawn->IsParachuteOpen() && !gpAthena->m_pPlayer->m_pPlayerPawn->IsParachuteForcedOpen())
				{
					gpAthena->m_pPlayer->m_pPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 2U);
					gpAthena->m_pPlayer->m_pPlayerPawn->OnRep_IsParachuteOpen(false);
					Sleep(200);
				}
				else if (GetAsyncKeyState(VK_SPACE) & 0x8000 && gpAthena->m_pPlayer->m_pPlayerPawn->IsSkydiving() && gpAthena->m_pPlayer->m_pPlayerPawn->IsParachuteOpen() && !gpAthena->m_pPlayer->m_pPlayerPawn->IsParachuteForcedOpen())
				{
					gpAthena->m_pPlayer->m_pPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 3U);
					gpAthena->m_pPlayer->m_pPlayerPawn->OnRep_IsParachuteOpen(true);
					Sleep(200);
				}

				// Keybind to sprint (only run if not skydiving & not targeting, else walk):
				if (static_cast<SDK::AAthena_PlayerController_C*>(Globals::gpPlayerController)->bWantsToSprint && gpAthena->m_pPlayer->m_pPlayerPawn && gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon && !gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon->bIsTargeting)
				{
					// If reloading, only allow sprinting if Sprinting Cancels Reload is on.
					if (gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon->IsReloading())
					{
						if (static_cast<SDK::AAthena_PlayerController_C*>(Globals::gpPlayerController)->bSprintCancelsReload)
						{
							gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon->bIsReloadingWeapon = false;
							gpAthena->m_pPlayer->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
						}
					}
					else
						gpAthena->m_pPlayer->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
				}
				else if (gpAthena->m_pPlayer->m_pPlayerPawn && gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon && !gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon->bIsTargeting)
					gpAthena->m_pPlayer->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Running;
				else if (gpAthena->m_pPlayer->m_pPlayerPawn && gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon && gpAthena->m_pPlayer->m_pPlayerPawn->CurrentWeapon->bIsTargeting)
					gpAthena->m_pPlayer->m_pPlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Walking;
			}

			// Update thread only runs at 60hz, so we don't rape CPUs.
			Sleep(1000 / 60);
		}

		return NULL;
	}

	// Initializes the ProcessEventHook and starts the Update thread.
	DWORD WINAPI LoaderThread(LPVOID lpParam)
	{
		Util::InitSdk();
		Util::InitCore();

		auto pProcessEventAddress = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
		if (!pProcessEventAddress)
			Util::ThrowFatalError(L"Finding pattern for ProcessEvent has failed. Please relaunch Fortnite and try again!");

		MH_CreateHook(static_cast<LPVOID>(pProcessEventAddress), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
		MH_EnableHook(static_cast<LPVOID>(pProcessEventAddress));

		CreateThread(0, 0, UpdateThread, 0, 0, 0); // Create thread to handle input, etc...

		return FALSE;
	}

	Athena::Athena()
	{
		// Athena is basically a singleton, so we can't have it more than once.
		if (gpAthena)
			Util::ThrowFatalError(L"Athena is already initialized!");

		gpAthena = this;
		CreateThread(0, 0, LoaderThread, 0, 0, 0);
	}
}
