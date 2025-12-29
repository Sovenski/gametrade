// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GTDataTypes.h"
#include "GTHierarchyTypes.generated.h"

/**
 * CK3-Style Hierarchical Settlement System
 * Empire → Kingdom → Duchy → County → Barony/Holdings
 */

// ========== HOLDING TYPES (like CK3) ==========

UENUM(BlueprintType)
enum class EHoldingType : uint8
{
	City UMETA(DisplayName = "City"),              // Merchants can own, generates trade income
	Castle UMETA(DisplayName = "Castle"),          // Feudal lords own, military power
	Village UMETA(DisplayName = "Village"),        // Produces raw resources
	Temple UMETA(DisplayName = "Temple"),          // Religious, provides bonuses
	Port UMETA(DisplayName = "Port"),              // Coastal, enables sea trade
	Mine UMETA(DisplayName = "Mine"),              // Specialized mining
	Monastery UMETA(DisplayName = "Monastery"),    // Specialized religious, produces books/medicine
	TradePost UMETA(DisplayName = "Trade Post"),   // On trade routes, warehouses

	MAX UMETA(Hidden)
};

// ========== FEUDAL RANKS ==========

UENUM(BlueprintType)
enum class EFedalRank : uint8
{
	Barony UMETA(DisplayName = "Barony"),      // Single holding
	County UMETA(DisplayName = "County"),      // 2-6 baronies
	Duchy UMETA(DisplayName = "Duchy"),        // 2-4 counties
	Kingdom UMETA(DisplayName = "Kingdom"),    // 2-6 duchies
	Empire UMETA(DisplayName = "Empire"),      // 2+ kingdoms

	MAX UMETA(Hidden)
};

// ========== GEOGRAPHY TYPES (affects trade) ==========

UENUM(BlueprintType)
enum class EGeographyType : uint8
{
	Plains UMETA(DisplayName = "Plains"),          // +20% trade speed
	Hills UMETA(DisplayName = "Hills"),            // -10% trade speed
	Mountains UMETA(DisplayName = "Mountains"),    // -50% trade speed, blocks passive trade
	Forest UMETA(DisplayName = "Forest"),          // -20% trade speed
	Swamp UMETA(DisplayName = "Swamp"),            // -30% trade speed
	Desert UMETA(DisplayName = "Desert"),          // -40% trade speed
	Coast UMETA(DisplayName = "Coast"),            // Enables sea trade
	River UMETA(DisplayName = "River"),            // +30% trade speed

	MAX UMETA(Hidden)
};

// ========== TRADE ROUTE TYPES ==========

UENUM(BlueprintType)
enum class ETradeRouteType : uint8
{
	Passive UMETA(DisplayName = "Passive"),        // Auto-generated between adjacent
	PlayerCustom UMETA(DisplayName = "Player Custom"), // Player-defined
	AICustom UMETA(DisplayName = "AI Custom"),     // AI merchant route
	Historic UMETA(DisplayName = "Historic"),      // Pre-existing major routes (Silk Road, Hanseatic)

	MAX UMETA(Hidden)
};

// ========== ECONOMY LAYER ==========

UENUM(BlueprintType)
enum class EEconomyLayer : uint8
{
	Local UMETA(DisplayName = "Local"),        // Village/barony - raw resources
	Regional UMETA(DisplayName = "Regional"),  // County/duchy - processing
	Imperial UMETA(DisplayName = "Imperial"),  // Kingdom/empire - luxury trade

	MAX UMETA(Hidden)
};

// ========== STRUCTURES ==========

/**
 * Holding (individual settlement - like CK3 barony)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FHoldingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HoldingName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHoldingType HoldingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Population;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGeographyType Geography;

	// What this holding produces (e.g., Village produces Grain, Mine produces IronOre)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> LocalProduction;

	// What this holding's population needs to consume
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> PopulationNeeds;

	// Current stockpile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> Stockpile;

	// Who owns this holding
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwnerID; // Can be feudal lord or merchant family

	// Parent county
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ParentCountyID;

	// Population growth factors
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NeedsSatisfactionPercent; // 0.0 - 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PopulationGrowthRate; // Weekly growth %

	FHoldingData()
		: HoldingType(EHoldingType::Village)
		, Population(500)
		, Geography(EGeographyType::Plains)
		, NeedsSatisfactionPercent(0.7f)
		, PopulationGrowthRate(0.0f)
	{}
};

/**
 * County (collection of holdings - like CK3 county)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FCountyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CountyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> HoldingIDs; // All holdings in this county

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ParentDuchyID;

	// Regional market (aggregates all holdings)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> RegionalSupply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> RegionalDemand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, float> RegionalPrices;

	// Who controls the county (feudal lord)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RulerID;

	FCountyData()
	{}
};

/**
 * Duchy (collection of counties - like CK3 duchy)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FDuchyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DuchyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> CountyIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ParentKingdomID;

	// Duchy-level market
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, float> DuchyPrices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DukeID;

	// Duchy capital (main city for trade)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CapitalHoldingID;

	FDuchyData()
	{}
};

/**
 * Kingdom (collection of duchies - like CK3 kingdom)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FKingdomData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString KingdomName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> DuchyIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ParentEmpireID;

	// Kingdom tariffs and trade laws
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseTariff; // 0.05 = 5% tax on trade

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString KingID;

	FKingdomData()
		: BaseTariff(0.1f)
	{}
};

/**
 * Empire (top-level - HRE, France, etc.)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FEmpireData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EmpireName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> KingdomIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImperialTariff;

	// Political complexity (HRE = high, France = low)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoliticalComplexity; // 0.0 - 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EmperorID;

	FEmpireData()
		: ImperialTariff(0.05f)
		, PoliticalComplexity(0.5f)
	{}
};

/**
 * Passive Trade Connection (auto-generated between adjacent holdings)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FPassiveTradeConnection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HoldingA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HoldingB;

	// Distance in km
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance;

	// Geography modifier (-1.0 to 1.0, negative = harder trade)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GeographyModifier;

	// Trade flow (auto-calculated based on supply/demand)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> GoodsFlowAtoB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> GoodsFlowBtoA;

	// Weekly trade volume (gold value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TradeVolume;

	FPassiveTradeConnection()
		: Distance(50.0f)
		, GeographyModifier(0.0f)
		, TradeVolume(0.0f)
	{}
};

/**
 * Custom Trade Route (player or AI created)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FCustomTradeRoute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RouteID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETradeRouteType RouteType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OriginHoldingID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DestinationHoldingID;

	// What goods to transport
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> TargetGoodsPerWeek;

	// Ownership structure
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> OwnershipShares; // MerchantFamilyID -> % (0.0-1.0)

	// Weekly profit from this route
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeeklyProfit;

	// Route status
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlocked; // War, embargo, etc.

	FCustomTradeRoute()
		: RouteType(ETradeRouteType::PlayerCustom)
		, WeeklyProfit(0.0f)
		, bIsActive(true)
		, bIsBlocked(false)
	{}
};

/**
 * AI Merchant Dynasty (competing with player)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FAIMerchantDynasty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FamilyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FamilyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HomeHoldingID;

	// Total wealth
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalGold;

	// Holdings they own
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> OwnedHoldingIDs;

	// Trade routes they control
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> TradeRouteIDs;

	// AI personality (affects trading behavior)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Aggressiveness; // 0.0 - 1.0, higher = more competitive

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RiskTolerance; // 0.0 - 1.0, higher = more speculation

	// Relationship with player (-100 to 100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerOpinion;

	FAIMerchantDynasty()
		: TotalGold(10000.0f)
		, Aggressiveness(0.5f)
		, RiskTolerance(0.5f)
		, PlayerOpinion(0)
	{}
};

/**
 * Population Needs Template (what pops consume weekly per 100 people)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FPopulationNeedsTemplate
{
	GENERATED_BODY()

	// Basic needs (required for survival)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> BasicNeeds; // e.g., Bread: 10 per 100 people

	// Comfort needs (required for growth)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> ComfortNeeds; // e.g., Beer: 5, Garments: 2

	// Luxury needs (required for prosperity)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> LuxuryNeeds; // e.g., Spices: 1

	FPopulationNeedsTemplate()
	{}
};

/**
 * Trade Share Offer (player can buy/sell shares of routes)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FTradeShareOffer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RouteID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SellerFamilyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SharePercentage; // 0.0 - 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AskingPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeeksValid; // Offer expires

	FTradeShareOffer()
		: SharePercentage(0.0f)
		, AskingPrice(0.0f)
		, WeeksValid(4)
	{}
};
