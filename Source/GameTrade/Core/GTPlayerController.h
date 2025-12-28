// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/GTDataTypes.h"
#include "GTPlayerController.generated.h"

/**
 * Player Controller for Medieval Trading Dynasty
 * Handles player input and UI interaction
 */
UCLASS()
class GAMETRADE_API AGTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGTPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Player actions
	UFUNCTION(BlueprintCallable, Category = "Player")
	bool TradeGoods(const FString& Settlement, EResourceType Resource, int32 Amount, bool bBuying);

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool SendCaravan(const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& Cargo);

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool BuildBuilding(const FString& Settlement, EBuildingType BuildingType);

	// Player state
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetPlayerGold() const;

	UFUNCTION(BlueprintPure, Category = "Player")
	FCharacterData GetPlayerCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Player")
	TArray<FResourceAmount> GetPlayerInventory() const;

	// UI
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowTradeWindow(const FString& SettlementName);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowFamilyTree();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowProductionOverview();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowEventPopup(const FString& EventTitle, const FString& EventDescription);

protected:
	// Input handling
	void OnNextTurnPressed();
	void OnPausePressed();
	void OnOpenMapPressed();

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	FString CurrentSettlement;
};
