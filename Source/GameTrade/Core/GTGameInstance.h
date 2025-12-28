// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GTSaveGame.h"
#include "GTGameInstance.generated.h"

/**
 * Game Instance - Persists across level loads
 * Manages save/load and global game state
 */
UCLASS()
class GAMETRADE_API UGTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// Save/Load
	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool SaveGame(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool LoadGame(const FString& SlotName);

	UFUNCTION(BlueprintPure, Category = "Save System")
	bool DoesSaveGameExist(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	TArray<FString> GetAllSaveSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool DeleteSaveGame(const FString& SlotName);

	// New game
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNewGame(const FString& FamilyName, const FString& StartingSettlement);

	// Current save
	UFUNCTION(BlueprintPure, Category = "Save System")
	UGTSaveGame* GetCurrentSave() const { return CurrentSaveGame; }

protected:
	UPROPERTY()
	UGTSaveGame* CurrentSaveGame;

	// Auto-save
	void AutoSave();
	FTimerHandle AutoSaveTimerHandle;

	const FString AUTO_SAVE_SLOT = TEXT("AutoSave");
	const float AUTO_SAVE_INTERVAL = 300.0f; // 5 minutes
};
