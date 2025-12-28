// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GTDataTypes.h"
#include "GTEventTypes.generated.h"

/**
 * Event types and data structures
 */

UENUM(BlueprintType)
enum class EEventType : uint8
{
	// Economic Events
	GoodHarvest UMETA(DisplayName = "Good Harvest"),
	BadHarvest UMETA(DisplayName = "Bad Harvest"),
	PriceSpike UMETA(DisplayName = "Price Spike"),
	MarketCrash UMETA(DisplayName = "Market Crash"),
	NewMineDiscovered UMETA(DisplayName = "New Mine Discovered"),
	TradeDealOffered UMETA(DisplayName = "Trade Deal Offered"),

	// Natural Disasters
	Fire UMETA(DisplayName = "Fire"),
	Flood UMETA(DisplayName = "Flood"),
	HarshWinter UMETA(DisplayName = "Harsh Winter"),
	Drought UMETA(DisplayName = "Drought"),
	Earthquake UMETA(DisplayName = "Earthquake"),

	// Political Events
	WarDeclared UMETA(DisplayName = "War Declared"),
	PeaceTreaty UMETA(DisplayName = "Peace Treaty"),
	NewTariffImposed UMETA(DisplayName = "New Tariff Imposed"),
	TradeEmbargoLifted UMETA(DisplayName = "Trade Embargo Lifted"),
	NoblePatronage UMETA(DisplayName = "Noble Patronage"),

	// Dynasty Events
	MarriageProposal UMETA(DisplayName = "Marriage Proposal"),
	RivalChallenge UMETA(DisplayName = "Rival Challenge"),
	SuccessionCrisis UMETA(DisplayName = "Succession Crisis"),
	FamilyFeud UMETA(DisplayName = "Family Feud"),

	// Criminal/Intrigue
	SabotageAttempt UMETA(DisplayName = "Sabotage Attempt"),
	SpyDiscovered UMETA(DisplayName = "Spy Discovered"),
	AssassinationAttempt UMETA(DisplayName = "Assassination Attempt"),
	TheftReported UMETA(DisplayName = "Theft Reported"),

	// Historical Events
	BlackDeath UMETA(DisplayName = "Black Death"),
	ChampagneFair UMETA(DisplayName = "Champagne Fair"),
	CrusadeAnnounced UMETA(DisplayName = "Crusade Announced"),
	PrintingPressInvented UMETA(DisplayName = "Printing Press Invented"),

	// Opportunities
	BuildingForSale UMETA(DisplayName = "Building For Sale"),
	RivalBankruptcy UMETA(DisplayName = "Rival Bankruptcy"),
	LuckyFind UMETA(DisplayName = "Lucky Find"),
	ExclusiveContract UMETA(DisplayName = "Exclusive Contract")
};

UENUM(BlueprintType)
enum class EEventSeverity : uint8
{
	Minor UMETA(DisplayName = "Minor"),
	Moderate UMETA(DisplayName = "Moderate"),
	Major UMETA(DisplayName = "Major"),
	Critical UMETA(DisplayName = "Critical"),
	Historical UMETA(DisplayName = "Historical")
};

USTRUCT(BlueprintType)
struct FGameEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EventID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	EEventType EventType = EEventType::GoodHarvest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	EEventSeverity Severity = EEventSeverity::Minor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EventTitle = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EventDescription = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FGameDate EventDate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString AffectedSettlement = TEXT(""); // Empty if global

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString AffectedFamily = TEXT(""); // Empty if affects all

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TArray<FEventChoice> Choices; // Player choices

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool bRequiresPlayerResponse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool bHasBeenResolved = false;
};

USTRUCT(BlueprintType)
struct FEventChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString ChoiceID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString ChoiceText = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString ChoiceDescription = TEXT(""); // What will happen if chosen

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float GoldCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TArray<FEventEffect> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float SuccessChance = 1.0f; // 0.0 to 1.0
};

USTRUCT(BlueprintType)
struct FEventEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EffectDescription = TEXT("");

	// Economic effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float GoldChange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TMap<EResourceType, int32> ResourceChanges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float PriceMultiplier = 1.0f; // Affects specific resource prices

	// Character effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString AffectedCharacterID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 SkillChange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString SkillName = TEXT("");

	// Relationship effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString AffectedFamilyID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 ReputationChange = 0;
};

USTRUCT(BlueprintType)
struct FEventTemplate : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	EEventType EventType = EEventType::GoodHarvest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	EEventSeverity Severity = EEventSeverity::Minor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString TitleTemplate = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString DescriptionTemplate = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float BaseChance = 0.05f; // 5% per turn

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 MinimumYear = 1347;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 MaximumYear = 1450;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool bCanRepeat = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 CooldownTurns = 0; // Minimum turns before can occur again
};
