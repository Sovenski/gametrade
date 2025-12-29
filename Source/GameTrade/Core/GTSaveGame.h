// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/GTDataTypes.h"
#include "GTSaveGame.generated.h"

/**
 * Save game data structure
 * Contains all persistent game state
 */
UCLASS()
class GAMETRADE_API UGTSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UGTSaveGame();

	// Save metadata
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	int32 SaveIndex;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	FDateTime SaveDateTime;

	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	FString PlayerFamilyName;

	// Game state
	UPROPERTY(SaveGame)
	FGameDate CurrentDate;

	UPROPERTY(SaveGame)
	FString PlayerFamilyID;

	UPROPERTY(SaveGame)
	FString PlayerCharacterID;

	// Economy state
	UPROPERTY(SaveGame)
	TMap<FString, FSettlementData> Settlements;

	// Family & Characters
	UPROPERTY(SaveGame)
	TMap<FString, FCharacterData> Characters;

	UPROPERTY(SaveGame)
	TArray<FString> FamilyMemberIDs; // All members of player's family

	// Buildings & Production
	UPROPERTY(SaveGame)
	TMap<FString, FBuildingData> Buildings;

	// Trade
	UPROPERTY(SaveGame)
	TMap<FString, FCaravanData> Caravans;

	// Player resources
	UPROPERTY(SaveGame)
	float Gold;

	UPROPERTY(SaveGame)
	TArray<FResourceAmount> Inventory;

	// Statistics
	UPROPERTY(SaveGame)
	int32 TotalTurnsPlayed;

	UPROPERTY(SaveGame)
	float TotalGoldEarned;

	UPROPERTY(SaveGame)
	int32 TotalTradesMade;

	UPROPERTY(SaveGame)
	int32 BuildingsOwned;
};
