// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTHierarchyTypes.h"
#include "Data/GTDataTypes.h"
#include "GTHierarchicalEconomySubsystem.generated.h"

/**
 * Manages the hierarchical economy system with:
 * - CK3-style settlement hierarchy (Empire → Kingdom → Duchy → County → Holding)
 * - Multi-layered economies (Local, Regional, Imperial)
 * - Passive trade between adjacent holdings
 * - Population growth based on needs satisfaction
 * - AI merchant dynasties competing with player
 * - Trade route ownership and share system
 */
UCLASS()
class GAMETRADE_API UGTHierarchicalEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========== HIERARCHY MANAGEMENT ==========

	/** Create the full settlement hierarchy from existing data */
	UFUNCTION(BlueprintCallable, Category = "Hierarchy")
	void BuildHierarchyFromLegacyData();

	/** Get holding by ID */
	UFUNCTION(BlueprintCallable, Category = "Hierarchy")
	FHoldingData* GetHolding(const FString& HoldingID);

	/** Get all holdings in a county */
	UFUNCTION(BlueprintCallable, Category = "Hierarchy")
	TArray<FHoldingData*> GetHoldingsInCounty(const FString& CountyID);

	/** Get all counties in a duchy */
	UFUNCTION(BlueprintCallable, Category = "Hierarchy")
	TArray<FCountyData*> GetCountiesInDuchy(const FString& DuchyID);

	// ========== PASSIVE TRADE SYSTEM ==========

	/** Generate passive trade connections between adjacent holdings */
	UFUNCTION(BlueprintCallable, Category = "Passive Trade")
	void GeneratePassiveTradeNetwork();

	/** Calculate if two holdings can passively trade (geography check) */
	UFUNCTION(BlueprintCallable, Category = "Passive Trade")
	bool CanPassivelyTrade(const FString& HoldingA, const FString& HoldingB);

	/** Process all passive trade flows for this turn */
	UFUNCTION(BlueprintCallable, Category = "Passive Trade")
	void ProcessPassiveTrade();

	// ========== POPULATION & GROWTH ==========

	/** Calculate needs satisfaction for a holding */
	UFUNCTION(BlueprintCallable, Category = "Population")
	float CalculateNeedsSatisfaction(const FString& HoldingID);

	/** Process population growth for all holdings */
	UFUNCTION(BlueprintCallable, Category = "Population")
	void ProcessPopulationGrowth();

	/** Check if holding's population needs are met */
	UFUNCTION(BlueprintCallable, Category = "Population")
	void UpdatePopulationNeeds(const FString& HoldingID);

	// ========== REGIONAL ECONOMIES ==========

	/** Aggregate local production to county level */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AggregateCountyEconomy(const FString& CountyID);

	/** Aggregate county economies to duchy level */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AggregateDuchyEconomy(const FString& DuchyID);

	/** Aggregate duchy economies to kingdom level */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AggregateKingdomEconomy(const FString& KingdomID);

	/** Process all economy layers (local → regional → imperial) */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void ProcessLayeredEconomies();

	// ========== CUSTOM TRADE ROUTES ==========

	/** Player creates a custom trade route */
	UFUNCTION(BlueprintCallable, Category = "Trade Routes")
	FString CreateCustomTradeRoute(const FString& OriginID, const FString& DestinationID,
		const TMap<EResourceType, int32>& GoodsToTransport, const FString& PlayerFamilyID);

	/** Process all custom trade routes */
	UFUNCTION(BlueprintCallable, Category = "Trade Routes")
	void ProcessCustomTradeRoutes();

	/** Calculate profit for a trade route */
	UFUNCTION(BlueprintCallable, Category = "Trade Routes")
	float CalculateRouteProfit(const FString& RouteID);

	// ========== TRADE ROUTE OWNERSHIP ==========

	/** Buy shares of a trade route */
	UFUNCTION(BlueprintCallable, Category = "Ownership")
	bool BuyRouteShares(const FString& RouteID, const FString& BuyerFamilyID,
		float SharePercentage, float Price);

	/** Sell shares of a trade route */
	UFUNCTION(BlueprintCallable, Category = "Ownership")
	bool SellRouteShares(const FString& RouteID, const FString& SellerFamilyID,
		float SharePercentage);

	/** Usurp trade route through political schemes */
	UFUNCTION(BlueprintCallable, Category = "Ownership")
	bool UsurpTradeRoute(const FString& RouteID, const FString& UsurperFamilyID,
		const FString& FeudLordID, float BribeAmount);

	// ========== AI MERCHANT DYNASTIES ==========

	/** Create AI merchant families to compete with player */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SpawnAIMerchantDynasties(int32 Count);

	/** Process AI merchant actions for this turn */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ProcessAIMerchants();

	/** AI evaluates potential trade routes */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void AIEvaluateTradeOpportunities(const FString& AIFamilyID);

	/** AI attempts to buy player's route shares */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void AIAttemptRouteAcquisition(const FString& AIFamilyID, const FString& RouteID);

	// ========== TURN PROCESSING ==========

	/** Process entire hierarchical economy for one turn */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void ProcessHierarchicalEconomyTurn();

private:
	// ========== DATA STORAGE ==========

	UPROPERTY()
	TMap<FString, FHoldingData> AllHoldings;

	UPROPERTY()
	TMap<FString, FCountyData> AllCounties;

	UPROPERTY()
	TMap<FString, FDuchyData> AllDuchies;

	UPROPERTY()
	TMap<FString, FKingdomData> AllKingdoms;

	UPROPERTY()
	TMap<FString, FEmpireData> AllEmpires;

	UPROPERTY()
	TArray<FPassiveTradeConnection> PassiveTradeNetwork;

	UPROPERTY()
	TMap<FString, FCustomTradeRoute> CustomTradeRoutes;

	UPROPERTY()
	TMap<FString, FAIMerchantDynasty> AIMerchants;

	UPROPERTY()
	TArray<FTradeShareOffer> ActiveShareOffers;

	UPROPERTY()
	FPopulationNeedsTemplate PopulationNeeds;

	// ========== HELPER FUNCTIONS ==========

	/** Calculate distance between two holdings */
	float CalculateDistance(const FString& HoldingA, const FString& HoldingB);

	/** Calculate geography modifier for trade */
	float CalculateGeographyModifier(const FString& HoldingA, const FString& HoldingB);

	/** Check if geography blocks trade (high mountains, large sea) */
	bool IsGeographyBlocking(EGeographyType GeographyA, EGeographyType GeographyB);

	/** Calculate price difference for arbitrage */
	float CalculatePriceDifference(const FString& HoldingA, const FString& HoldingB,
		EResourceType Resource);

	/** Find best trade opportunities for AI */
	TArray<TPair<FString, FString>> FindProfitableRoutes(const FString& HomeHoldingID);

	/** Calculate population growth rate based on needs */
	float CalculateGrowthRate(float NeedsSatisfaction);

	/** Distribute profits to route owners */
	void DistributeRouteProfit(const FString& RouteID, float Profit);

	/** Initialize default population needs */
	void InitializePopulationNeeds();

	/** Get holding's county */
	FCountyData* GetHoldingCounty(const FString& HoldingID);

	/** Get county's duchy */
	FDuchyData* GetCountyDuchy(const FString& CountyID);

	/** Get duchy's kingdom */
	FKingdomData* GetDuchyKingdom(const FString& DuchyID);

	/** Calculate tariffs for trade between two holdings */
	float CalculateTariffs(const FString& HoldingA, const FString& HoldingB);
};
