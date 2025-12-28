// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTDataTypes.h"
#include "GTTradeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaravanDeparted, const FString&, CaravanID, const FString&, Destination);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCaravanArrived, const FString&, CaravanID, const FString&, Destination, bool, bSafeArrival);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaravanAttacked, const FString&, CaravanID, float, LossPercentage);

/**
 * Manages caravans and trade routes
 * - Caravan creation and management
 * - Travel simulation
 * - Random encounters (bandits, weather)
 * - Automatic trade routes
 */
UCLASS()
class GAMETRADE_API UGTTradeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Caravan management
	UFUNCTION(BlueprintCallable, Category = "Trade")
	FString SendCaravan(const FString& OwnerFamilyID, const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& Cargo);

	UFUNCTION(BlueprintCallable, Category = "Trade")
	bool RecallCaravan(const FString& CaravanID);

	UFUNCTION(BlueprintCallable, Category = "Trade")
	bool SetCaravanAutomatic(const FString& CaravanID, bool bAutomatic);

	// Turn processing
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void ProcessCaravansTurn();

	// Queries
	UFUNCTION(BlueprintPure, Category = "Trade")
	TArray<FCaravanData> GetActiveCaravans(const FString& OwnerFamilyID) const;

	UFUNCTION(BlueprintPure, Category = "Trade")
	FCaravanData GetCaravanData(const FString& CaravanID) const;

	UFUNCTION(BlueprintPure, Category = "Trade")
	int32 GetCaravanCount(const FString& OwnerFamilyID) const;

	UFUNCTION(BlueprintPure, Category = "Trade")
	bool IsCaravanTraveling(const FString& CaravanID) const;

	// Trade route templates (for automatic trading)
	UFUNCTION(BlueprintCallable, Category = "Trade")
	FString CreateTradeRoute(const FString& OwnerFamilyID, const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& GoodsToBuy, const TArray<FResourceAmount>& GoodsToSell);

	UFUNCTION(BlueprintCallable, Category = "Trade")
	bool DeleteTradeRoute(const FString& RouteID);

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Trade")
	FOnCaravanDeparted OnCaravanDeparted;

	UPROPERTY(BlueprintAssignable, Category = "Trade")
	FOnCaravanArrived OnCaravanArrived;

	UPROPERTY(BlueprintAssignable, Category = "Trade")
	FOnCaravanAttacked OnCaravanAttacked;

protected:
	// Data
	UPROPERTY()
	TMap<FString, FCaravanData> Caravans;

	UPROPERTY()
	TMap<FString, FTradeRoute> TradeRoutes;

	// Helper functions
	FString GenerateCaravanID() const;
	FString GenerateRouteID() const;
	void AdvanceCaravanTravel(FCaravanData& Caravan);
	void HandleCaravanArrival(FCaravanData& Caravan);
	bool RollRandomEncounter(FCaravanData& Caravan);
	void HandleBanditAttack(FCaravanData& Caravan);
	void HandleWeatherDelay(FCaravanData& Caravan);
	float CalculateRiskLevel(const FString& FromSettlement, const FString& ToSettlement) const;
};

// Trade route structure (for automatic trading)
USTRUCT(BlueprintType)
struct FTradeRoute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RouteID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwnerFamilyID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FromSettlement = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ToSettlement = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceAmount> GoodsToBuy; // What to buy in FromSettlement

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceAmount> GoodsToSell; // What to sell in ToSettlement

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TripsMade = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalProfitEarned = 0.0f;
};
