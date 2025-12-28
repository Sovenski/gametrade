// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTDataTypes.h"
#include "Engine/DataTable.h"
#include "GTEconomySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPriceChanged, FString, SettlementName, EResourceType, ResourceType, float, NewPrice);

/**
 * Manages the economy simulation
 * - Dynamic pricing based on supply/demand
 * - Production and consumption
 * - Trade between settlements
 */
UCLASS()
class GAMETRADE_API UGTEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Initialization
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void LoadSettlementsFromDataTable(UDataTable* SettlementTable);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void InitializeEconomy();

	// Turn processing
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void ProcessEconomyTurn();

	// Price queries
	UFUNCTION(BlueprintPure, Category = "Economy")
	float GetPrice(const FString& SettlementName, EResourceType ResourceType) const;

	UFUNCTION(BlueprintPure, Category = "Economy")
	FPriceInfo GetPriceInfo(const FString& SettlementName, EResourceType ResourceType) const;

	// Trading
	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool BuyGoods(const FString& SettlementName, EResourceType ResourceType, int32 Amount, float& OutTotalCost);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool SellGoods(const FString& SettlementName, EResourceType ResourceType, int32 Amount, float& OutTotalRevenue);

	// Settlement queries
	UFUNCTION(BlueprintPure, Category = "Economy")
	TArray<FString> GetAllSettlementNames() const;

	UFUNCTION(BlueprintPure, Category = "Economy")
	FSettlementData GetSettlementData(const FString& SettlementName) const;

	UFUNCTION(BlueprintPure, Category = "Economy")
	bool DoesSettlementProduce(const FString& SettlementName, EResourceType ResourceType) const;

	UFUNCTION(BlueprintPure, Category = "Economy")
	bool DoesSettlementNeed(const FString& SettlementName, EResourceType ResourceType) const;

	// Travel
	UFUNCTION(BlueprintPure, Category = "Economy")
	int32 CalculateTravelTime(const FString& FromSettlement, const FString& ToSettlement) const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnPriceChanged OnPriceChanged;

protected:
	// Simulation functions
	void UpdateProduction();
	void UpdateConsumption();
	void UpdatePrices();
	float CalculateBasePrice(EResourceType ResourceType) const;
	void ApplySeasonalModifiers();

	// Data
	UPROPERTY()
	TMap<FString, FSettlementData> Settlements;

	UPROPERTY()
	TMap<EResourceType, float> BasePrices;

	// Constants
	const float PRICE_ELASTICITY = 0.3f; // How much supply/demand affects price
	const int32 BASE_PRODUCTION_PER_WEEK = 100;
	const int32 BASE_CONSUMPTION_PER_1000_POP = 10;
};
