// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GTSystemTester.generated.h"

/**
 * Test harness to demonstrate sandbox system interactions
 * Use console commands to run integration tests
 */
UCLASS()
class GAMETRADE_API UGTSystemTester : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Test 1: Harvest Event Cascade
	 * Demonstrates: Event → Economy → Production → Trade chain
	 *
	 * Expected Behavior:
	 * 1. Trigger "Bad Harvest" event in München
	 * 2. Grain supply drops 60%
	 * 3. Grain price spikes 2-3x
	 * 4. Bakeries can't afford grain → production drops
	 * 5. Bread supply drops → bread price spikes
	 * 6. Trade routes adjust (caravans import grain)
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestHarvestEventCascade(UWorld* World);

	/**
	 * Test 2: Production Chain Integration
	 * Demonstrates: Production → Economy → Supply/Demand
	 *
	 * Expected Behavior:
	 * 1. Player builds 3 breweries in München
	 * 2. Breweries consume 90 grain/week
	 * 3. München grain demand spikes → price rises
	 * 4. Breweries produce 150 beer/week
	 * 5. München beer supply spikes → price drops
	 * 6. AI traders notice opportunity → start importing grain
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestProductionChainImpact(UWorld* World);

	/**
	 * Test 3: Dynasty Marriage Benefits
	 * Demonstrates: Dynasty → Economy → Production unlock
	 *
	 * Expected Behavior:
	 * 1. Player character marries Venetian merchant
	 * 2. Dynasty system grants "Venice Access" trait
	 * 3. Economy system unlocks Venice silk workshops
	 * 4. Production system allows building silk workshop
	 * 5. Trade routes to Venice open with bonuses
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestDynastyEconomyIntegration(UWorld* World);

	/**
	 * Test 4: Multi-Turn System Interaction
	 * Demonstrates: All systems working together over time
	 *
	 * Expected Behavior:
	 * 1. Set up player with brewery in München
	 * 2. Run 52 turns (1 year)
	 * 3. Track: Production → Sales → Gold → Events → Dynasty changes
	 * 4. Verify seasonal price fluctuations affect profits
	 * 5. Verify random events impact business
	 * 6. Verify character aging affects manager efficiency
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestMultiTurnIntegration(UWorld* World);

	/**
	 * Test 5: Trade Route Disruption Cascade
	 * Demonstrates: Event → Trade → Economy → Production
	 *
	 * Expected Behavior:
	 * 1. Trigger "War" event between England and France
	 * 2. Trade routes through English Channel blocked
	 * 3. English wool can't reach Bruges
	 * 4. Bruges cloth production drops (no wool input)
	 * 5. Cloth prices spike across Europe
	 * 6. Alternative trade routes emerge (longer, more expensive)
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestTradeDisruptionCascade(UWorld* World);

	/**
	 * Test 6: Black Death Pandemic (Full Sandbox Test)
	 * Demonstrates: ALL systems interacting during crisis
	 *
	 * Expected Behavior:
	 * 1. Trigger Black Death event (1348)
	 * 2. Dynasty: 30% of characters die (including managers)
	 * 3. Production: Buildings lose managers → efficiency drops
	 * 4. Economy: Population drops → demand crashes
	 * 5. Economy: Labor shortage → wages spike → prices spike
	 * 6. Trade: Caravans avoid plague cities → routes change
	 * 7. Events: Multiple follow-up events (riots, church authority)
	 * 8. Player choices affect recovery speed
	 */
	UFUNCTION(BlueprintCallable, Category = "Testing")
	static void TestBlackDeathFullSandbox(UWorld* World);

private:
	// Helper functions for test setup
	static void SetupTestScenario(UWorld* World);
	static void CreateTestCharacter(UWorld* World, const FString& Name, const FString& HomeTown);
	static void CreateTestBuilding(UWorld* World, const FString& SettlementName, EBuildingType Type);
	static void LogSystemState(UWorld* World, const FString& TestName);
	static void PrintTestResults(const FString& TestName, bool bSuccess, const FString& Details);
};
