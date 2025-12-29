// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTMarketEconomySubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGTMarketEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("GTMarketEconomySubsystem Initialized"));

	InitializeProductionRecipes();
	InitializePopNeeds();
}

void UGTMarketEconomySubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("GTMarketEconomySubsystem Deinitialized"));
}

// ========== INITIALIZATION ==========

void UGTMarketEconomySubsystem::InitializeMarketEconomy()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing Market Economy System..."));

	// Create example markets
	CreateMarket(TEXT("München"), 5.0f); // Large city
	CreateMarket(TEXT("Dachau"), 1.0f);  // Village
	CreateMarket(TEXT("Paris"), 10.0f);  // Major city

	// Create some example buildings in München
	ConstructBuilding(TEXT("München"), EBuildingType_Market::Farm, TEXT("NPC_Farmer"), 0.0f);
	ConstructBuilding(TEXT("München"), EBuildingType_Market::Mill, TEXT("NPC_Miller"), 0.0f);
	ConstructBuilding(TEXT("München"), EBuildingType_Market::Bakery, TEXT("NPC_Baker"), 0.0f);
	ConstructBuilding(TEXT("München"), EBuildingType_Market::Brewery, TEXT("NPC_Brewer"), 0.0f);
	ConstructBuilding(TEXT("München"), EBuildingType_Market::Blacksmith, TEXT("NPC_Smith"), 0.0f);

	// Create pop groups in München
	CreatePopGroup(TEXT("München"), EPopClass::Peasant, 10000);    // 10,000 peasants
	CreatePopGroup(TEXT("München"), EPopClass::Craftsman, 3000);   // 3,000 craftsmen
	CreatePopGroup(TEXT("München"), EPopClass::Merchant, 1500);    // 1,500 merchants
	CreatePopGroup(TEXT("München"), EPopClass::Noble, 500);        // 500 nobles

	UE_LOG(LogTemp, Warning, TEXT("  Created 3 markets, 5 buildings, 4 pop groups"));
}

void UGTMarketEconomySubsystem::CreateMarket(const FString& MarketName, float InitialSize)
{
	FLocalMarket NewMarket;
	NewMarket.MarketName = MarketName;
	NewMarket.MarketSize = InitialSize;
	NewMarket.MarketEfficiency = 0.8f;

	// Initialize price/supply/demand for ALL resource types
	for (int32 i = 0; i < (int32)EResourceType::MAX; i++)
	{
		EResourceType Resource = (EResourceType)i;
		FMarketGoodData GoodData;
		GoodData.Price = GetBasePrice(Resource);
		GoodData.Supply = 0;
		GoodData.Demand = 0;
		GoodData.Stockpile = 100; // Start with some initial stockpile

		NewMarket.GoodsMarket.Add(Resource, GoodData);
	}

	AllMarkets.Add(MarketName, NewMarket);

	UE_LOG(LogTemp, Log, TEXT("Created market: %s (size: %.1f)"), *MarketName, InitialSize);
}

void UGTMarketEconomySubsystem::InitializeProductionRecipes()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing Production Recipes..."));

	// ========== PRIMARY PRODUCTION (Raw Resources) ==========

	// FARM: Produces Grain
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 2);        // Need tools for farming
		Recipe.Outputs.Add(EResourceType::Grain, 500);     // Produce 500 grain/week
		Recipe.LaborRequired = 20;                         // 20 farm workers
		Recipe.WagePerWorker = 0.5f;                       // Low wages
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Farm, Recipe);
	}

	// RANCH: Produces Wool, Hides, Cattle
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Grain, 100);      // Feed for animals
		Recipe.Inputs.Add(EResourceType::Tools, 1);
		Recipe.Outputs.Add(EResourceType::Wool, 150);
		Recipe.Outputs.Add(EResourceType::Hides, 50);
		Recipe.Outputs.Add(EResourceType::Cattle, 20);
		Recipe.LaborRequired = 15;
		Recipe.WagePerWorker = 0.6f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Ranch, Recipe);
	}

	// FISHERY: Produces Fish
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 1);
		Recipe.Inputs.Add(EResourceType::Salt, 10);        // For preserving
		Recipe.Outputs.Add(EResourceType::Fish, 300);
		Recipe.Outputs.Add(EResourceType::Cod, 100);
		Recipe.Outputs.Add(EResourceType::Herring, 150);
		Recipe.LaborRequired = 25;
		Recipe.WagePerWorker = 0.7f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Fishery, Recipe);
	}

	// FOREST: Produces Timber
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 3);        // Axes, saws
		Recipe.Outputs.Add(EResourceType::Timber, 400);
		Recipe.Outputs.Add(EResourceType::Game, 50);       // Hunting byproduct
		Recipe.LaborRequired = 30;
		Recipe.WagePerWorker = 0.6f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Forest, Recipe);
	}

	// IRON MINE: Produces Iron Ore
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 5);
		Recipe.Inputs.Add(EResourceType::Timber, 50);      // For shoring
		Recipe.Outputs.Add(EResourceType::IronOre, 200);
		Recipe.LaborRequired = 40;
		Recipe.WagePerWorker = 0.8f;                       // Dangerous work = higher wages
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::IronMine, Recipe);
	}

	// SALT MINE: Produces Salt
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 3);
		Recipe.Outputs.Add(EResourceType::Salt, 300);
		Recipe.LaborRequired = 25;
		Recipe.WagePerWorker = 0.7f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::SaltMine, Recipe);
	}

	// QUARRY: Produces Stone, Marble
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Tools, 4);
		Recipe.Outputs.Add(EResourceType::Stone, 300);
		Recipe.Outputs.Add(EResourceType::Marble, 50);     // Rare high-quality
		Recipe.LaborRequired = 35;
		Recipe.WagePerWorker = 0.7f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Quarry, Recipe);
	}

	// ========== SECONDARY PRODUCTION (Processing) ==========

	// MILL: Grain → Flour
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Grain, 400);
		Recipe.Inputs.Add(EResourceType::Tools, 1);
		Recipe.Outputs.Add(EResourceType::Flour, 350);     // 12.5% waste
		Recipe.LaborRequired = 8;
		Recipe.WagePerWorker = 0.8f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Mill, Recipe);
	}

	// BAKERY: Flour → Bread
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Flour, 300);
		Recipe.Inputs.Add(EResourceType::Salt, 5);
		Recipe.Inputs.Add(EResourceType::Timber, 10);      // For oven fuel
		Recipe.Outputs.Add(EResourceType::Bread, 400);     // Value add!
		Recipe.LaborRequired = 10;
		Recipe.WagePerWorker = 1.0f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Bakery, Recipe);
	}

	// BREWERY: Grain + Hops → Beer
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Grain, 200);
		Recipe.Inputs.Add(EResourceType::Hops, 50);
		Recipe.Inputs.Add(EResourceType::Timber, 15);      // For heating
		Recipe.Outputs.Add(EResourceType::Beer, 300);
		Recipe.LaborRequired = 12;
		Recipe.WagePerWorker = 1.2f;                       // Skilled work
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Brewery, Recipe);
	}

	// WINERY: Grapes → Wine
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Grapes, 300);
		Recipe.Inputs.Add(EResourceType::Timber, 10);      // Barrels
		Recipe.Outputs.Add(EResourceType::Wine, 250);
		Recipe.LaborRequired = 10;
		Recipe.WagePerWorker = 1.5f;                       // Luxury good = higher wages
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Winery, Recipe);
	}

	// TANNERY: Hides → Leather
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Hides, 100);
		Recipe.Inputs.Add(EResourceType::Salt, 20);        // For curing
		Recipe.Outputs.Add(EResourceType::Leather, 80);    // 20% waste
		Recipe.LaborRequired = 15;
		Recipe.WagePerWorker = 0.9f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Tannery, Recipe);
	}

	// WEAVER: Wool/Flax → Cloth
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Wool, 100);
		Recipe.Inputs.Add(EResourceType::Flax, 50);
		Recipe.Inputs.Add(EResourceType::Tools, 2);
		Recipe.Outputs.Add(EResourceType::WoolenCloth, 120);
		Recipe.Outputs.Add(EResourceType::Linen, 40);
		Recipe.LaborRequired = 20;
		Recipe.WagePerWorker = 1.0f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Weaver, Recipe);
	}

	// SMELTER: Ore + Coal → Metal Bars
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::IronOre, 150);
		Recipe.Inputs.Add(EResourceType::Charcoal, 100);   // Fuel
		Recipe.Inputs.Add(EResourceType::Tools, 3);
		Recipe.Outputs.Add(EResourceType::IronBars, 100);  // Significant waste
		Recipe.LaborRequired = 25;
		Recipe.WagePerWorker = 1.2f;                       // Hot, dangerous work
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Smelter, Recipe);
	}

	// SAWMILL: Timber → Planks
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Timber, 300);
		Recipe.Inputs.Add(EResourceType::Tools, 2);
		Recipe.Outputs.Add(EResourceType::Planks, 250);    // 16.7% waste (sawdust)
		Recipe.LaborRequired = 15;
		Recipe.WagePerWorker = 0.9f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Sawmill, Recipe);
	}

	// ========== TERTIARY PRODUCTION (Manufacturing) ==========

	// TAILOR: Cloth → Garments
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::WoolenCloth, 80);
		Recipe.Inputs.Add(EResourceType::Linen, 30);
		Recipe.Inputs.Add(EResourceType::Tools, 2);
		Recipe.Outputs.Add(EResourceType::Garments, 100);
		Recipe.LaborRequired = 12;
		Recipe.WagePerWorker = 1.3f;                       // Skilled artisan
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Tailor, Recipe);
	}

	// SHOEMAKER: Leather → Shoes
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Leather, 50);
		Recipe.Inputs.Add(EResourceType::Tools, 1);
		Recipe.Outputs.Add(EResourceType::Shoes, 60);
		Recipe.Outputs.Add(EResourceType::Boots, 20);
		Recipe.LaborRequired = 8;
		Recipe.WagePerWorker = 1.2f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Shoemaker, Recipe);
	}

	// BLACKSMITH: Iron → Tools + Weapons
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::IronBars, 80);
		Recipe.Inputs.Add(EResourceType::Charcoal, 40);
		Recipe.Inputs.Add(EResourceType::Timber, 20);      // For handles
		Recipe.Outputs.Add(EResourceType::Tools, 60);
		Recipe.Outputs.Add(EResourceType::Weapons, 30);
		Recipe.LaborRequired = 10;
		Recipe.WagePerWorker = 1.5f;                       // Highly skilled
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Blacksmith, Recipe);
	}

	// ARMORER: Iron → Armor
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::IronBars, 100);
		Recipe.Inputs.Add(EResourceType::Charcoal, 50);
		Recipe.Inputs.Add(EResourceType::Leather, 30);     // For straps
		Recipe.Outputs.Add(EResourceType::Armor, 40);
		Recipe.LaborRequired = 15;
		Recipe.WagePerWorker = 2.0f;                       // Master craftsman
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Armorer, Recipe);
	}

	// CARPENTER: Planks → Furniture
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Planks, 150);
		Recipe.Inputs.Add(EResourceType::Tools, 3);
		Recipe.Inputs.Add(EResourceType::IronBars, 10);    // Nails, hinges
		Recipe.Outputs.Add(EResourceType::Tools, 20);      // Makes tools too!
		// Note: No Furniture resource defined yet, would need to add
		Recipe.LaborRequired = 12;
		Recipe.WagePerWorker = 1.4f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Carpenter, Recipe);
	}

	// SHIPYARD: Planks + Tools → Ships
	{
		FProductionRecipe Recipe;
		Recipe.Inputs.Add(EResourceType::Planks, 500);
		Recipe.Inputs.Add(EResourceType::Tools, 50);
		Recipe.Inputs.Add(EResourceType::IronBars, 100);
		Recipe.Inputs.Add(EResourceType::WoolenCloth, 200); // Sails
		Recipe.Outputs.Add(EResourceType::Ships, 1);        // 1 ship/week = major project!
		Recipe.LaborRequired = 100;
		Recipe.WagePerWorker = 1.5f;
		Recipe.BaseEfficiency = 1.0f;
		ProductionRecipes.Add(EBuildingType_Market::Shipyard, Recipe);
	}

	UE_LOG(LogTemp, Log, TEXT("  Initialized %d production recipes"), ProductionRecipes.Num());
}

void UGTMarketEconomySubsystem::InitializePopNeeds()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing Pop Needs Templates..."));

	// PEASANT: Basic necessities only
	{
		FPopNeedsTemplate Needs;
		Needs.PopClass = EPopClass::Peasant;
		Needs.WeeklyNeeds.Add(EResourceType::Bread, 20);   // Staple food
		Needs.WeeklyNeeds.Add(EResourceType::Fish, 8);     // Protein
		Needs.WeeklyNeeds.Add(EResourceType::Beer, 5);     // Drink
		Needs.WeeklyNeeds.Add(EResourceType::Garments, 1); // Clothing (1 per 100 people)
		Needs.MinimumSatisfaction = 0.4f;                  // Tolerates hardship
		PopNeedsTemplates.Add(EPopClass::Peasant, Needs);
	}

	// CRAFTSMAN: Comfortable lifestyle
	{
		FPopNeedsTemplate Needs;
		Needs.PopClass = EPopClass::Craftsman;
		Needs.WeeklyNeeds.Add(EResourceType::Bread, 15);
		Needs.WeeklyNeeds.Add(EResourceType::Fish, 10);
		Needs.WeeklyNeeds.Add(EResourceType::SmokedMeat, 5);
		Needs.WeeklyNeeds.Add(EResourceType::Beer, 8);
		Needs.WeeklyNeeds.Add(EResourceType::Wine, 2);      // Some luxury
		Needs.WeeklyNeeds.Add(EResourceType::Garments, 2);
		Needs.WeeklyNeeds.Add(EResourceType::Shoes, 1);
		Needs.MinimumSatisfaction = 0.6f;                   // Expects comfort
		PopNeedsTemplates.Add(EPopClass::Craftsman, Needs);
	}

	// MERCHANT: Good lifestyle with luxuries
	{
		FPopNeedsTemplate Needs;
		Needs.PopClass = EPopClass::Merchant;
		Needs.WeeklyNeeds.Add(EResourceType::Bread, 12);
		Needs.WeeklyNeeds.Add(EResourceType::SmokedMeat, 10);
		Needs.WeeklyNeeds.Add(EResourceType::Wine, 8);
		Needs.WeeklyNeeds.Add(EResourceType::Beer, 5);
		Needs.WeeklyNeeds.Add(EResourceType::Garments, 3);
		Needs.WeeklyNeeds.Add(EResourceType::LuxuryCloth, 1);
		Needs.WeeklyNeeds.Add(EResourceType::Spices, 2);
		Needs.WeeklyNeeds.Add(EResourceType::VenetianGlass, 1);
		Needs.MinimumSatisfaction = 0.7f;                   // High standards
		PopNeedsTemplates.Add(EPopClass::Merchant, Needs);
	}

	// NOBLE: Luxury lifestyle
	{
		FPopNeedsTemplate Needs;
		Needs.PopClass = EPopClass::Noble;
		Needs.WeeklyNeeds.Add(EResourceType::Bread, 8);     // Eats less but better
		Needs.WeeklyNeeds.Add(EResourceType::SmokedMeat, 15);
		Needs.WeeklyNeeds.Add(EResourceType::Wine, 20);     // Lots of wine!
		Needs.WeeklyNeeds.Add(EResourceType::Spices, 5);
		Needs.WeeklyNeeds.Add(EResourceType::LuxuryCloth, 3);
		Needs.WeeklyNeeds.Add(EResourceType::Silk, 2);
		Needs.WeeklyNeeds.Add(EResourceType::VenetianGlass, 2);
		Needs.WeeklyNeeds.Add(EResourceType::Silverware, 1);
		Needs.WeeklyNeeds.Add(EResourceType::Tapestries, 1);
		Needs.MinimumSatisfaction = 0.8f;                   // Demands luxury
		PopNeedsTemplates.Add(EPopClass::Noble, Needs);
	}

	// CLERGY: Special religious needs
	{
		FPopNeedsTemplate Needs;
		Needs.PopClass = EPopClass::Clergy;
		Needs.WeeklyNeeds.Add(EResourceType::Bread, 10);
		Needs.WeeklyNeeds.Add(EResourceType::Wine, 15);     // For sacrament
		Needs.WeeklyNeeds.Add(EResourceType::Garments, 2);
		// Would add: Books, Incense, Candles (not yet defined)
		Needs.MinimumSatisfaction = 0.7f;
		PopNeedsTemplates.Add(EPopClass::Clergy, Needs);
	}

	UE_LOG(LogTemp, Log, TEXT("  Initialized %d pop needs templates"), PopNeedsTemplates.Num());
}

// ========== TURN PROCESSING ==========

void UGTMarketEconomySubsystem::ProcessMarketEconomyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("=== MARKET ECONOMY TURN START ==="));

	// 1. Buildings buy inputs
	ProcessBuildingInputPurchases();

	// 2. Buildings produce outputs
	ProcessBuildingProduction();

	// 3. Pops earn wages
	ProcessPopWages();

	// 4. Pops buy goods
	ProcessPopConsumption();

	// 5. Clear market orders (match buyers/sellers)
	for (auto& MarketPair : AllMarkets)
	{
		ClearMarketOrders(MarketPair.Key);
	}

	// 6. Adjust prices based on unmet demand
	for (auto& MarketPair : AllMarkets)
	{
		AdjustMarketPrices(MarketPair.Key);
	}

	// 7. Trade between markets
	ProcessInterMarketTrade();

	// 8. Update pop militancy
	UpdatePopMilitancy();

	UE_LOG(LogTemp, Warning, TEXT("=== MARKET ECONOMY TURN END ===\n"));
}

// ========== PRODUCTION PHASE ==========

void UGTMarketEconomySubsystem::ProcessBuildingInputPurchases()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Building Input Purchases..."));

	for (auto& BuildingPair : AllBuildings)
	{
		FMarketBuilding& Building = BuildingPair.Value;
		if (!Building.bIsActive) continue;

		FLocalMarket* Market = GetMarket(Building.MarketName);
		if (!Market) continue;

		// Try to buy inputs from market
		bool bCanProduce = true;
		for (auto& InputPair : Building.Recipe.Inputs)
		{
			EResourceType Resource = InputPair.Key;
			int32 AmountNeeded = FMath::RoundToInt(InputPair.Value * Building.CurrentEfficiency);

			FMarketGoodData* GoodData = Market->GoodsMarket.Find(Resource);
			if (GoodData && GoodData->Stockpile >= AmountNeeded)
			{
				// Buy from market
				GoodData->Stockpile -= AmountNeeded;
				GoodData->Demand += AmountNeeded;

				UE_LOG(LogTemp, Log, TEXT("  %s bought %d %s from %s market"),
					*Building.BuildingID, AmountNeeded, *UEnum::GetValueAsString(Resource),
					*Building.MarketName);
			}
			else
			{
				// Can't afford or not available
				bCanProduce = false;
				UE_LOG(LogTemp, Warning, TEXT("  %s CANNOT buy %d %s (stockpile: %d)"),
					*Building.BuildingID, AmountNeeded, *UEnum::GetValueAsString(Resource),
					GoodData ? GoodData->Stockpile : 0);
			}
		}

		Building.bIsActive = bCanProduce;
	}
}

void UGTMarketEconomySubsystem::ProcessBuildingProduction()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Building Production..."));

	for (auto& BuildingPair : AllBuildings)
	{
		FMarketBuilding& Building = BuildingPair.Value;
		if (!Building.bIsActive) continue;

		FLocalMarket* Market = GetMarket(Building.MarketName);
		if (!Market) continue;

		// Produce outputs
		for (auto& OutputPair : Building.Recipe.Outputs)
		{
			EResourceType Resource = OutputPair.Key;
			int32 AmountProduced = FMath::RoundToInt(OutputPair.Value * Building.CurrentEfficiency);

			FMarketGoodData* GoodData = Market->GoodsMarket.Find(Resource);
			if (GoodData)
			{
				// Sell to market
				GoodData->Stockpile += AmountProduced;
				GoodData->Supply += AmountProduced;

				UE_LOG(LogTemp, Log, TEXT("  %s produced %d %s, sold to %s market"),
					*Building.BuildingID, AmountProduced, *UEnum::GetValueAsString(Resource),
					*Building.MarketName);
			}
		}

		// Calculate profit
		Building.LastTurnProfit = CalculateBuildingProfit(BuildingPair.Key);
	}
}

float UGTMarketEconomySubsystem::CalculateBuildingProfit(const FString& BuildingID)
{
	FMarketBuilding* Building = GetBuilding(BuildingID);
	if (!Building) return 0.0f;

	FLocalMarket* Market = GetMarket(Building->MarketName);
	if (!Market) return 0.0f;

	float Revenue = 0.0f;
	float Costs = 0.0f;

	// Calculate revenue from outputs
	for (auto& OutputPair : Building->Recipe.Outputs)
	{
		float Price = GetMarketPrice(Building->MarketName, OutputPair.Key);
		Revenue += Price * OutputPair.Value;
	}

	// Calculate costs from inputs
	for (auto& InputPair : Building->Recipe.Inputs)
	{
		float Price = GetMarketPrice(Building->MarketName, InputPair.Key);
		Costs += Price * InputPair.Value;
	}

	// Add labor costs
	Costs += Building->Recipe.LaborRequired * Building->Recipe.WagePerWorker;

	return Revenue - Costs;
}

// ========== POP PHASE ==========

void UGTMarketEconomySubsystem::ProcessPopWages()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Pop Wages..."));

	for (auto& PopPair : AllPops)
	{
		FPopGroup& Pop = PopPair.Value;

		if (!Pop.EmployedInBuildingID.IsEmpty())
		{
			FMarketBuilding* Building = GetBuilding(Pop.EmployedInBuildingID);
			if (Building && Building->bIsActive)
			{
				// Earn wage
				float Wage = Building->Recipe.WagePerWorker;
				Pop.Cash += Wage * Pop.Size / 100.0f; // Scale by pop size
				Pop.WeeklyWage = Wage;

				UE_LOG(LogTemp, Log, TEXT("  Pop %s earned %.2fg from %s"),
					*PopPair.Key, Wage * Pop.Size / 100.0f, *Pop.EmployedInBuildingID);
			}
		}
	}
}

void UGTMarketEconomySubsystem::ProcessPopConsumption()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Pop Consumption..."));

	for (auto& PopPair : AllPops)
	{
		FPopGroup& Pop = PopPair.Value;

		FLocalMarket* Market = GetMarket(Pop.MarketName);
		if (!Market) continue;

		FPopNeedsTemplate Needs = GetPopNeeds(Pop.PopClass);
		float PopScale = Pop.Size / 100.0f;

		int32 NeedsSatisfied = 0;
		int32 TotalNeeds = 0;

		// Try to buy needed goods
		for (auto& NeedPair : Needs.WeeklyNeeds)
		{
			EResourceType Resource = NeedPair.Key;
			int32 AmountNeeded = FMath::RoundToInt(NeedPair.Value * PopScale);
			TotalNeeds++;

			FMarketGoodData* GoodData = Market->GoodsMarket.Find(Resource);
			float Price = GetMarketPrice(Pop.MarketName, Resource);
			float TotalCost = Price * AmountNeeded;

			if (GoodData && GoodData->Stockpile >= AmountNeeded && Pop.Cash >= TotalCost)
			{
				// Buy goods
				GoodData->Stockpile -= AmountNeeded;
				GoodData->Demand += AmountNeeded;
				Pop.Cash -= TotalCost;
				NeedsSatisfied++;

				UE_LOG(LogTemp, Log, TEXT("  Pop %s bought %d %s for %.2fg"),
					*PopPair.Key, AmountNeeded, *UEnum::GetValueAsString(Resource), TotalCost);
			}
		}

		// Calculate satisfaction
		Pop.NeedsSatisfaction = TotalNeeds > 0 ? (float)NeedsSatisfied / TotalNeeds : 0.5f;
	}
}

float UGTMarketEconomySubsystem::CalculatePopSatisfaction(const FString& PopID)
{
	FPopGroup* Pop = GetPop(PopID);
	return Pop ? Pop->NeedsSatisfaction : 0.0f;
}

void UGTMarketEconomySubsystem::UpdatePopMilitancy()
{
	for (auto& PopPair : AllPops)
	{
		FPopGroup& Pop = PopPair.Value;

		FPopNeedsTemplate Needs = GetPopNeeds(Pop.PopClass);

		// Low satisfaction → higher militancy
		if (Pop.NeedsSatisfaction < Needs.MinimumSatisfaction)
		{
			Pop.Militancy += 0.01f; // +1% militancy per turn of unmet needs
			Pop.Militancy = FMath::Clamp(Pop.Militancy, 0.0f, 1.0f);
		}
		else if (Pop.NeedsSatisfaction > 0.8f)
		{
			// High satisfaction → lower militancy
			Pop.Militancy -= 0.005f;
			Pop.Militancy = FMath::Clamp(Pop.Militancy, 0.0f, 1.0f);
		}
	}
}

// ========== MARKET CLEARING ==========

void UGTMarketEconomySubsystem::ClearMarketOrders(const FString& MarketName)
{
	// Orders already processed in input/output phases
	// This is where we'd match complex buy/sell orders if needed
}

void UGTMarketEconomySubsystem::AdjustMarketPrices(const FString& MarketName)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return;

	for (auto& GoodPair : Market->GoodsMarket)
	{
		FMarketGoodData& Good = GoodPair.Value;

		// Calculate supply/demand ratio
		float SupplyDemandRatio = Good.Demand > 0 ?
			(float)Good.Supply / (float)Good.Demand : 1.0f;

		// Adjust price based on ratio
		if (SupplyDemandRatio < 0.5f)
		{
			// High demand, low supply → price increase
			Good.Price *= 1.1f; // +10%
		}
		else if (SupplyDemandRatio < 0.8f)
		{
			Good.Price *= 1.05f; // +5%
		}
		else if (SupplyDemandRatio > 2.0f)
		{
			// Low demand, high supply → price decrease
			Good.Price *= 0.9f; // -10%
		}
		else if (SupplyDemandRatio > 1.2f)
		{
			Good.Price *= 0.95f; // -5%
		}

		// Clamp price to reasonable range
		float BasePrice = GetBasePrice(GoodPair.Key);
		Good.Price = FMath::Clamp(Good.Price, BasePrice * 0.2f, BasePrice * 5.0f);

		// Reset supply/demand for next turn
		Good.Supply = 0;
		Good.Demand = 0;
	}
}

// ========== TRADE BETWEEN MARKETS ==========

void UGTMarketEconomySubsystem::ProcessInterMarketTrade()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Inter-Market Trade..."));

	// Simple arbitrage: goods flow from low-price to high-price markets
	for (FMarketTradeRoute& Route : TradeRoutes)
	{
		if (!Route.bIsActive) continue;

		FLocalMarket* MarketA = GetMarket(Route.MarketA);
		FLocalMarket* MarketB = GetMarket(Route.MarketB);
		if (!MarketA || !MarketB) continue;

		// Check each resource type for arbitrage
		for (int32 i = 0; i < (int32)EResourceType::MAX; i++)
		{
			EResourceType Resource = (EResourceType)i;

			float PriceDiff = CalculatePriceDifference(Route.MarketA, Route.MarketB, Resource);
			if (PriceDiff > Route.TransportCost)
			{
				// Profitable to trade
				FMarketGoodData* GoodA = MarketA->GoodsMarket.Find(Resource);
				FMarketGoodData* GoodB = MarketB->GoodsMarket.Find(Resource);

				if (GoodA && GoodB && GoodA->Stockpile > 10)
				{
					int32 TradeAmount = FMath::Min(GoodA->Stockpile / 2, Route.TradeCapacity / 10);
					TransferGoodsBetweenMarkets(Route.MarketA, Route.MarketB, Resource, TradeAmount);
				}
			}
		}
	}
}

float UGTMarketEconomySubsystem::CalculatePriceDifference(const FString& MarketA, const FString& MarketB,
	EResourceType Resource)
{
	float PriceA = GetMarketPrice(MarketA, Resource);
	float PriceB = GetMarketPrice(MarketB, Resource);
	return PriceB - PriceA;
}

bool UGTMarketEconomySubsystem::TransferGoodsBetweenMarkets(const FString& FromMarket, const FString& ToMarket,
	EResourceType Resource, int32 Amount)
{
	FLocalMarket* From = GetMarket(FromMarket);
	FLocalMarket* To = GetMarket(ToMarket);

	if (!From || !To) return false;

	FMarketGoodData* FromGood = From->GoodsMarket.Find(Resource);
	FMarketGoodData* ToGood = To->GoodsMarket.Find(Resource);

	if (FromGood && ToGood && FromGood->Stockpile >= Amount)
	{
		FromGood->Stockpile -= Amount;
		ToGood->Stockpile += Amount;

		UE_LOG(LogTemp, Log, TEXT("  Traded %d %s: %s -> %s"),
			Amount, *UEnum::GetValueAsString(Resource), *FromMarket, *ToMarket);

		return true;
	}

	return false;
}

// ========== BUILDING MANAGEMENT ==========

FString UGTMarketEconomySubsystem::ConstructBuilding(const FString& MarketName,
	EBuildingType_Market BuildingType, const FString& OwnerID, float& OutConstructionCost)
{
	FString BuildingID = FString::Printf(TEXT("%s_%s_%d"),
		*MarketName, *UEnum::GetValueAsString(BuildingType), AllBuildings.Num());

	FMarketBuilding NewBuilding;
	NewBuilding.BuildingID = BuildingID;
	NewBuilding.BuildingType = BuildingType;
	NewBuilding.MarketName = MarketName;
	NewBuilding.OwnerID = OwnerID;
	NewBuilding.Recipe = GetRecipe(BuildingType);
	NewBuilding.ProductionMethod = EProductionMethod::Manual;
	NewBuilding.CurrentEfficiency = 1.0f;
	NewBuilding.bIsActive = true;

	AllBuildings.Add(BuildingID, NewBuilding);

	OutConstructionCost = 1000.0f; // TODO: Calculate based on building type

	UE_LOG(LogTemp, Warning, TEXT("Built %s in %s (Owner: %s)"),
		*UEnum::GetValueAsString(BuildingType), *MarketName, *OwnerID);

	return BuildingID;
}

FMarketBuilding* UGTMarketEconomySubsystem::GetBuilding(const FString& BuildingID)
{
	return AllBuildings.Find(BuildingID);
}

// ========== POP MANAGEMENT ==========

FString UGTMarketEconomySubsystem::CreatePopGroup(const FString& MarketName, EPopClass PopClass, int32 Size)
{
	FString PopID = FString::Printf(TEXT("Pop_%s_%s_%d"),
		*MarketName, *UEnum::GetValueAsString(PopClass), AllPops.Num());

	FPopGroup NewPop;
	NewPop.PopID = PopID;
	NewPop.PopClass = PopClass;
	NewPop.Size = Size;
	NewPop.MarketName = MarketName;
	NewPop.Cash = 50.0f * (Size / 100.0f); // Start with some money
	NewPop.NeedsSatisfaction = 0.5f;

	AllPops.Add(PopID, NewPop);

	UE_LOG(LogTemp, Log, TEXT("Created pop group: %d %s in %s"),
		Size, *UEnum::GetValueAsString(PopClass), *MarketName);

	return PopID;
}

FPopGroup* UGTMarketEconomySubsystem::GetPop(const FString& PopID)
{
	return AllPops.Find(PopID);
}

// ========== MARKET QUERIES ==========

FLocalMarket* UGTMarketEconomySubsystem::GetMarket(const FString& MarketName)
{
	return AllMarkets.Find(MarketName);
}

float UGTMarketEconomySubsystem::GetMarketPrice(const FString& MarketName, EResourceType Resource)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return 0.0f;

	FMarketGoodData* Good = Market->GoodsMarket.Find(Resource);
	return Good ? Good->Price : 0.0f;
}

int32 UGTMarketEconomySubsystem::GetMarketSupply(const FString& MarketName, EResourceType Resource)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return 0;

	FMarketGoodData* Good = Market->GoodsMarket.Find(Resource);
	return Good ? Good->Stockpile : 0;
}

int32 UGTMarketEconomySubsystem::GetMarketDemand(const FString& MarketName, EResourceType Resource)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return 0;

	FMarketGoodData* Good = Market->GoodsMarket.Find(Resource);
	return Good ? Good->Demand : 0;
}

// ========== DEBUGGING ==========

void UGTMarketEconomySubsystem::PrintMarketState(const FString& MarketName)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return;

	UE_LOG(LogTemp, Display, TEXT("========== MARKET: %s =========="), *MarketName);
	UE_LOG(LogTemp, Display, TEXT("Size: %.1f, Efficiency: %.2f"),
		Market->MarketSize, Market->MarketEfficiency);

	// Print top 10 goods by stockpile
	TArray<TPair<EResourceType, FMarketGoodData>> SortedGoods;
	for (auto& Pair : Market->GoodsMarket)
	{
		if (Pair.Value.Stockpile > 0 || Pair.Value.Demand > 0)
		{
			SortedGoods.Add(TPair<EResourceType, FMarketGoodData>(Pair.Key, Pair.Value));
		}
	}

	SortedGoods.Sort([](const TPair<EResourceType, FMarketGoodData>& A,
		const TPair<EResourceType, FMarketGoodData>& B)
	{
		return A.Value.Stockpile > B.Value.Stockpile;
	});

	UE_LOG(LogTemp, Display, TEXT("Top Goods:"));
	for (int32 i = 0; i < FMath::Min(10, SortedGoods.Num()); i++)
	{
		auto& Pair = SortedGoods[i];
		UE_LOG(LogTemp, Display, TEXT("  %s: Price %.2fg, Stock %d, Demand %d"),
			*UEnum::GetValueAsString(Pair.Key),
			Pair.Value.Price, Pair.Value.Stockpile, Pair.Value.Demand);
	}
}

void UGTMarketEconomySubsystem::PrintBuildingEconomics(const FString& BuildingID)
{
	FMarketBuilding* Building = GetBuilding(BuildingID);
	if (!Building) return;

	UE_LOG(LogTemp, Display, TEXT("========== BUILDING: %s =========="), *BuildingID);
	UE_LOG(LogTemp, Display, TEXT("Type: %s, Owner: %s, Market: %s"),
		*UEnum::GetValueAsString(Building->BuildingType),
		*Building->OwnerID, *Building->MarketName);
	UE_LOG(LogTemp, Display, TEXT("Efficiency: %.2f, Profit: %.2fg"),
		Building->CurrentEfficiency, Building->LastTurnProfit);
	UE_LOG(LogTemp, Display, TEXT("Workers: %d, Wage: %.2fg/worker"),
		Building->Recipe.LaborRequired, Building->Recipe.WagePerWorker);
}

// ========== HELPER FUNCTIONS ==========

FProductionRecipe UGTMarketEconomySubsystem::GetRecipe(EBuildingType_Market BuildingType)
{
	FProductionRecipe* Recipe = ProductionRecipes.Find(BuildingType);
	return Recipe ? *Recipe : FProductionRecipe();
}

FPopNeedsTemplate UGTMarketEconomySubsystem::GetPopNeeds(EPopClass PopClass)
{
	FPopNeedsTemplate* Needs = PopNeedsTemplates.Find(PopClass);
	return Needs ? *Needs : FPopNeedsTemplate();
}

float UGTMarketEconomySubsystem::GetBasePrice(EResourceType Resource)
{
	// Base prices for different resource tiers
	switch (Resource)
	{
		// Tier 1 - Raw resources: 1-3g
		case EResourceType::Grain:
		case EResourceType::Timber:
		case EResourceType::Fish:
			return 2.0f;

		// Tier 2 - Processed: 5-10g
		case EResourceType::Flour:
		case EResourceType::Leather:
		case EResourceType::Planks:
			return 7.0f;

		// Tier 3 - Manufactured: 15-30g
		case EResourceType::Bread:
		case EResourceType::Beer:
		case EResourceType::Garments:
		case EResourceType::Tools:
			return 20.0f;

		// Tier 4 - Luxury: 50-200g
		case EResourceType::Wine:
		case EResourceType::Silk:
		case EResourceType::Spices:
			return 100.0f;

		default:
			return 10.0f;
	}
}

bool UGTMarketEconomySubsystem::CanAffordInputs(const FMarketBuilding& Building, const FLocalMarket& Market)
{
	// Check if all inputs are available
	for (auto& InputPair : Building.Recipe.Inputs)
	{
		const FMarketGoodData* Good = Market.GoodsMarket.Find(InputPair.Key);
		if (!Good || Good->Stockpile < InputPair.Value)
		{
			return false;
		}
	}
	return true;
}

bool UGTMarketEconomySubsystem::AssignWorkersToBuilding(const FString& BuildingID, int32 NumWorkers)
{
	// TODO: Implement worker assignment
	return false;
}

bool UGTMarketEconomySubsystem::PromotePop(const FString& PopID, EPopClass NewClass)
{
	// TODO: Implement pop promotion
	return false;
}

float UGTMarketEconomySubsystem::CalculateWage(EPopClass PopClass, const FString& MarketName)
{
	// TODO: Implement dynamic wage calculation
	return 1.0f;
}

TArray<FString> UGTMarketEconomySubsystem::FindAvailableWorkers(const FString& MarketName, int32 NumNeeded)
{
	// TODO: Implement worker finding
	return TArray<FString>();
}

float UGTMarketEconomySubsystem::GetSupplyDemandRatio(const FString& MarketName, EResourceType Resource)
{
	FLocalMarket* Market = GetMarket(MarketName);
	if (!Market) return 1.0f;

	FMarketGoodData* Good = Market->GoodsMarket.Find(Resource);
	if (!Good || Good->Demand == 0) return 1.0f;

	return (float)Good->Supply / (float)Good->Demand;
}

void UGTMarketEconomySubsystem::CreateBuyOrder(const FString& MarketName, EResourceType Resource,
	int32 Amount, const FString& BuyerID)
{
	// Orders processed immediately in current implementation
}

void UGTMarketEconomySubsystem::CreateSellOrder(const FString& MarketName, EResourceType Resource,
	int32 Amount, const FString& SellerID)
{
	// Orders processed immediately in current implementation
}
