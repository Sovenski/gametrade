// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTProductionSubsystem.h"
#include "Systems/GTEconomySubsystem.h"
#include "Core/GTGameInstance.h"
#include "Core/GTSaveGame.h"

void UGTProductionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Production Subsystem Initialized"));

	InitializeBuildingRecipes();
}

void UGTProductionSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Production Subsystem Deinitialized"));
}

void UGTProductionSubsystem::InitializeBuildingRecipes()
{
	// Farm - produces grain
	FBuildingRecipe FarmRecipe;
	FarmRecipe.BuildingType = EBuildingType::Farm;
	FarmRecipe.BaseCost = 200.0f;
	FarmRecipe.BaseWorkers = 3;
	FarmRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Grain, 50));
	FarmRecipe.ConstructionTimeWeeks = 8;
	FarmRecipe.Description = TEXT("Produces grain");
	BuildingRecipes.Add(EBuildingType::Farm, FarmRecipe);

	// Mill - converts grain to flour
	FBuildingRecipe MillRecipe;
	MillRecipe.BuildingType = EBuildingType::Mill;
	MillRecipe.BaseCost = 300.0f;
	MillRecipe.BaseWorkers = 2;
	MillRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Grain, 40));
	MillRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Flour, 40));
	MillRecipe.ConstructionTimeWeeks = 6;
	MillRecipe.Description = TEXT("Grinds grain into flour");
	BuildingRecipes.Add(EBuildingType::Mill, MillRecipe);

	// Bakery - converts flour to bread
	FBuildingRecipe BakeryRecipe;
	BakeryRecipe.BuildingType = EBuildingType::Bakery;
	BakeryRecipe.BaseCost = 250.0f;
	BakeryRecipe.BaseWorkers = 2;
	BakeryRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Flour, 30));
	BakeryRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Bread, 60)); // 2:1 ratio
	BakeryRecipe.ConstructionTimeWeeks = 5;
	BakeryRecipe.Description = TEXT("Bakes bread from flour");
	BuildingRecipes.Add(EBuildingType::Bakery, BakeryRecipe);

	// Brewery - beer production
	FBuildingRecipe BreweryRecipe;
	BreweryRecipe.BuildingType = EBuildingType::Brewery;
	BreweryRecipe.BaseCost = 350.0f;
	BreweryRecipe.BaseWorkers = 3;
	BreweryRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Grain, 30));
	BreweryRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Hops, 10));
	BreweryRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Beer, 50));
	BreweryRecipe.ConstructionTimeWeeks = 6;
	BreweryRecipe.Description = TEXT("Brews beer from grain and hops");
	BuildingRecipes.Add(EBuildingType::Brewery, BreweryRecipe);

	// Tannery - leather production
	FBuildingRecipe TanneryRecipe;
	TanneryRecipe.BuildingType = EBuildingType::Tannery;
	TanneryRecipe.BaseCost = 400.0f;
	TanneryRecipe.BaseWorkers = 4;
	TanneryRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Hides, 30));
	TanneryRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Leather, 25));
	TanneryRecipe.ConstructionTimeWeeks = 7;
	TanneryRecipe.Description = TEXT("Tans hides into leather");
	BuildingRecipes.Add(EBuildingType::Tannery, TanneryRecipe);

	// Sawmill - timber to planks
	FBuildingRecipe SawmillRecipe;
	SawmillRecipe.BuildingType = EBuildingType::Sawmill;
	SawmillRecipe.BaseCost = 500.0f;
	SawmillRecipe.BaseWorkers = 4;
	SawmillRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Timber, 80));
	SawmillRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Planks, 60));
	SawmillRecipe.ConstructionTimeWeeks = 8;
	SawmillRecipe.Description = TEXT("Cuts timber into planks");
	BuildingRecipes.Add(EBuildingType::Sawmill, SawmillRecipe);

	// Forge - smelting
	FBuildingRecipe ForgeRecipe;
	ForgeRecipe.BuildingType = EBuildingType::Forge;
	ForgeRecipe.BaseCost = 600.0f;
	ForgeRecipe.BaseWorkers = 5;
	ForgeRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::IronOre, 50));
	ForgeRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::Charcoal, 30));
	ForgeRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::IronBars, 40));
	ForgeRecipe.ConstructionTimeWeeks = 10;
	ForgeRecipe.Description = TEXT("Smelts iron ore into iron bars");
	BuildingRecipes.Add(EBuildingType::Forge, ForgeRecipe);

	// Blacksmith - tools and weapons
	FBuildingRecipe BlacksmithRecipe;
	BlacksmithRecipe.BuildingType = EBuildingType::Blacksmith;
	BlacksmithRecipe.BaseCost = 500.0f;
	BlacksmithRecipe.BaseWorkers = 3;
	BlacksmithRecipe.InputsPerWeek.Add(FResourceAmount(EResourceType::IronBars, 30));
	BlacksmithRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Tools, 20));
	BlacksmithRecipe.OutputsPerWeek.Add(FResourceAmount(EResourceType::Weapons, 10));
	BlacksmithRecipe.ConstructionTimeWeeks = 7;
	BlacksmithRecipe.Description = TEXT("Crafts tools and weapons from iron");
	BuildingRecipes.Add(EBuildingType::Blacksmith, BlacksmithRecipe);

	// Warehouse - storage (no production)
	FBuildingRecipe WarehouseRecipe;
	WarehouseRecipe.BuildingType = EBuildingType::Warehouse;
	WarehouseRecipe.BaseCost = 400.0f;
	WarehouseRecipe.BaseWorkers = 1;
	WarehouseRecipe.ConstructionTimeWeeks = 6;
	WarehouseRecipe.Description = TEXT("Stores goods, provides storage capacity");
	BuildingRecipes.Add(EBuildingType::Warehouse, WarehouseRecipe);

	UE_LOG(LogTemp, Log, TEXT("Initialized %d building recipes"), BuildingRecipes.Num());
}

bool UGTProductionSubsystem::ConstructBuilding(const FString& SettlementName, EBuildingType BuildingType, const FString& OwnerFamilyID, float& OutCost)
{
	if (!CanConstructBuilding(SettlementName, BuildingType, OwnerFamilyID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot construct building in %s"), *SettlementName);
		return false;
	}

	const FBuildingRecipe* Recipe = BuildingRecipes.Find(BuildingType);
	if (!Recipe)
	{
		UE_LOG(LogTemp, Error, TEXT("No recipe found for building type %s"), *UEnum::GetValueAsString(BuildingType));
		return false;
	}

	// Calculate cost
	OutCost = Recipe->BaseCost;

	// Create building
	FBuildingData NewBuilding;
	NewBuilding.BuildingID = GenerateBuildingID();
	NewBuilding.BuildingType = BuildingType;
	NewBuilding.SettlementName = SettlementName;
	NewBuilding.OwnerFamilyID = OwnerFamilyID;
	NewBuilding.Level = 1;
	NewBuilding.Workers = Recipe->BaseWorkers;
	NewBuilding.InputsPerWeek = Recipe->InputsPerWeek;
	NewBuilding.OutputsPerWeek = Recipe->OutputsPerWeek;

	Buildings.Add(NewBuilding.BuildingID, NewBuilding);

	OnBuildingConstructed.Broadcast(NewBuilding.BuildingID, BuildingType);

	UE_LOG(LogTemp, Log, TEXT("Constructed %s in %s (ID: %s, Cost: %.2f)"),
		*UEnum::GetValueAsString(BuildingType), *SettlementName, *NewBuilding.BuildingID, OutCost);

	return true;
}

bool UGTProductionSubsystem::UpgradeBuilding(const FString& BuildingID, float& OutCost)
{
	FBuildingData* Building = Buildings.Find(BuildingID);
	if (!Building)
	{
		UE_LOG(LogTemp, Warning, TEXT("Building not found: %s"), *BuildingID);
		return false;
	}

	if (Building->Level >= 5)
	{
		UE_LOG(LogTemp, Warning, TEXT("Building already at max level"));
		return false;
	}

	const FBuildingRecipe* Recipe = BuildingRecipes.Find(Building->BuildingType);
	if (!Recipe)
	{
		return false;
	}

	// Upgrade cost increases with level
	OutCost = Recipe->BaseCost * (Building->Level + 1) * 0.75f;

	// Increase level
	Building->Level++;

	// Increase output by 20% per level
	float OutputMultiplier = 1.0f + (Building->Level - 1) * 0.2f;
	for (FResourceAmount& Output : Building->OutputsPerWeek)
	{
		Output.Amount = FMath::RoundToInt(Recipe->OutputsPerWeek[0].Amount * OutputMultiplier);
	}

	UE_LOG(LogTemp, Log, TEXT("Upgraded building %s to level %d (Cost: %.2f)"),
		*BuildingID, Building->Level, OutCost);

	return true;
}

bool UGTProductionSubsystem::DestroyBuilding(const FString& BuildingID)
{
	if (Buildings.Remove(BuildingID) > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Destroyed building: %s"), *BuildingID);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Building not found for destruction: %s"), *BuildingID);
	return false;
}

bool UGTProductionSubsystem::AssignManager(const FString& BuildingID, const FString& CharacterID)
{
	FBuildingData* Building = Buildings.Find(BuildingID);
	if (!Building)
	{
		return false;
	}

	Building->ManagerCharacterID = CharacterID;

	UE_LOG(LogTemp, Log, TEXT("Assigned manager %s to building %s"), *CharacterID, *BuildingID);
	return true;
}

bool UGTProductionSubsystem::AddWorkers(const FString& BuildingID, int32 NumWorkers)
{
	FBuildingData* Building = Buildings.Find(BuildingID);
	if (!Building)
	{
		return false;
	}

	Building->Workers += NumWorkers;
	UE_LOG(LogTemp, Log, TEXT("Added %d workers to building %s (Total: %d)"),
		NumWorkers, *BuildingID, Building->Workers);
	return true;
}

bool UGTProductionSubsystem::RemoveWorkers(const FString& BuildingID, int32 NumWorkers)
{
	FBuildingData* Building = Buildings.Find(BuildingID);
	if (!Building)
	{
		return false;
	}

	Building->Workers = FMath::Max(0, Building->Workers - NumWorkers);
	UE_LOG(LogTemp, Log, TEXT("Removed %d workers from building %s (Remaining: %d)"),
		NumWorkers, *BuildingID, Building->Workers);
	return true;
}

void UGTProductionSubsystem::ProcessProductionTurn()
{
	UE_LOG(LogTemp, Log, TEXT("Processing production for %d buildings"), Buildings.Num());

	for (auto& BuildingPair : Buildings)
	{
		ProcessBuildingProduction(BuildingPair.Value);
	}
}

void UGTProductionSubsystem::ProcessBuildingProduction(FBuildingData& Building)
{
	// Check if building has required inputs
	if (!HasRequiredInputs(Building))
	{
		UE_LOG(LogTemp, Verbose, TEXT("Building %s lacks required inputs"), *Building.BuildingID);
		return;
	}

	// Calculate efficiency
	float Efficiency = CalculateBuildingEfficiency(Building.BuildingID);

	// Consume inputs
	ConsumeInputs(Building);

	// Produce outputs (modified by efficiency)
	TArray<FResourceAmount> ActualOutputs;
	for (const FResourceAmount& Output : Building.OutputsPerWeek)
	{
		FResourceAmount ActualOutput = Output;
		ActualOutput.Amount = FMath::RoundToInt(Output.Amount * Efficiency);
		ActualOutputs.Add(ActualOutput);
	}

	ProduceOutputs(Building);

	OnProductionComplete.Broadcast(Building.BuildingID, ActualOutputs);

	UE_LOG(LogTemp, Verbose, TEXT("Building %s produced goods (Efficiency: %.2f%%)"),
		*Building.BuildingID, Efficiency * 100.0f);
}

bool UGTProductionSubsystem::HasRequiredInputs(const FBuildingData& Building) const
{
	// If no inputs required (e.g., Farm), always return true
	if (Building.InputsPerWeek.Num() == 0)
	{
		return true;
	}

	// TODO: Check if owner has resources in inventory or warehouse
	// For now, assume inputs are available
	return true;
}

void UGTProductionSubsystem::ConsumeInputs(FBuildingData& Building)
{
	// TODO: Actually consume resources from owner's inventory
	UE_LOG(LogTemp, Verbose, TEXT("Consumed inputs for building %s"), *Building.BuildingID);
}

void UGTProductionSubsystem::ProduceOutputs(FBuildingData& Building)
{
	// TODO: Add resources to owner's inventory or settlement market
	UE_LOG(LogTemp, Verbose, TEXT("Produced outputs for building %s"), *Building.BuildingID);
}

float UGTProductionSubsystem::CalculateBuildingEfficiency(const FString& BuildingID) const
{
	const FBuildingData* Building = Buildings.Find(BuildingID);
	if (!Building)
	{
		return 0.0f;
	}

	float Efficiency = 1.0f;

	// Manager bonus
	if (!Building->ManagerCharacterID.IsEmpty())
	{
		float ManagerBonus = GetManagerBonus(Building->ManagerCharacterID);
		Efficiency += ManagerBonus;
	}

	// Level bonus (5% per level)
	Efficiency += (Building->Level - 1) * 0.05f;

	// Worker efficiency (baseline at BaseWorkers, -10% for each missing worker)
	const FBuildingRecipe* Recipe = BuildingRecipes.Find(Building->BuildingType);
	if (Recipe && Building->Workers < Recipe->BaseWorkers)
	{
		int32 MissingWorkers = Recipe->BaseWorkers - Building->Workers;
		Efficiency -= MissingWorkers * 0.1f;
	}

	return FMath::Max(0.1f, Efficiency); // Minimum 10% efficiency
}

float UGTProductionSubsystem::GetManagerBonus(const FString& ManagerID) const
{
	// TODO: Get character data and calculate bonus from Stewardship skill
	// For now, return base bonus
	return 0.1f; // 10% bonus
}

TArray<FBuildingData> UGTProductionSubsystem::GetBuildingsInSettlement(const FString& SettlementName) const
{
	TArray<FBuildingData> Result;

	for (const auto& BuildingPair : Buildings)
	{
		if (BuildingPair.Value.SettlementName == SettlementName)
		{
			Result.Add(BuildingPair.Value);
		}
	}

	return Result;
}

TArray<FBuildingData> UGTProductionSubsystem::GetBuildingsOwnedByFamily(const FString& FamilyID) const
{
	TArray<FBuildingData> Result;

	for (const auto& BuildingPair : Buildings)
	{
		if (BuildingPair.Value.OwnerFamilyID == FamilyID)
		{
			Result.Add(BuildingPair.Value);
		}
	}

	return Result;
}

FBuildingData UGTProductionSubsystem::GetBuildingData(const FString& BuildingID) const
{
	const FBuildingData* Building = Buildings.Find(BuildingID);
	return Building ? *Building : FBuildingData();
}

float UGTProductionSubsystem::GetBuildingCost(EBuildingType BuildingType, int32 Level) const
{
	const FBuildingRecipe* Recipe = BuildingRecipes.Find(BuildingType);
	if (!Recipe)
	{
		return 0.0f;
	}

	if (Level == 1)
	{
		return Recipe->BaseCost;
	}
	else
	{
		// Upgrade cost
		return Recipe->BaseCost * Level * 0.75f;
	}
}

bool UGTProductionSubsystem::CanConstructBuilding(const FString& SettlementName, EBuildingType BuildingType, const FString& FamilyID) const
{
	// TODO: Add more validation
	// - Check if settlement exists
	// - Check if family has enough gold
	// - Check if building type is allowed in settlement type
	// - Check for building limits

	return true;
}

FString UGTProductionSubsystem::GenerateBuildingID() const
{
	return FGuid::NewGuid().ToString();
}
