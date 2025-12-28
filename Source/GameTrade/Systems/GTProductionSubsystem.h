// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTDataTypes.h"
#include "Engine/DataTable.h"
#include "GTProductionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuildingConstructed, const FString&, BuildingID, EBuildingType, BuildingType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionComplete, const FString&, BuildingID, const TArray<FResourceAmount>&, OutputProduced);

/**
 * Manages production buildings and production chains
 * - Building construction and upgrades
 * - Production processing (inputs → outputs)
 * - Worker management
 * - Efficiency calculations
 */
UCLASS()
class GAMETRADE_API UGTProductionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Building construction
	UFUNCTION(BlueprintCallable, Category = "Production")
	bool ConstructBuilding(const FString& SettlementName, EBuildingType BuildingType, const FString& OwnerFamilyID, float& OutCost);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool UpgradeBuilding(const FString& BuildingID, float& OutCost);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool DestroyBuilding(const FString& BuildingID);

	// Building management
	UFUNCTION(BlueprintCallable, Category = "Production")
	bool AssignManager(const FString& BuildingID, const FString& CharacterID);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool AddWorkers(const FString& BuildingID, int32 NumWorkers);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool RemoveWorkers(const FString& BuildingID, int32 NumWorkers);

	// Production processing
	UFUNCTION(BlueprintCallable, Category = "Production")
	void ProcessProductionTurn();

	// Queries
	UFUNCTION(BlueprintPure, Category = "Production")
	TArray<FBuildingData> GetBuildingsInSettlement(const FString& SettlementName) const;

	UFUNCTION(BlueprintPure, Category = "Production")
	TArray<FBuildingData> GetBuildingsOwnedByFamily(const FString& FamilyID) const;

	UFUNCTION(BlueprintPure, Category = "Production")
	FBuildingData GetBuildingData(const FString& BuildingID) const;

	UFUNCTION(BlueprintPure, Category = "Production")
	float GetBuildingCost(EBuildingType BuildingType, int32 Level = 1) const;

	UFUNCTION(BlueprintPure, Category = "Production")
	float CalculateBuildingEfficiency(const FString& BuildingID) const;

	UFUNCTION(BlueprintPure, Category = "Production")
	bool CanConstructBuilding(const FString& SettlementName, EBuildingType BuildingType, const FString& FamilyID) const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnBuildingConstructed OnBuildingConstructed;

	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnProductionComplete OnProductionComplete;

protected:
	// Data
	UPROPERTY()
	TMap<FString, FBuildingData> Buildings;

	// Helper functions
	FString GenerateBuildingID() const;
	void InitializeBuildingRecipes();
	void ProcessBuildingProduction(FBuildingData& Building);
	bool HasRequiredInputs(const FBuildingData& Building) const;
	void ConsumeInputs(FBuildingData& Building);
	void ProduceOutputs(FBuildingData& Building);
	float GetManagerBonus(const FString& ManagerID) const;

	// Building recipes (defined by building type)
	UPROPERTY()
	TMap<EBuildingType, FBuildingRecipe> BuildingRecipes;
};

// Building recipe structure (defines what a building does)
USTRUCT(BlueprintType)
struct FBuildingRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildingType BuildingType = EBuildingType::Farm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseCost = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseWorkers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceAmount> InputsPerWeek;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceAmount> OutputsPerWeek;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ConstructionTimeWeeks = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description = TEXT("");
};
