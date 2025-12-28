// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTDataTypes.h"
#include "GTTimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnAdvanced, const FGameDate&, NewDate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, ESeason, NewSeason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnYearChanged, int32, NewYear);

/**
 * Manages the turn-based time system
 * 1 Turn = 1 Week
 * 52 Weeks = 1 Year
 */
UCLASS()
class GAMETRADE_API UGTTimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Time advancement
	UFUNCTION(BlueprintCallable, Category = "Time")
	void AdvanceTurn();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void AdvanceMultipleTurns(int32 NumTurns);

	// Getters
	UFUNCTION(BlueprintPure, Category = "Time")
	FGameDate GetCurrentDate() const { return CurrentDate; }

	UFUNCTION(BlueprintPure, Category = "Time")
	int32 GetCurrentYear() const { return CurrentDate.Year; }

	UFUNCTION(BlueprintPure, Category = "Time")
	int32 GetCurrentWeek() const { return CurrentDate.Week; }

	UFUNCTION(BlueprintPure, Category = "Time")
	ESeason GetCurrentSeason() const { return CurrentDate.Season; }

	UFUNCTION(BlueprintPure, Category = "Time")
	FString GetDateString() const { return CurrentDate.ToString(); }

	// Season effects
	UFUNCTION(BlueprintPure, Category = "Time")
	float GetSeasonalPriceModifier(EResourceType ResourceType) const;

	UFUNCTION(BlueprintPure, Category = "Time")
	float GetSeasonalTravelTimeModifier() const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnTurnAdvanced OnTurnAdvanced;

	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnSeasonChanged OnSeasonChanged;

	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnYearChanged OnYearChanged;

protected:
	UPROPERTY()
	FGameDate CurrentDate;

	ESeason LastSeason;
};
