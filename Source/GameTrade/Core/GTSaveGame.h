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
	UPROPERTY(SaveGame, Category = "Game State")
	FGameDate CurrentDate;

	UPROPERTY(SaveGame, Category = "Game State")
	FString PlayerFamilyID;

	UPROPERTY(SaveGame, Category = "Game State")
	FString PlayerCharacterID;

	// Economy state
	UPROPERTY(SaveGame, Category = "Economy")
	TMap<FString, FSettlementData> Settlements;

	// Family & Characters
	UPROPERTY(SaveGame, Category = "Dynasty")
	TMap<FString, FCharacterData> Characters;

	UPROPERTY(SaveGame, Category = "Dynasty")
	TArray<FString> FamilyMemberIDs; // All members of player's family

	// Buildings & Production
	UPROPERTY(SaveGame, Category = "Economy")
	TMap<FString, FBuildingData> Buildings;

	// Trade
	UPROPERTY(SaveGame, Category = "Trade")
	TMap<FString, FCaravanData> Caravans;

	// Player resources
	UPROPERTY(SaveGame, Category = "Player")
	float Gold;

	UPROPERTY(SaveGame, Category = "Player")
	TArray<FResourceAmount> Inventory;

	// Statistics
	UPROPERTY(SaveGame, Category = "Stats")
	int32 TotalTurnsPlayed;

	UPROPERTY(SaveGame, Category = "Stats")
	float TotalGoldEarned;

	UPROPERTY(SaveGame, Category = "Stats")
	int32 TotalTradesMade;

	UPROPERTY(SaveGame, Category = "Stats")
	int32 BuildingsOwned;
};
