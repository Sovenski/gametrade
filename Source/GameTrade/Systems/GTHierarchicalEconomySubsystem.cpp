// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTHierarchicalEconomySubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGTHierarchicalEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("GTHierarchicalEconomySubsystem Initialized"));

	InitializePopulationNeeds();
}

void UGTHierarchicalEconomySubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("GTHierarchicalEconomySubsystem Deinitialized"));
}

// ========== HIERARCHY MANAGEMENT ==========

void UGTHierarchicalEconomySubsystem::BuildHierarchyFromLegacyData()
{
	UE_LOG(LogTemp, Warning, TEXT("Building Settlement Hierarchy..."));

	// Example: Create HRE structure
	// Empire: Holy Roman Empire
	FEmpireData HRE;
	HRE.EmpireName = TEXT("Holy Roman Empire");
	HRE.ImperialTariff = 0.15f; // High due to fragmentation
	HRE.PoliticalComplexity = 0.9f; // Very complex!
	AllEmpires.Add(TEXT("HRE"), HRE);

	// Kingdom: Bavaria
	FKingdomData Bavaria;
	Bavaria.KingdomName = TEXT("Kingdom of Bavaria");
	Bavaria.BaseTariff = 0.08f;
	Bavaria.ParentEmpireID = TEXT("HRE");
	AllKingdoms.Add(TEXT("Bavaria"), Bavaria);

	// Duchy: Upper Bavaria
	FDuchyData UpperBavaria;
	UpperBavaria.DuchyName = TEXT("Duchy of Upper Bavaria");
	UpperBavaria.ParentKingdomID = TEXT("Bavaria");
	UpperBavaria.CapitalHoldingID = TEXT("München_City");
	AllDuchies.Add(TEXT("UpperBavaria"), UpperBavaria);

	// County: München County
	FCountyData MunichCounty;
	MunichCounty.CountyName = TEXT("County of München");
	MunichCounty.ParentDuchyID = TEXT("UpperBavaria");
	AllCounties.Add(TEXT("MunichCounty"), MunichCounty);

	// Holdings in München County
	// 1. München City (player can own)
	FHoldingData Munich;
	Munich.HoldingName = TEXT("München");
	Munich.HoldingType = EHoldingType::City;
	Munich.Population = 15000;
	Munich.Geography = EGeographyType::Plains;
	Munich.ParentCountyID = TEXT("MunichCounty");

	// München produces beer and tools
	Munich.LocalProduction.Add(EResourceType::Beer, 200);
	Munich.LocalProduction.Add(EResourceType::Tools, 50);
	Munich.LocalProduction.Add(EResourceType::Garments, 30);

	// Population needs
	Munich.PopulationNeeds.Add(EResourceType::Bread, 300); // 2 per 100 people
	Munich.PopulationNeeds.Add(EResourceType::Beer, 150);
	Munich.PopulationNeeds.Add(EResourceType::Grain, 100);
	Munich.PopulationNeeds.Add(EResourceType::Fish, 80);

	AllHoldings.Add(TEXT("München_City"), Munich);

	// 2. Nearby village (produces grain)
	FHoldingData DachauVillage;
	DachauVillage.HoldingName = TEXT("Dachau");
	DachauVillage.HoldingType = EHoldingType::Village;
	DachauVillage.Population = 800;
	DachauVillage.Geography = EGeographyType::Plains;
	DachauVillage.ParentCountyID = TEXT("MunichCounty");
	DachauVillage.LocalProduction.Add(EResourceType::Grain, 500); // Farming village
	DachauVillage.LocalProduction.Add(EResourceType::Cattle, 50);
	DachauVillage.PopulationNeeds.Add(EResourceType::Bread, 16);
	DachauVillage.PopulationNeeds.Add(EResourceType::Tools, 5);
	AllHoldings.Add(TEXT("Dachau_Village"), DachauVillage);

	// 3. Freising Castle (feudal lord)
	FHoldingData FreisingCastle;
	FreisingCastle.HoldingName = TEXT("Freising");
	FreisingCastle.HoldingType = EHoldingType::Castle;
	FreisingCastle.Population = 1200;
	FreisingCastle.Geography = EGeographyType::Hills;
	FreisingCastle.ParentCountyID = TEXT("MunichCounty");
	FreisingCastle.LocalProduction.Add(EResourceType::Weapons, 20);
	FreisingCastle.LocalProduction.Add(EResourceType::Armor, 10);
	FreisingCastle.PopulationNeeds.Add(EResourceType::Bread, 24);
	FreisingCastle.PopulationNeeds.Add(EResourceType::Wine, 15);
	AllHoldings.Add(TEXT("Freising_Castle"), FreisingCastle);

	// Link holdings to county
	MunichCounty.HoldingIDs.Add(TEXT("München_City"));
	MunichCounty.HoldingIDs.Add(TEXT("Dachau_Village"));
	MunichCounty.HoldingIDs.Add(TEXT("Freising_Castle"));

	UE_LOG(LogTemp, Warning, TEXT("  Created: 1 Empire, 1 Kingdom, 1 Duchy, 1 County, 3 Holdings"));

	// Generate passive trade network between these holdings
	GeneratePassiveTradeNetwork();
}

void UGTHierarchicalEconomySubsystem::AddHolding(const FHoldingData& HoldingData)
{
	// Generate unique ID for holding if not provided
	FString HoldingID = HoldingData.HoldingName + TEXT("_") +
		UEnum::GetValueAsString(HoldingData.HoldingType);

	// Add to holdings map
	AllHoldings.Add(HoldingID, HoldingData);

	UE_LOG(LogTemp, Log, TEXT("Added holding: %s (Type: %s, Pop: %d)"),
		*HoldingData.HoldingName,
		*UEnum::GetValueAsString(HoldingData.HoldingType),
		HoldingData.Population);
}

FHoldingData* UGTHierarchicalEconomySubsystem::GetHolding(const FString& HoldingID)
{
	return AllHoldings.Find(HoldingID);
}

bool UGTHierarchicalEconomySubsystem::GetHoldingCopy(const FString& HoldingID, FHoldingData& OutHolding)
{
	FHoldingData* Holding = AllHoldings.Find(HoldingID);
	if (Holding)
	{
		OutHolding = *Holding;
		return true;
	}
	return false;
}

TArray<FHoldingData*> UGTHierarchicalEconomySubsystem::GetHoldingsInCounty(const FString& CountyID)
{
	TArray<FHoldingData*> Holdings;
	FCountyData* County = AllCounties.Find(CountyID);
	if (County)
	{
		for (const FString& HoldingID : County->HoldingIDs)
		{
			if (FHoldingData* Holding = AllHoldings.Find(HoldingID))
			{
				Holdings.Add(Holding);
			}
		}
	}
	return Holdings;
}

TArray<FCountyData*> UGTHierarchicalEconomySubsystem::GetCountiesInDuchy(const FString& DuchyID)
{
	TArray<FCountyData*> Counties;
	FDuchyData* Duchy = AllDuchies.Find(DuchyID);
	if (Duchy)
	{
		for (const FString& CountyID : Duchy->CountyIDs)
		{
			if (FCountyData* County = AllCounties.Find(CountyID))
			{
				Counties.Add(County);
			}
		}
	}
	return Counties;
}

// ========== PASSIVE TRADE SYSTEM ==========

void UGTHierarchicalEconomySubsystem::GeneratePassiveTradeNetwork()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Passive Trade Network..."));

	PassiveTradeNetwork.Empty();

	// Create connections between all holdings in same county (always connected)
	for (auto& CountyPair : AllCounties)
	{
		FCountyData& County = CountyPair.Value;

		for (int32 i = 0; i < County.HoldingIDs.Num(); i++)
		{
			for (int32 j = i + 1; j < County.HoldingIDs.Num(); j++)
			{
				FString HoldingA = County.HoldingIDs[i];
				FString HoldingB = County.HoldingIDs[j];

				if (CanPassivelyTrade(HoldingA, HoldingB))
				{
					FPassiveTradeConnection Connection;
					Connection.HoldingA = HoldingA;
					Connection.HoldingB = HoldingB;
					Connection.Distance = CalculateDistance(HoldingA, HoldingB);
					Connection.GeographyModifier = CalculateGeographyModifier(HoldingA, HoldingB);

					PassiveTradeNetwork.Add(Connection);

					UE_LOG(LogTemp, Log, TEXT("  Passive trade: %s <-> %s (%.1f km, mod: %.2f)"),
						*HoldingA, *HoldingB, Connection.Distance, Connection.GeographyModifier);
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Generated %d passive trade connections"), PassiveTradeNetwork.Num());
}

bool UGTHierarchicalEconomySubsystem::CanPassivelyTrade(const FString& HoldingA, const FString& HoldingB)
{
	FHoldingData* A = GetHolding(HoldingA);
	FHoldingData* B = GetHolding(HoldingB);

	if (!A || !B) return false;

	// Same county = always can trade
	if (A->ParentCountyID == B->ParentCountyID) return true;

	// Check geography blocking
	if (IsGeographyBlocking(A->Geography, B->Geography)) return false;

	// Check distance (max 100km for passive trade)
	float Distance = CalculateDistance(HoldingA, HoldingB);
	if (Distance > 100.0f) return false;

	return true;
}

void UGTHierarchicalEconomySubsystem::ProcessPassiveTrade()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Passive Trade..."));

	for (FPassiveTradeConnection& Connection : PassiveTradeNetwork)
	{
		FHoldingData* A = GetHolding(Connection.HoldingA);
		FHoldingData* B = GetHolding(Connection.HoldingB);

		if (!A || !B) continue;

		// Calculate what A needs that B has surplus of
		for (auto& NeedPair : A->PopulationNeeds)
		{
			EResourceType Resource = NeedPair.Key;
			int32 AmountNeeded = NeedPair.Value;
			int32 CurrentStockpile = A->Stockpile.FindRef(Resource);

			// A needs more of this resource
			if (CurrentStockpile < AmountNeeded)
			{
				int32 Deficit = AmountNeeded - CurrentStockpile;
				int32 BSurplus = B->Stockpile.FindRef(Resource);

				if (BSurplus > 0)
				{
					// Trade flow from B to A
					int32 TradeAmount = FMath::Min(Deficit, BSurplus / 2); // B keeps half

					// Apply geography modifier
					TradeAmount = FMath::RoundToInt(TradeAmount * (1.0f + Connection.GeographyModifier));

					if (TradeAmount > 0)
					{
						B->Stockpile[Resource] -= TradeAmount;
						A->Stockpile[Resource] = A->Stockpile.FindRef(Resource) + TradeAmount;

						Connection.GoodsFlowBtoA.Add(Resource, TradeAmount);

						UE_LOG(LogTemp, Log, TEXT("  Passive trade: %d %s from %s -> %s"),
							TradeAmount, *UEnum::GetValueAsString(Resource),
							*Connection.HoldingB, *Connection.HoldingA);
					}
				}
			}
		}

		// Reverse: Calculate what B needs that A has surplus of
		for (auto& NeedPair : B->PopulationNeeds)
		{
			EResourceType Resource = NeedPair.Key;
			int32 AmountNeeded = NeedPair.Value;
			int32 CurrentStockpile = B->Stockpile.FindRef(Resource);

			if (CurrentStockpile < AmountNeeded)
			{
				int32 Deficit = AmountNeeded - CurrentStockpile;
				int32 ASurplus = A->Stockpile.FindRef(Resource);

				if (ASurplus > 0)
				{
					int32 TradeAmount = FMath::Min(Deficit, ASurplus / 2);
					TradeAmount = FMath::RoundToInt(TradeAmount * (1.0f + Connection.GeographyModifier));

					if (TradeAmount > 0)
					{
						A->Stockpile[Resource] -= TradeAmount;
						B->Stockpile[Resource] = B->Stockpile.FindRef(Resource) + TradeAmount;

						Connection.GoodsFlowAtoB.Add(Resource, TradeAmount);
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Passive trade processing complete"));
}

// ========== POPULATION & GROWTH ==========

float UGTHierarchicalEconomySubsystem::CalculateNeedsSatisfaction(const FString& HoldingID)
{
	FHoldingData* Holding = GetHolding(HoldingID);
	if (!Holding) return 0.0f;

	float TotalNeeds = 0.0f;
	float TotalSatisfied = 0.0f;

	for (auto& NeedPair : Holding->PopulationNeeds)
	{
		EResourceType Resource = NeedPair.Key;
		int32 AmountNeeded = NeedPair.Value;
		int32 AmountAvailable = Holding->Stockpile.FindRef(Resource);

		TotalNeeds += AmountNeeded;
		TotalSatisfied += FMath::Min(AmountNeeded, AmountAvailable);
	}

	if (TotalNeeds == 0.0f) return 1.0f;

	return TotalSatisfied / TotalNeeds;
}

void UGTHierarchicalEconomySubsystem::ProcessPopulationGrowth()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Population Growth..."));

	for (auto& HoldingPair : AllHoldings)
	{
		FHoldingData& Holding = HoldingPair.Value;

		// Calculate needs satisfaction
		Holding.NeedsSatisfactionPercent = CalculateNeedsSatisfaction(HoldingPair.Key);

		// Calculate growth rate
		Holding.PopulationGrowthRate = CalculateGrowthRate(Holding.NeedsSatisfactionPercent);

		// Apply growth
		if (Holding.PopulationGrowthRate != 0.0f)
		{
			int32 OldPop = Holding.Population;
			Holding.Population = FMath::RoundToInt(Holding.Population * (1.0f + Holding.PopulationGrowthRate));

			if (Holding.Population != OldPop)
			{
				UE_LOG(LogTemp, Log, TEXT("  %s: %d -> %d (%.1f%% satisfaction, %.2f%% growth)"),
					*Holding.HoldingName, OldPop, Holding.Population,
					Holding.NeedsSatisfactionPercent * 100.0f,
					Holding.PopulationGrowthRate * 100.0f);
			}
		}

		// Update population needs based on new population
		UpdatePopulationNeeds(HoldingPair.Key);
	}
}

void UGTHierarchicalEconomySubsystem::UpdatePopulationNeeds(const FString& HoldingID)
{
	FHoldingData* Holding = GetHolding(HoldingID);
	if (!Holding) return;

	// Calculate needs per 100 people, scale to actual population
	float PopulationFactor = Holding->Population / 100.0f;

	// Basic needs
	for (auto& NeedPair : PopulationNeeds.BasicNeeds)
	{
		Holding->PopulationNeeds.Add(NeedPair.Key, FMath::RoundToInt(NeedPair.Value * PopulationFactor));
	}

	// Comfort needs (only if basic needs satisfied)
	if (Holding->NeedsSatisfactionPercent > 0.7f)
	{
		for (auto& NeedPair : PopulationNeeds.ComfortNeeds)
		{
			Holding->PopulationNeeds.Add(NeedPair.Key, FMath::RoundToInt(NeedPair.Value * PopulationFactor));
		}
	}

	// Luxury needs (only if thriving)
	if (Holding->NeedsSatisfactionPercent > 0.9f)
	{
		for (auto& NeedPair : PopulationNeeds.LuxuryNeeds)
		{
			Holding->PopulationNeeds.Add(NeedPair.Key, FMath::RoundToInt(NeedPair.Value * PopulationFactor));
		}
	}
}

// ========== REGIONAL ECONOMIES ==========

void UGTHierarchicalEconomySubsystem::AggregateCountyEconomy(const FString& CountyID)
{
	FCountyData* County = AllCounties.Find(CountyID);
	if (!County) return;

	// Clear previous aggregates
	County->RegionalSupply.Empty();
	County->RegionalDemand.Empty();

	// Sum up all holdings in county
	for (const FString& HoldingID : County->HoldingIDs)
	{
		FHoldingData* Holding = GetHolding(HoldingID);
		if (!Holding) continue;

		// Add production to supply
		for (auto& ProdPair : Holding->LocalProduction)
		{
			int32& Supply = County->RegionalSupply.FindOrAdd(ProdPair.Key);
			Supply += ProdPair.Value;
		}

		// Add needs to demand
		for (auto& NeedPair : Holding->PopulationNeeds)
		{
			int32& Demand = County->RegionalDemand.FindOrAdd(NeedPair.Key);
			Demand += NeedPair.Value;
		}
	}

	// Calculate regional prices based on supply/demand
	for (auto& SupplyPair : County->RegionalSupply)
	{
		EResourceType Resource = SupplyPair.Key;
		int32 Supply = SupplyPair.Value;
		int32 Demand = County->RegionalDemand.FindRef(Resource);

		if (Demand > 0)
		{
			float SupplyDemandRatio = (float)Supply / (float)Demand;
			float BasePrice = 5.0f; // TODO: Get from resource data

			// Price inversely related to supply/demand ratio
			float Price = BasePrice / FMath::Max(SupplyDemandRatio, 0.1f);
			County->RegionalPrices.Add(Resource, Price);
		}
	}
}

void UGTHierarchicalEconomySubsystem::AggregateDuchyEconomy(const FString& DuchyID)
{
	// Similar to county but aggregates counties
	// TODO: Implement duchy-level aggregation
}

void UGTHierarchicalEconomySubsystem::AggregateKingdomEconomy(const FString& KingdomID)
{
	// Kingdom-level aggregation
	// TODO: Implement kingdom-level aggregation
}

void UGTHierarchicalEconomySubsystem::ProcessLayeredEconomies()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Layered Economies..."));

	// 1. Holdings produce goods (already done in production system)

	// 2. Aggregate to county level
	for (auto& CountyPair : AllCounties)
	{
		AggregateCountyEconomy(CountyPair.Key);
	}

	// 3. Aggregate to duchy level
	for (auto& DuchyPair : AllDuchies)
	{
		AggregateDuchyEconomy(DuchyPair.Key);
	}

	// 4. Aggregate to kingdom level
	for (auto& KingdomPair : AllKingdoms)
	{
		AggregateKingdomEconomy(KingdomPair.Key);
	}

	UE_LOG(LogTemp, Log, TEXT("  Layered economy processing complete"));
}

// ========== CUSTOM TRADE ROUTES ==========

FString UGTHierarchicalEconomySubsystem::CreateCustomTradeRoute(
	const FString& OriginID, const FString& DestinationID,
	const TMap<EResourceType, int32>& GoodsToTransport, const FString& PlayerFamilyID)
{
	FString RouteID = FString::Printf(TEXT("Route_%s_%s_%d"),
		*OriginID, *DestinationID, FMath::Rand());

	FCustomTradeRoute NewRoute;
	NewRoute.RouteID = RouteID;
	NewRoute.RouteType = ETradeRouteType::PlayerCustom;
	NewRoute.OriginHoldingID = OriginID;
	NewRoute.DestinationHoldingID = DestinationID;
	NewRoute.TargetGoodsPerWeek = GoodsToTransport;
	NewRoute.bIsActive = true;

	// Player starts with 100% ownership
	NewRoute.OwnershipShares.Add(PlayerFamilyID, 1.0f);

	CustomTradeRoutes.Add(RouteID, NewRoute);

	UE_LOG(LogTemp, Warning, TEXT("Created custom trade route: %s -> %s"), *OriginID, *DestinationID);

	return RouteID;
}

void UGTHierarchicalEconomySubsystem::ProcessCustomTradeRoutes()
{
	UE_LOG(LogTemp, Log, TEXT("Processing Custom Trade Routes..."));

	for (auto& RoutePair : CustomTradeRoutes)
	{
		FCustomTradeRoute& Route = RoutePair.Value;

		if (!Route.bIsActive || Route.bIsBlocked) continue;

		FHoldingData* Origin = GetHolding(Route.OriginHoldingID);
		FHoldingData* Destination = GetHolding(Route.DestinationHoldingID);

		if (!Origin || !Destination) continue;

		// Transport goods
		for (auto& GoodsPair : Route.TargetGoodsPerWeek)
		{
			EResourceType Resource = GoodsPair.Key;
			int32 TargetAmount = GoodsPair.Value;

			int32 AvailableAtOrigin = Origin->Stockpile.FindRef(Resource);
			int32 ActualTransport = FMath::Min(TargetAmount, AvailableAtOrigin);

			if (ActualTransport > 0)
			{
				// Move goods
				Origin->Stockpile[Resource] -= ActualTransport;
				Destination->Stockpile[Resource] = Destination->Stockpile.FindRef(Resource) + ActualTransport;

				UE_LOG(LogTemp, Log, TEXT("  Route %s: Transported %d %s"),
					*Route.RouteID, ActualTransport, *UEnum::GetValueAsString(Resource));
			}
		}

		// Calculate profit
		Route.WeeklyProfit = CalculateRouteProfit(Route.RouteID);

		// Distribute profit to owners
		DistributeRouteProfit(Route.RouteID, Route.WeeklyProfit);
	}
}

float UGTHierarchicalEconomySubsystem::CalculateRouteProfit(const FString& RouteID)
{
	FCustomTradeRoute* Route = CustomTradeRoutes.Find(RouteID);
	if (!Route) return 0.0f;

	// Simple profit calculation: price difference * volume - tariffs
	float Profit = 0.0f;

	for (auto& GoodsPair : Route->TargetGoodsPerWeek)
	{
		float PriceDiff = CalculatePriceDifference(
			Route->OriginHoldingID, Route->DestinationHoldingID, GoodsPair.Key);

		Profit += PriceDiff * GoodsPair.Value;
	}

	// Subtract tariffs
	float Tariff = CalculateTariffs(Route->OriginHoldingID, Route->DestinationHoldingID);
	Profit *= (1.0f - Tariff);

	return Profit;
}

// ========== TRADE ROUTE OWNERSHIP ==========

bool UGTHierarchicalEconomySubsystem::BuyRouteShares(const FString& RouteID,
	const FString& BuyerFamilyID, float SharePercentage, float Price)
{
	FCustomTradeRoute* Route = CustomTradeRoutes.Find(RouteID);
	if (!Route) return false;

	// TODO: Implement ownership transfer logic
	// Check buyer has gold, find seller, transfer shares

	UE_LOG(LogTemp, Warning, TEXT("%s bought %.1f%% of route %s for %.2fg"),
		*BuyerFamilyID, SharePercentage * 100.0f, *RouteID, Price);

	return true;
}

bool UGTHierarchicalEconomySubsystem::SellRouteShares(const FString& RouteID,
	const FString& SellerFamilyID, float SharePercentage)
{
	// Create share offer
	FTradeShareOffer Offer;
	Offer.RouteID = RouteID;
	Offer.SellerFamilyID = SellerFamilyID;
	Offer.SharePercentage = SharePercentage;
	Offer.AskingPrice = 1000.0f; // TODO: Calculate fair price
	Offer.WeeksValid = 4;

	ActiveShareOffers.Add(Offer);

	UE_LOG(LogTemp, Warning, TEXT("%s offering %.1f%% of route %s for %.2fg"),
		*SellerFamilyID, SharePercentage * 100.0f, *RouteID, Offer.AskingPrice);

	return true;
}

bool UGTHierarchicalEconomySubsystem::UsurpTradeRoute(const FString& RouteID,
	const FString& UsurperFamilyID, const FString& FeudalLordID, float BribeAmount)
{
	FCustomTradeRoute* Route = CustomTradeRoutes.Find(RouteID);
	if (!Route) return false;

	// TODO: Implement political intrigue system
	// Check relationships, bribe effectiveness, risk of discovery

	UE_LOG(LogTemp, Warning, TEXT("%s attempting to usurp route %s by bribing %s with %.2fg"),
		*UsurperFamilyID, *RouteID, *FeudalLordID, BribeAmount);

	return false; // Not implemented yet
}

// ========== AI MERCHANT DYNASTIES ==========

void UGTHierarchicalEconomySubsystem::SpawnAIMerchantDynasties(int32 Count)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning %d AI Merchant Dynasties..."), Count);

	TArray<FString> MerchantNames = {
		TEXT("Medici"), TEXT("Fugger"), TEXT("Hanseatic"), TEXT("Lombard"),
		TEXT("Venetian Guild"), TEXT("Genoan Traders")
	};

	for (int32 i = 0; i < Count && i < MerchantNames.Num(); i++)
	{
		FString FamilyID = FString::Printf(TEXT("AI_Merchant_%d"), i);

		FAIMerchantDynasty AI;
		AI.FamilyID = FamilyID;
		AI.FamilyName = MerchantNames[i];
		AI.TotalGold = FMath::RandRange(5000.0f, 20000.0f);
		AI.Aggressiveness = FMath::FRandRange(0.3f, 0.9f);
		AI.RiskTolerance = FMath::FRandRange(0.2f, 0.8f);
		AI.PlayerOpinion = 0;

		// Assign random home holding (city)
		// TODO: Pick actual city from holdings

		AIMerchants.Add(FamilyID, AI);

		UE_LOG(LogTemp, Log, TEXT("  Spawned: %s (Gold: %.0f, Aggressive: %.2f, Risk: %.2f)"),
			*AI.FamilyName, AI.TotalGold, AI.Aggressiveness, AI.RiskTolerance);
	}
}

void UGTHierarchicalEconomySubsystem::ProcessAIMerchants()
{
	UE_LOG(LogTemp, Log, TEXT("Processing AI Merchants..."));

	for (auto& MerchantPair : AIMerchants)
	{
		FAIMerchantDynasty& AI = MerchantPair.Value;

		// AI evaluates opportunities
		AIEvaluateTradeOpportunities(MerchantPair.Key);

		// AI may try to acquire player routes (if aggressive)
		if (AI.Aggressiveness > 0.7f && FMath::FRand() < 0.1f)
		{
			// 10% chance to attempt acquisition each turn if aggressive
			// TODO: Pick target route
		}
	}
}

void UGTHierarchicalEconomySubsystem::AIEvaluateTradeOpportunities(const FString& AIFamilyID)
{
	FAIMerchantDynasty* AI = AIMerchants.Find(AIFamilyID);
	if (!AI) return;

	// Find profitable routes
	TArray<TPair<FString, FString>> ProfitableRoutes = FindProfitableRoutes(AI->HomeHoldingID);

	// AI creates new route if finds good opportunity
	if (ProfitableRoutes.Num() > 0 && AI->TotalGold > 1000.0f)
	{
		auto BestRoute = ProfitableRoutes[0];

		// TODO: Create AI trade route
		UE_LOG(LogTemp, Log, TEXT("  %s considering route: %s -> %s"),
			*AI->FamilyName, *BestRoute.Key, *BestRoute.Value);
	}
}

void UGTHierarchicalEconomySubsystem::AIAttemptRouteAcquisition(const FString& AIFamilyID,
	const FString& RouteID)
{
	// AI tries to buy shares or usurp player's route
	// TODO: Implement AI acquisition logic
}

// ========== TURN PROCESSING ==========

void UGTHierarchicalEconomySubsystem::ProcessHierarchicalEconomyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("=== HIERARCHICAL ECONOMY TURN START ==="));

	// 1. Holdings produce goods (handled by production subsystem)

	// 2. Process passive trade between adjacent holdings
	ProcessPassiveTrade();

	// 3. Process custom trade routes
	ProcessCustomTradeRoutes();

	// 4. Aggregate economies to regional/imperial levels
	ProcessLayeredEconomies();

	// 5. Check population needs and growth
	ProcessPopulationGrowth();

	// 6. AI merchants take actions
	ProcessAIMerchants();

	UE_LOG(LogTemp, Warning, TEXT("=== HIERARCHICAL ECONOMY TURN END ===\n"));
}

// ========== HELPER FUNCTIONS ==========

float UGTHierarchicalEconomySubsystem::CalculateDistance(const FString& HoldingA, const FString& HoldingB)
{
	// TODO: Use actual map coordinates
	// For now, return random distance
	return FMath::FRandRange(10.0f, 80.0f);
}

float UGTHierarchicalEconomySubsystem::CalculateGeographyModifier(const FString& HoldingA,
	const FString& HoldingB)
{
	FHoldingData* A = GetHolding(HoldingA);
	FHoldingData* B = GetHolding(HoldingB);

	if (!A || !B) return 0.0f;

	// Average the geography effects
	float ModifierA = 0.0f;
	float ModifierB = 0.0f;

	switch (A->Geography)
	{
		case EGeographyType::Plains: ModifierA = 0.2f; break;
		case EGeographyType::River: ModifierA = 0.3f; break;
		case EGeographyType::Hills: ModifierA = -0.1f; break;
		case EGeographyType::Mountains: ModifierA = -0.5f; break;
		case EGeographyType::Forest: ModifierA = -0.2f; break;
		case EGeographyType::Swamp: ModifierA = -0.3f; break;
		default: break;
	}

	switch (B->Geography)
	{
		case EGeographyType::Plains: ModifierB = 0.2f; break;
		case EGeographyType::River: ModifierB = 0.3f; break;
		case EGeographyType::Hills: ModifierB = -0.1f; break;
		case EGeographyType::Mountains: ModifierB = -0.5f; break;
		case EGeographyType::Forest: ModifierB = -0.2f; break;
		case EGeographyType::Swamp: ModifierB = -0.3f; break;
		default: break;
	}

	return (ModifierA + ModifierB) / 2.0f;
}

bool UGTHierarchicalEconomySubsystem::IsGeographyBlocking(EGeographyType GeographyA,
	EGeographyType GeographyB)
{
	// Mountains block passive trade
	if (GeographyA == EGeographyType::Mountains || GeographyB == EGeographyType::Mountains)
		return true;

	return false;
}

float UGTHierarchicalEconomySubsystem::CalculatePriceDifference(const FString& HoldingA,
	const FString& HoldingB, EResourceType Resource)
{
	// Get county prices
	FHoldingData* A = GetHolding(HoldingA);
	FHoldingData* B = GetHolding(HoldingB);

	if (!A || !B) return 0.0f;

	FCountyData* CountyA = GetHoldingCounty(HoldingA);
	FCountyData* CountyB = GetHoldingCounty(HoldingB);

	if (!CountyA || !CountyB) return 0.0f;

	float PriceA = CountyA->RegionalPrices.FindRef(Resource);
	float PriceB = CountyB->RegionalPrices.FindRef(Resource);

	return PriceB - PriceA; // Profit if positive
}

TArray<TPair<FString, FString>> UGTHierarchicalEconomySubsystem::FindProfitableRoutes(
	const FString& HomeHoldingID)
{
	TArray<TPair<FString, FString>> Routes;

	// TODO: Implement route finding algorithm
	// Find holdings with high price differences

	return Routes;
}

float UGTHierarchicalEconomySubsystem::CalculateGrowthRate(float NeedsSatisfaction)
{
	// Growth formula based on needs satisfaction
	if (NeedsSatisfaction < 0.5f)
	{
		// Starvation: negative growth
		return -0.01f * (1.0f - NeedsSatisfaction);
	}
	else if (NeedsSatisfaction < 0.7f)
	{
		// Barely surviving: no growth
		return 0.0f;
	}
	else if (NeedsSatisfaction < 0.9f)
	{
		// Comfortable: slow growth
		return 0.001f * (NeedsSatisfaction - 0.7f);
	}
	else
	{
		// Thriving: fast growth
		return 0.002f * (NeedsSatisfaction - 0.9f);
	}
}

void UGTHierarchicalEconomySubsystem::DistributeRouteProfit(const FString& RouteID, float Profit)
{
	FCustomTradeRoute* Route = CustomTradeRoutes.Find(RouteID);
	if (!Route || Profit <= 0.0f) return;

	// Distribute profit to all owners based on their share percentage
	for (auto& OwnerPair : Route->OwnershipShares)
	{
		FString OwnerID = OwnerPair.Key;
		float Share = OwnerPair.Value;
		float OwnerProfit = Profit * Share;

		// TODO: Add gold to owner (player or AI)
		UE_LOG(LogTemp, Log, TEXT("  Route %s: %s earned %.2fg (%.1f%% share)"),
			*RouteID, *OwnerID, OwnerProfit, Share * 100.0f);
	}
}

void UGTHierarchicalEconomySubsystem::InitializePopulationNeeds()
{
	// Basic needs per 100 people per week
	PopulationNeeds.BasicNeeds.Add(EResourceType::Bread, 20);    // 2kg/person/week
	PopulationNeeds.BasicNeeds.Add(EResourceType::Fish, 10);
	PopulationNeeds.BasicNeeds.Add(EResourceType::Grain, 15);

	// Comfort needs
	PopulationNeeds.ComfortNeeds.Add(EResourceType::Beer, 8);
	PopulationNeeds.ComfortNeeds.Add(EResourceType::Garments, 2);
	PopulationNeeds.ComfortNeeds.Add(EResourceType::Tools, 1);

	// Luxury needs
	PopulationNeeds.LuxuryNeeds.Add(EResourceType::Wine, 3);
	PopulationNeeds.LuxuryNeeds.Add(EResourceType::Spices, 1);
	PopulationNeeds.LuxuryNeeds.Add(EResourceType::LuxuryCloth, 1);

	UE_LOG(LogTemp, Log, TEXT("Population needs template initialized"));
}

FCountyData* UGTHierarchicalEconomySubsystem::GetHoldingCounty(const FString& HoldingID)
{
	FHoldingData* Holding = GetHolding(HoldingID);
	if (!Holding) return nullptr;

	return AllCounties.Find(Holding->ParentCountyID);
}

FDuchyData* UGTHierarchicalEconomySubsystem::GetCountyDuchy(const FString& CountyID)
{
	FCountyData* County = AllCounties.Find(CountyID);
	if (!County) return nullptr;

	return AllDuchies.Find(County->ParentDuchyID);
}

FKingdomData* UGTHierarchicalEconomySubsystem::GetDuchyKingdom(const FString& DuchyID)
{
	FDuchyData* Duchy = AllDuchies.Find(DuchyID);
	if (!Duchy) return nullptr;

	return AllKingdoms.Find(Duchy->ParentKingdomID);
}

float UGTHierarchicalEconomySubsystem::CalculateTariffs(const FString& HoldingA,
	const FString& HoldingB)
{
	FCountyData* CountyA = GetHoldingCounty(HoldingA);
	FCountyData* CountyB = GetHoldingCounty(HoldingB);

	if (!CountyA || !CountyB) return 0.0f;

	// Same county = no tariff
	if (CountyA == CountyB) return 0.0f;

	FDuchyData* DuchyA = GetCountyDuchy(CountyA->CountyName);
	FDuchyData* DuchyB = GetCountyDuchy(CountyB->CountyName);

	// Same duchy = low tariff
	if (DuchyA && DuchyB && DuchyA == DuchyB) return 0.02f;

	FKingdomData* KingdomA = DuchyA ? GetDuchyKingdom(DuchyA->DuchyName) : nullptr;
	FKingdomData* KingdomB = DuchyB ? GetDuchyKingdom(DuchyB->DuchyName) : nullptr;

	// Same kingdom = kingdom tariff
	if (KingdomA && KingdomB && KingdomA == KingdomB)
	{
		return KingdomA->BaseTariff;
	}

	// Different kingdoms = kingdom + imperial tariffs
	float TotalTariff = 0.0f;
	if (KingdomA) TotalTariff += KingdomA->BaseTariff;
	if (KingdomB) TotalTariff += KingdomB->BaseTariff;

	// Add imperial tariff if in same empire
	// TODO: Check empire membership

	return FMath::Min(TotalTariff, 0.5f); // Cap at 50%
}
