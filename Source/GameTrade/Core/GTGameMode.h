// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GTGameMode.generated.h"

/**
 * Main Game Mode for Medieval Trading Dynasty
 * Manages gameplay loop and turn progression
 */
UCLASS()
class GAMETRADE_API AGTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGTGameMode();

	virtual void BeginPlay() override;

	// Turn management
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NextTurn();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void FastForward(int32 NumTurns);

	// Game state
	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsGamePaused() const { return bGamePaused; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResumeGame();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	bool bGamePaused;

	// Turn callbacks
	UFUNCTION()
	void OnTurnAdvanced(const FGameDate& NewDate);

	UFUNCTION()
	void OnSeasonChanged(ESeason NewSeason);

	UFUNCTION()
	void OnYearChanged(int32 NewYear);
};
