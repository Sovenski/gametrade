// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTTimeSubsystem.h"

void UGTTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize starting date
	CurrentDate.Year = 1347;
	CurrentDate.Week = 1;
	CurrentDate.Season = ESeason::Spring;
	LastSeason = ESeason::Spring;

	UE_LOG(LogTemp, Log, TEXT("Time Subsystem Initialized: %s"), *CurrentDate.ToString());
}

void UGTTimeSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Time Subsystem Deinitialized"));
}

void UGTTimeSubsystem::AdvanceTurn()
{
	int32 OldYear = CurrentDate.Year;
	ESeason OldSeason = CurrentDate.Season;

	CurrentDate.AdvanceWeek();

	// Broadcast delegates
	OnTurnAdvanced.Broadcast(CurrentDate);

	if (CurrentDate.Season != OldSeason)
	{
		OnSeasonChanged.Broadcast(CurrentDate.Season);
		UE_LOG(LogTemp, Log, TEXT("Season changed to %s"), *UEnum::GetValueAsString(CurrentDate.Season));
	}

	if (CurrentDate.Year != OldYear)
	{
		OnYearChanged.Broadcast(CurrentDate.Year);
		UE_LOG(LogTemp, Log, TEXT("New year: %d"), CurrentDate.Year);
	}

	UE_LOG(LogTemp, Log, TEXT("Turn advanced: %s"), *CurrentDate.ToString());
}

void UGTTimeSubsystem::AdvanceMultipleTurns(int32 NumTurns)
{
	for (int32 i = 0; i < NumTurns; i++)
	{
		AdvanceTurn();
	}
}

float UGTTimeSubsystem::GetSeasonalPriceModifier(EResourceType ResourceType) const
{
	// Seasonal price modifiers for different goods
	switch (CurrentDate.Season)
	{
	case ESeason::Spring:
		if (ResourceType == EResourceType::Grain) return 1.3f; // Expensive before harvest
		if (ResourceType == EResourceType::Game) return 1.1f;
		break;

	case ESeason::Summer:
		if (ResourceType == EResourceType::Grain) return 0.9f; // Harvest starting
		if (ResourceType == EResourceType::Wine) return 1.2f; // Before grape harvest
		break;

	case ESeason::Autumn:
		if (ResourceType == EResourceType::Grain) return 0.7f; // Harvest complete!
		if (ResourceType == EResourceType::Wine) return 0.8f; // New wine available
		if (ResourceType == EResourceType::Game) return 0.9f; // Hunting season
		break;

	case ESeason::Winter:
		if (ResourceType == EResourceType::Grain) return 1.1f; // Stocks depleting
		if (ResourceType == EResourceType::SmokedMeat) return 0.8f; // Preserved meat needed
		if (ResourceType == EResourceType::Furs) return 0.7f; // High demand for warmth
		if (ResourceType == EResourceType::Firewood) return 0.6f; // Essential
		break;
	}

	return 1.0f; // No modifier
}

float UGTTimeSubsystem::GetSeasonalTravelTimeModifier() const
{
	switch (CurrentDate.Season)
	{
	case ESeason::Spring:
		return 1.1f; // Mud season, slightly slower

	case ESeason::Summer:
		return 1.0f; // Best travel weather

	case ESeason::Autumn:
		return 1.0f; // Still good

	case ESeason::Winter:
		return 1.5f; // Slow due to snow, ice, storms

	default:
		return 1.0f;
	}
}
