// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTTradeSubsystem.h"
#include "Systems/GTEconomySubsystem.h"
#include "Systems/GTTimeSubsystem.h"
#include "Core/GTGameInstance.h"

void UGTTradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Trade Subsystem Initialized"));
}

void UGTTradeSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Trade Subsystem Deinitialized"));
}

FString UGTTradeSubsystem::SendCaravan(const FString& OwnerFamilyID, const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& Cargo)
{
	UGTEconomySubsystem* EconomySubsystem = GetGameInstance()->GetSubsystem<UGTEconomySubsystem>();
	if (!EconomySubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Economy subsystem not found"));
		return TEXT("");
	}

	// Calculate travel time
	int32 TravelTime = EconomySubsystem->CalculateTravelTime(FromSettlement, ToSettlement);
	if (TravelTime <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid travel time between %s and %s"), *FromSettlement, *ToSettlement);
		return TEXT("");
	}

	// Create caravan
	FCaravanData NewCaravan;
	NewCaravan.CaravanID = GenerateCaravanID();
	NewCaravan.OwnerFamilyID = OwnerFamilyID;
	NewCaravan.CurrentLocation = FromSettlement;
	NewCaravan.Destination = ToSettlement;
	NewCaravan.WeeksToDestination = TravelTime;
	NewCaravan.Cargo = Cargo;
	NewCaravan.Capacity = 20; // TODO: Make this configurable based on cart type

	// Calculate total cargo
	int32 TotalCargo = 0;
	for (const FResourceAmount& Item : Cargo)
	{
		TotalCargo += Item.Amount;
	}

	if (TotalCargo > NewCaravan.Capacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo (%d) exceeds caravan capacity (%d)"), TotalCargo, NewCaravan.Capacity);
		return TEXT("");
	}

	Caravans.Add(NewCaravan.CaravanID, NewCaravan);

	OnCaravanDeparted.Broadcast(NewCaravan.CaravanID, ToSettlement);

	UE_LOG(LogTemp, Log, TEXT("Caravan %s departed from %s to %s (ETA: %d weeks)"),
		*NewCaravan.CaravanID, *FromSettlement, *ToSettlement, TravelTime);

	return NewCaravan.CaravanID;
}

bool UGTTradeSubsystem::RecallCaravan(const FString& CaravanID)
{
	FCaravanData* Caravan = Caravans.Find(CaravanID);
	if (!Caravan)
	{
		return false;
	}

	// Swap destination and current location (turn around)
	FString Temp = Caravan->Destination;
	Caravan->Destination = Caravan->CurrentLocation;
	Caravan->CurrentLocation = Temp;

	// Recalculate travel time
	UGTEconomySubsystem* EconomySubsystem = GetGameInstance()->GetSubsystem<UGTEconomySubsystem>();
	if (EconomySubsystem)
	{
		Caravan->WeeksToDestination = EconomySubsystem->CalculateTravelTime(Caravan->CurrentLocation, Caravan->Destination);
	}

	UE_LOG(LogTemp, Log, TEXT("Caravan %s recalled, returning to %s"), *CaravanID, *Caravan->Destination);
	return true;
}

bool UGTTradeSubsystem::SetCaravanAutomatic(const FString& CaravanID, bool bAutomatic)
{
	FCaravanData* Caravan = Caravans.Find(CaravanID);
	if (!Caravan)
	{
		return false;
	}

	Caravan->bIsAutomatic = bAutomatic;
	UE_LOG(LogTemp, Log, TEXT("Caravan %s automatic mode: %s"), *CaravanID, bAutomatic ? TEXT("ON") : TEXT("OFF"));
	return true;
}

void UGTTradeSubsystem::ProcessCaravansTurn()
{
	TArray<FString> ArrivedCaravans;

	for (auto& CaravanPair : Caravans)
	{
		FCaravanData& Caravan = CaravanPair.Value;

		// Check for random encounters
		if (RollRandomEncounter(Caravan))
		{
			// Encounter handled in function
			continue;
		}

		// Advance travel
		AdvanceCaravanTravel(Caravan);

		// Check if arrived
		if (Caravan.WeeksToDestination <= 0)
		{
			ArrivedCaravans.Add(Caravan.CaravanID);
		}
	}

	// Handle arrivals
	for (const FString& CaravanID : ArrivedCaravans)
	{
		FCaravanData* Caravan = Caravans.Find(CaravanID);
		if (Caravan)
		{
			HandleCaravanArrival(*Caravan);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Processed %d caravans, %d arrived"), Caravans.Num(), ArrivedCaravans.Num());
}

void UGTTradeSubsystem::AdvanceCaravanTravel(FCaravanData& Caravan)
{
	Caravan.WeeksToDestination--;
	UE_LOG(LogTemp, Verbose, TEXT("Caravan %s: %d weeks to %s"),
		*Caravan.CaravanID, Caravan.WeeksToDestination, *Caravan.Destination);
}

void UGTTradeSubsystem::HandleCaravanArrival(FCaravanData& Caravan)
{
	UE_LOG(LogTemp, Log, TEXT("Caravan %s arrived at %s"), *Caravan.CaravanID, *Caravan.Destination);

	// Update location
	Caravan.CurrentLocation = Caravan.Destination;
	Caravan.WeeksToDestination = 0;

	// Broadcast arrival
	OnCaravanArrived.Broadcast(Caravan.CaravanID, Caravan.Destination, true);

	// TODO: Implement automatic trading if bIsAutomatic
	// - Sell cargo
	// - Buy new cargo
	// - Return to origin

	// If automatic, set up return journey
	if (Caravan.bIsAutomatic)
	{
		UE_LOG(LogTemp, Log, TEXT("Caravan %s is automatic, setting up return journey"), *Caravan.CaravanID);
		// TODO: Implement auto-trading logic
	}
	else
	{
		// Remove caravan (it has arrived)
		Caravans.Remove(Caravan.CaravanID);
	}
}

bool UGTTradeSubsystem::RollRandomEncounter(FCaravanData& Caravan)
{
	// Calculate risk level based on route
	float RiskLevel = CalculateRiskLevel(Caravan.CurrentLocation, Caravan.Destination);

	// Roll for encounter
	float Roll = FMath::FRand();

	if (Roll < RiskLevel * 0.1f) // Base 10% chance modified by risk
	{
		// Bandit attack!
		HandleBanditAttack(Caravan);
		return true;
	}
	else if (Roll < RiskLevel * 0.15f) // 15% chance for weather
	{
		// Weather delay
		HandleWeatherDelay(Caravan);
		return true;
	}

	return false; // No encounter
}

void UGTTradeSubsystem::HandleBanditAttack(FCaravanData& Caravan)
{
	// Random loss: 10-50% of cargo
	float LossPercentage = FMath::FRandRange(0.1f, 0.5f);

	UE_LOG(LogTemp, Warning, TEXT("Caravan %s attacked by bandits! %.1f%% cargo lost"),
		*Caravan.CaravanID, LossPercentage * 100.0f);

	// Reduce cargo
	for (FResourceAmount& Item : Caravan.Cargo)
	{
		int32 Loss = FMath::RoundToInt(Item.Amount * LossPercentage);
		Item.Amount = FMath::Max(0, Item.Amount - Loss);
	}

	OnCaravanAttacked.Broadcast(Caravan.CaravanID, LossPercentage);
}

void UGTTradeSubsystem::HandleWeatherDelay(FCaravanData& Caravan)
{
	// Delay: 1-3 weeks
	int32 Delay = FMath::RandRange(1, 3);
	Caravan.WeeksToDestination += Delay;

	UE_LOG(LogTemp, Warning, TEXT("Caravan %s delayed by weather (+%d weeks)"),
		*Caravan.CaravanID, Delay);
}

float UGTTradeSubsystem::CalculateRiskLevel(const FString& FromSettlement, const FString& ToSettlement) const
{
	// Base risk
	float Risk = 1.0f;

	// TODO: Increase risk based on:
	// - War zones (2x risk)
	// - Mountain passes (1.5x risk)
	// - Winter season (1.3x risk)
	// - Distance (longer = more risk)

	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (TimeSubsystem && TimeSubsystem->GetCurrentSeason() == ESeason::Winter)
	{
		Risk *= 1.3f;
	}

	return FMath::Clamp(Risk, 0.5f, 3.0f);
}

TArray<FCaravanData> UGTTradeSubsystem::GetActiveCaravans(const FString& OwnerFamilyID) const
{
	TArray<FCaravanData> Result;

	for (const auto& CaravanPair : Caravans)
	{
		if (CaravanPair.Value.OwnerFamilyID == OwnerFamilyID)
		{
			Result.Add(CaravanPair.Value);
		}
	}

	return Result;
}

FCaravanData UGTTradeSubsystem::GetCaravanData(const FString& CaravanID) const
{
	const FCaravanData* Caravan = Caravans.Find(CaravanID);
	return Caravan ? *Caravan : FCaravanData();
}

int32 UGTTradeSubsystem::GetCaravanCount(const FString& OwnerFamilyID) const
{
	int32 Count = 0;

	for (const auto& CaravanPair : Caravans)
	{
		if (CaravanPair.Value.OwnerFamilyID == OwnerFamilyID)
		{
			Count++;
		}
	}

	return Count;
}

bool UGTTradeSubsystem::IsCaravanTraveling(const FString& CaravanID) const
{
	const FCaravanData* Caravan = Caravans.Find(CaravanID);
	return Caravan && Caravan->WeeksToDestination > 0;
}

FString UGTTradeSubsystem::CreateTradeRoute(const FString& OwnerFamilyID, const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& GoodsToBuy, const TArray<FResourceAmount>& GoodsToSell)
{
	FTradeRoute NewRoute;
	NewRoute.RouteID = GenerateRouteID();
	NewRoute.OwnerFamilyID = OwnerFamilyID;
	NewRoute.FromSettlement = FromSettlement;
	NewRoute.ToSettlement = ToSettlement;
	NewRoute.GoodsToBuy = GoodsToBuy;
	NewRoute.GoodsToSell = GoodsToSell;
	NewRoute.bActive = true;

	TradeRoutes.Add(NewRoute.RouteID, NewRoute);

	UE_LOG(LogTemp, Log, TEXT("Created trade route %s: %s → %s"),
		*NewRoute.RouteID, *FromSettlement, *ToSettlement);

	return NewRoute.RouteID;
}

bool UGTTradeSubsystem::DeleteTradeRoute(const FString& RouteID)
{
	if (TradeRoutes.Remove(RouteID) > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Deleted trade route: %s"), *RouteID);
		return true;
	}

	return false;
}

FString UGTTradeSubsystem::GenerateCaravanID() const
{
	return FGuid::NewGuid().ToString();
}

FString UGTTradeSubsystem::GenerateRouteID() const
{
	return FGuid::NewGuid().ToString();
}
