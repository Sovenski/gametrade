// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTMarketTypes.h"
#include "GTMarketEconomySubsystem.generated.h"

/**
 * Victoria 3-Style Market Economy System
 *
 * Flow each turn:
 * 1. Buildings buy inputs from local market (create buy orders)
 * 2. Buildings produce outputs, sell to local market (create sell orders)
 * 3. Pops earn wages from buildings
 * 4. Pops buy goods from market with wages (create buy orders)
 * 5. Market clears orders (match buyers/sellers)
 * 6. Prices adjust based on unmet demand/unsold supply
 * 7. Trade routes equalize prices between markets
 *
 * Key Concept: Each settlement is a LOCAL MARKET with its own prices for ALL goods
 */
UCLASS()
class GAMETRADE_API UGTMarketEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========== INITIALIZATION ==========

	/** Initialize all markets and create default production recipes */
	UFUNCTION(BlueprintCallable, Category = "Market Economy")
	void InitializeMarketEconomy();

	/** Create a local market for a settlement */
	UFUNCTION(BlueprintCallable, Category = "Market")
	void CreateMarket(const FString& MarketName, float InitialSize);

	/** Initialize production recipes for all building types */
	void InitializeProductionRecipes();

	/** Initialize pop needs templates for all classes */
	void InitializePopNeeds();

	// ========== TURN PROCESSING ==========

	/** Process entire market economy for one turn */
	UFUNCTION(BlueprintCallable, Category = "Market Economy")
	void ProcessMarketEconomyTurn();

	// ========== PRODUCTION PHASE ==========

	/** Buildings attempt to buy inputs from market */
	UFUNCTION(BlueprintCallable, Category = "Production")
	void ProcessBuildingInputPurchases();

	/** Buildings produce goods and sell outputs to market */
	UFUNCTION(BlueprintCallable, Category = "Production")
	void ProcessBuildingProduction();

	/** Calculate building profitability */
	UFUNCTION(BlueprintCallable, Category = "Production")
	float CalculateBuildingProfit(const FString& BuildingID);

	// ========== POP PHASE ==========

	/** Pops receive wages from their employers */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	void ProcessPopWages();

	/** Pops attempt to buy goods from market */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	void ProcessPopConsumption();

	/** Calculate pop needs satisfaction */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	float CalculatePopSatisfaction(const FString& PopID);

	/** Update pop militancy based on satisfaction */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	void UpdatePopMilitancy();

	// ========== MARKET CLEARING ==========

	/** Match buy and sell orders in market */
	UFUNCTION(BlueprintCallable, Category = "Market")
	void ClearMarketOrders(const FString& MarketName);

	/** Adjust prices based on unmet demand/unsold supply */
	UFUNCTION(BlueprintCallable, Category = "Market")
	void AdjustMarketPrices(const FString& MarketName);

	// ========== TRADE BETWEEN MARKETS ==========

	/** Goods flow between markets to equalize prices */
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void ProcessInterMarketTrade();

	/** Calculate price difference for arbitrage */
	UFUNCTION(BlueprintCallable, Category = "Trade")
	float CalculatePriceDifference(const FString& MarketA, const FString& MarketB,
		EResourceType Resource);

	/** Move goods from one market to another */
	UFUNCTION(BlueprintCallable, Category = "Trade")
	bool TransferGoodsBetweenMarkets(const FString& FromMarket, const FString& ToMarket,
		EResourceType Resource, int32 Amount);

	// ========== BUILDING MANAGEMENT ==========

	/** Player constructs a new building in a market */
	UFUNCTION(BlueprintCallable, Category = "Buildings")
	FString ConstructBuilding(const FString& MarketName, EBuildingType_Market BuildingType,
		const FString& OwnerID, float& OutConstructionCost);

	/** Assign pops to work in building */
	UFUNCTION(BlueprintCallable, Category = "Buildings")
	bool AssignWorkersToBuilding(const FString& BuildingID, int32 NumWorkers);

	/** Get building by ID (C++ only, returns pointer) */
	FMarketBuilding* GetBuilding(const FString& BuildingID);

	// ========== POP MANAGEMENT ==========

	/** Create a new pop group in a market */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	FString CreatePopGroup(const FString& MarketName, EPopClass PopClass, int32 Size);

	/** Get pop by ID (C++ only, returns pointer) */
	FPopGroup* GetPop(const FString& PopID);

	/** Promote pop to higher class (craftsman → merchant) */
	UFUNCTION(BlueprintCallable, Category = "Pops")
	bool PromotePop(const FString& PopID, EPopClass NewClass);

	// ========== MARKET QUERIES ==========

	/** Get market by name (C++ only, returns pointer) */
	FLocalMarket* GetMarket(const FString& MarketName);

	/** Get current price of good in market */
	UFUNCTION(BlueprintCallable, Category = "Market")
	float GetMarketPrice(const FString& MarketName, EResourceType Resource);

	/** Get available supply of good in market */
	UFUNCTION(BlueprintCallable, Category = "Market")
	int32 GetMarketSupply(const FString& MarketName, EResourceType Resource);

	/** Get total demand for good in market */
	UFUNCTION(BlueprintCallable, Category = "Market")
	int32 GetMarketDemand(const FString& MarketName, EResourceType Resource);

	// ========== DEBUGGING ==========

	/** Print market state for debugging */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintMarketState(const FString& MarketName);

	/** Print building economics */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintBuildingEconomics(const FString& BuildingID);

private:
	// ========== DATA STORAGE ==========

	/** All local markets (one per settlement) */
	UPROPERTY()
	TMap<FString, FLocalMarket> AllMarkets;

	/** All buildings (industries) */
	UPROPERTY()
	TMap<FString, FMarketBuilding> AllBuildings;

	/** All pop groups */
	UPROPERTY()
	TMap<FString, FPopGroup> AllPops;

	/** Trade routes connecting markets */
	UPROPERTY()
	TArray<FMarketTradeRoute> TradeRoutes;

	/** Production recipes (templates for each building type) */
	UPROPERTY()
	TMap<EBuildingType_Market, FProductionRecipe> ProductionRecipes;

	/** Pop needs (templates for each class) */
	UPROPERTY()
	TMap<EPopClass, FPopNeedsTemplate> PopNeedsTemplates;

	// ========== HELPER FUNCTIONS ==========

	/** Create buy order for building */
	void CreateBuyOrder(const FString& MarketName, EResourceType Resource, int32 Amount,
		const FString& BuyerID);

	/** Create sell order for building */
	void CreateSellOrder(const FString& MarketName, EResourceType Resource, int32 Amount,
		const FString& SellerID);

	/** Get production recipe for building type */
	FProductionRecipe GetRecipe(EBuildingType_Market BuildingType);

	/** Get pop needs for class */
	FPopNeedsTemplate GetPopNeeds(EPopClass PopClass);

	/** Calculate base price for resource */
	float GetBasePrice(EResourceType Resource);

	/** Calculate supply/demand ratio in market */
	float GetSupplyDemandRatio(const FString& MarketName, EResourceType Resource);

	/** Check if building can afford inputs */
	bool CanAffordInputs(const FMarketBuilding& Building, const FLocalMarket& Market);

	/** Calculate wages for pop class in market */
	float CalculateWage(EPopClass PopClass, const FString& MarketName);

	/** Find available workers for building */
	TArray<FString> FindAvailableWorkers(const FString& MarketName, int32 NumNeeded);
};
