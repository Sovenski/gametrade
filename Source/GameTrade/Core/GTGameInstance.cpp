// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Core/GTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/GTTimeSubsystem.h"
#include "Systems/GTEconomySubsystem.h"

void UGTGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Log, TEXT("GTGameInstance Initialized"));

	// Set up auto-save timer
	GetTimerManager().SetTimer(AutoSaveTimerHandle, this, &UGTGameInstance::AutoSave, AUTO_SAVE_INTERVAL, true);
}

void UGTGameInstance::StartNewGame(const FString& FamilyName, const FString& StartingSettlement)
{
	// Create new save game
	CurrentSaveGame = Cast<UGTSaveGame>(UGameplayStatics::CreateSaveGameObject(UGTSaveGame::StaticClass()));

	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create save game object!"));
		return;
	}

	// Initialize save data
	CurrentSaveGame->SaveDateTime = FDateTime::Now();
	CurrentSaveGame->PlayerFamilyName = FamilyName;
	CurrentSaveGame->Gold = 50.0f; // Starting gold
	CurrentSaveGame->CurrentDate.Year = 1347;
	CurrentSaveGame->CurrentDate.Week = 1;
	CurrentSaveGame->CurrentDate.Season = ESeason::Spring;

	// Create player character
	FCharacterData PlayerCharacter;
	PlayerCharacter.CharacterID = FGuid::NewGuid().ToString();
	PlayerCharacter.FirstName = TEXT("Hans"); // TODO: Get from UI
	PlayerCharacter.FamilyName = FamilyName;
	PlayerCharacter.bIsMale = true;
	PlayerCharacter.Age = 27;
	PlayerCharacter.Stats.Commerce = 2;
	PlayerCharacter.Stats.Stewardship = 1;
	PlayerCharacter.Stats.Intrigue = 1;
	PlayerCharacter.Stats.Leadership = 1;
	PlayerCharacter.Stats.Combat = 1;
	PlayerCharacter.bIsAlive = true;
	PlayerCharacter.CurrentSettlement = StartingSettlement;

	CurrentSaveGame->PlayerCharacterID = PlayerCharacter.CharacterID;
	CurrentSaveGame->Characters.Add(PlayerCharacter.CharacterID, PlayerCharacter);
	CurrentSaveGame->FamilyMemberIDs.Add(PlayerCharacter.CharacterID);

	UE_LOG(LogTemp, Log, TEXT("New game started: %s family in %s"), *FamilyName, *StartingSettlement);

	// Initialize subsystems
	UGTTimeSubsystem* TimeSubsystem = GetSubsystem<UGTTimeSubsystem>();
	UGTEconomySubsystem* EconomySubsystem = GetSubsystem<UGTEconomySubsystem>();

	// Auto-save
	SaveGame(TEXT("NewGame"));
}

bool UGTGameInstance::SaveGame(const FString& SlotName)
{
	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("No save game to save!"));
		return false;
	}

	// Update save metadata
	CurrentSaveGame->SaveSlotName = SlotName;
	CurrentSaveGame->SaveDateTime = FDateTime::Now();

	// Get data from subsystems
	UGTTimeSubsystem* TimeSubsystem = GetSubsystem<UGTTimeSubsystem>();
	UGTEconomySubsystem* EconomySubsystem = GetSubsystem<UGTEconomySubsystem>();

	if (TimeSubsystem)
	{
		CurrentSaveGame->CurrentDate = TimeSubsystem->GetCurrentDate();
	}

	if (EconomySubsystem)
	{
		// TODO: Save economy state
		// CurrentSaveGame->Settlements = EconomySubsystem->GetAllSettlements();
	}

	// Perform save
	bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Game saved to slot: %s"), *SlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot: %s"), *SlotName);
	}

	return bSuccess;
}

bool UGTGameInstance::LoadGame(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Save game does not exist: %s"), *SlotName);
		return false;
	}

	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
	CurrentSaveGame = Cast<UGTSaveGame>(LoadedGame);

	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load save game: %s"), *SlotName);
		return false;
	}

	// Restore state to subsystems
	UGTTimeSubsystem* TimeSubsystem = GetSubsystem<UGTTimeSubsystem>();
	UGTEconomySubsystem* EconomySubsystem = GetSubsystem<UGTEconomySubsystem>();

	if (TimeSubsystem)
	{
		// TODO: Restore time
	}

	if (EconomySubsystem)
	{
		// TODO: Restore economy
	}

	UE_LOG(LogTemp, Log, TEXT("Game loaded from slot: %s"), *SlotName);
	return true;
}

bool UGTGameInstance::DoesSaveGameExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

TArray<FString> UGTGameInstance::GetAllSaveSlots() const
{
	TArray<FString> SaveSlots;

	// Check for common save slots
	for (int32 i = 1; i <= 5; i++)
	{
		FString SlotName = FString::Printf(TEXT("SaveSlot%d"), i);
		if (DoesSaveGameExist(SlotName))
		{
			SaveSlots.Add(SlotName);
		}
	}

	// Check for auto-save
	if (DoesSaveGameExist(AUTO_SAVE_SLOT))
	{
		SaveSlots.Add(AUTO_SAVE_SLOT);
	}

	return SaveSlots;
}

bool UGTGameInstance::DeleteSaveGame(const FString& SlotName)
{
	bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Deleted save game: %s"), *SlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to delete save game: %s"), *SlotName);
	}

	return bSuccess;
}

void UGTGameInstance::AutoSave()
{
	if (CurrentSaveGame)
	{
		SaveGame(AUTO_SAVE_SLOT);
		UE_LOG(LogTemp, Log, TEXT("Auto-saved game"));
	}
}
