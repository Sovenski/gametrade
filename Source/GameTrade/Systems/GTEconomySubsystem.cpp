// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTEconomySubsystem.h"
#include "Systems/GTTimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGTEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Economy Subsystem Initialized"));

	// Initialize base prices for all resources
	BasePrices.Add(EResourceType::Grain, 3.0f);
	BasePrices.Add(EResourceType::Wool, 8.0f);
	BasePrices.Add(EResourceType::Timber, 2.0f);
	BasePrices.Add(EResourceType::IronOre, 10.0f);
	BasePrices.Add(EResourceType::Salt, 12.0f);
	BasePrices.Add(EResourceType::Wine, 15.0f);
	BasePrices.Add(EResourceType::Beer, 5.0f);
	BasePrices.Add(EResourceType::Leather, 10.0f);
	BasePrices.Add(EResourceType::WoolenCloth, 25.0f);
	BasePrices.Add(EResourceType::IronBars, 30.0f);
	BasePrices.Add(EResourceType::Tools, 50.0f);
	BasePrices.Add(EResourceType::Weapons, 80.0f);
	BasePrices.Add(EResourceType::Armor, 120.0f);
	BasePrices.Add(EResourceType::Spices, 200.0f);
	BasePrices.Add(EResourceType::Silk, 150.0f);
	// TODO: Add all other resources
}

void UGTEconomySubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Economy Subsystem Deinitialized"));
}

void UGTEconomySubsystem::LoadSettlementsFromDataTable(UDataTable* SettlementTable)
{
	if (!SettlementTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Settlement DataTable is null!"));
		return;
	}

	TArray<FSettlementData*> Rows;
	SettlementTable->GetAllRows<FSettlementData>(TEXT("LoadSettlements"), Rows);

	for (FSettlementData* Row : Rows)
	{
		if (Row)
		{
			Settlements.Add(Row->SettlementName, *Row);
			UE_LOG(LogTemp, Log, TEXT("Loaded settlement: %s"), *Row->SettlementName);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d settlements"), Settlements.Num());
}

void UGTEconomySubsystem::InitializeEconomy()
{
	// Initialize prices for all settlements
	for (auto& SettlementPair : Settlements)
	{
		FSettlementData& Settlement = SettlementPair.Value;

		// Set base prices for all resources
		for (const auto& BasePricePair : BasePrices)
		{
			FPriceInfo PriceInfo;
			PriceInfo.BasePrice = BasePricePair.Value;
			PriceInfo.CurrentPrice = BasePricePair.Value;

			// If settlement produces this, set initial supply
			if (Settlement.LocalProduction.Contains(BasePricePair.Key))
			{
				PriceInfo.Supply = BASE_PRODUCTION_PER_WEEK;
			}

			// If settlement needs this, set initial demand
			if (Settlement.LocalNeeds.Contains(BasePricePair.Key))
			{
				PriceInfo.Demand = (Settlement.Population / 1000) * BASE_CONSUMPTION_PER_1000_POP;
			}

			Settlement.Prices.Add(BasePricePair.Key, PriceInfo);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Economy initialized with prices"));
}

void UGTEconomySubsystem::ProcessEconomyTurn()
{
	UpdateProduction();
	UpdateConsumption();
	UpdatePrices();
	ApplySeasonalModifiers();

	UE_LOG(LogTemp, Log, TEXT("Economy turn processed"));
}

void UGTEconomySubsystem::UpdateProduction()
{
	for (auto& SettlementPair : Settlements)
	{
		FSettlementData& Settlement = SettlementPair.Value;

		for (EResourceType ResourceType : Settlement.LocalProduction)
		{
			if (FPriceInfo* PriceInfo = Settlement.Prices.Find(ResourceType))
			{
				// Add weekly production
				PriceInfo->Supply += BASE_PRODUCTION_PER_WEEK;
			}
		}
	}
}

void UGTEconomySubsystem::UpdateConsumption()
{
	for (auto& SettlementPair : Settlements)
	{
		FSettlementData& Settlement = SettlementPair.Value;

		for (EResourceType ResourceType : Settlement.LocalNeeds)
		{
			if (FPriceInfo* PriceInfo = Settlement.Prices.Find(ResourceType))
			{
				// Calculate consumption based on population
				int32 Consumption = (Settlement.Population / 1000) * BASE_CONSUMPTION_PER_1000_POP;
				PriceInfo->Demand = Consumption;

				// Consume from supply if available
				if (PriceInfo->Supply >= Consumption)
				{
					PriceInfo->Supply -= Consumption;
				}
				else
				{
					// Shortage! Prices will rise
					PriceInfo->Supply = 0;
				}
			}
		}
	}
}

void UGTEconomySubsystem::UpdatePrices()
{
	for (auto& SettlementPair : Settlements)
	{
		FSettlementData& Settlement = SettlementPair.Value;

		for (auto& PricePair : Settlement.Prices)
		{
			FPriceInfo& PriceInfo = PricePair.Value;
			float OldPrice = PriceInfo.CurrentPrice;

			// Supply/Demand price adjustment
			if (PriceInfo.Demand > 0)
			{
				float SupplyDemandRatio = (float)PriceInfo.Supply / (float)PriceInfo.Demand;

				if (SupplyDemandRatio < 0.5f)
				{
					// Severe shortage - price spike!
					PriceInfo.CurrentPrice = PriceInfo.BasePrice * (1.0f + PRICE_ELASTICITY * 2.0f);
				}
				else if (SupplyDemandRatio < 1.0f)
				{
					// Shortage - price increase
					float Shortage = 1.0f - SupplyDemandRatio;
					PriceInfo.CurrentPrice = PriceInfo.BasePrice * (1.0f + PRICE_ELASTICITY * Shortage);
				}
				else if (SupplyDemandRatio > 2.0f)
				{
					// Oversupply - price crash!
					PriceInfo.CurrentPrice = PriceInfo.BasePrice * (1.0f - PRICE_ELASTICITY * 0.5f);
				}
				else
				{
					// Normal market
					PriceInfo.CurrentPrice = PriceInfo.BasePrice;
				}
			}

			// Calculate price change percentage
			if (OldPrice > 0)
			{
				PriceInfo.PriceChangePercent = ((PriceInfo.CurrentPrice - OldPrice) / OldPrice) * 100.0f;
			}

			// Broadcast if significant change
			if (FMath::Abs(PriceInfo.PriceChangePercent) > 5.0f)
			{
				OnPriceChanged.Broadcast(Settlement.SettlementName, PricePair.Key, PriceInfo.CurrentPrice);
			}
		}
	}
}

void UGTEconomySubsystem::ApplySeasonalModifiers()
{
	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (!TimeSubsystem) return;

	for (auto& SettlementPair : Settlements)
	{
		FSettlementData& Settlement = SettlementPair.Value;

		for (auto& PricePair : Settlement.Prices)
		{
			FPriceInfo& PriceInfo = PricePair.Value;
			float SeasonalModifier = TimeSubsystem->GetSeasonalPriceModifier(PricePair.Key);
			PriceInfo.CurrentPrice *= SeasonalModifier;
		}
	}
}

float UGTEconomySubsystem::CalculateBasePrice(EResourceType ResourceType) const
{
	const float* Price = BasePrices.Find(ResourceType);
	return Price ? *Price : 1.0f;
}

float UGTEconomySubsystem::GetPrice(const FString& SettlementName, EResourceType ResourceType) const
{
	const FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return 0.0f;

	const FPriceInfo* PriceInfo = Settlement->Prices.Find(ResourceType);
	return PriceInfo ? PriceInfo->CurrentPrice : 0.0f;
}

FPriceInfo UGTEconomySubsystem::GetPriceInfo(const FString& SettlementName, EResourceType ResourceType) const
{
	const FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return FPriceInfo();

	const FPriceInfo* PriceInfo = Settlement->Prices.Find(ResourceType);
	return PriceInfo ? *PriceInfo : FPriceInfo();
}

bool UGTEconomySubsystem::BuyGoods(const FString& SettlementName, EResourceType ResourceType, int32 Amount, float& OutTotalCost)
{
	FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return false;

	FPriceInfo* PriceInfo = Settlement->Prices.Find(ResourceType);
	if (!PriceInfo) return false;

	// Check if enough supply
	if (PriceInfo->Supply < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough supply in %s for %s"), *SettlementName, *UEnum::GetValueAsString(ResourceType));
		return false;
	}

	// Calculate cost
	OutTotalCost = PriceInfo->CurrentPrice * Amount;

	// Remove from supply
	PriceInfo->Supply -= Amount;

	UE_LOG(LogTemp, Log, TEXT("Bought %d %s in %s for %.2f gold"), Amount, *UEnum::GetValueAsString(ResourceType), *SettlementName, OutTotalCost);

	return true;
}

bool UGTEconomySubsystem::SellGoods(const FString& SettlementName, EResourceType ResourceType, int32 Amount, float& OutTotalRevenue)
{
	FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return false;

	FPriceInfo* PriceInfo = Settlement->Prices.Find(ResourceType);
	if (!PriceInfo) return false;

	// Calculate revenue
	OutTotalRevenue = PriceInfo->CurrentPrice * Amount;

	// Add to supply (you're selling to the market)
	PriceInfo->Supply += Amount;

	UE_LOG(LogTemp, Log, TEXT("Sold %d %s in %s for %.2f gold"), Amount, *UEnum::GetValueAsString(ResourceType), *SettlementName, OutTotalRevenue);

	return true;
}

TArray<FString> UGTEconomySubsystem::GetAllSettlementNames() const
{
	TArray<FString> Names;
	Settlements.GetKeys(Names);
	return Names;
}

FSettlementData UGTEconomySubsystem::GetSettlementData(const FString& SettlementName) const
{
	const FSettlementData* Settlement = Settlements.Find(SettlementName);
	return Settlement ? *Settlement : FSettlementData();
}

bool UGTEconomySubsystem::DoesSettlementProduce(const FString& SettlementName, EResourceType ResourceType) const
{
	const FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return false;
	return Settlement->LocalProduction.Contains(ResourceType);
}

bool UGTEconomySubsystem::DoesSettlementNeed(const FString& SettlementName, EResourceType ResourceType) const
{
	const FSettlementData* Settlement = Settlements.Find(SettlementName);
	if (!Settlement) return false;
	return Settlement->LocalNeeds.Contains(ResourceType);
}

int32 UGTEconomySubsystem::CalculateTravelTime(const FString& FromSettlement, const FString& ToSettlement) const
{
	const FSettlementData* From = Settlements.Find(FromSettlement);
	const FSettlementData* To = Settlements.Find(ToSettlement);

	if (!From || !To) return 0;

	// Calculate distance (simplified - using 2D map coordinates)
	FVector2D Distance = From->MapLocation - To->MapLocation;
	float DistanceKm = Distance.Size(); // Assume map units = km

	// Medieval caravan speed: ~30-40 km/week
	const float BASE_SPEED_KM_PER_WEEK = 35.0f;
	int32 BaseWeeks = FMath::CeilToInt(DistanceKm / BASE_SPEED_KM_PER_WEEK);

	// Apply seasonal modifier
	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (TimeSubsystem)
	{
		float SeasonalModifier = TimeSubsystem->GetSeasonalTravelTimeModifier();
		BaseWeeks = FMath::CeilToInt(BaseWeeks * SeasonalModifier);
	}

	return FMath::Max(1, BaseWeeks); // Minimum 1 week
}
