// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GTDataTypes.h"
#include "GTMarketTypes.generated.h"

/**
 * Victoria 3-Style Market Economy
 *
 * Key Concepts:
 * - Each settlement is a LOCAL MARKET with prices for all goods
 * - Buildings buy inputs from market, produce outputs, sell to market
 * - Pops earn wages from buildings, buy goods from market
 * - Trade routes connect markets and equalize prices
 * - Complex production chains with multiple inputs/outputs
 */

// ========== POP CLASSES (Social Strata) ==========

UENUM(BlueprintType)
enum class EPopClass : uint8
{
	Peasant UMETA(DisplayName = "Peasant"),          // Farmers, laborers - basic needs only
	Craftsman UMETA(DisplayName = "Craftsman"),      // Skilled workers - some comfort goods
	Merchant UMETA(DisplayName = "Merchant"),        // Traders - luxury goods
	Noble UMETA(DisplayName = "Noble"),              // Aristocracy - expensive luxuries
	Clergy UMETA(DisplayName = "Clergy"),            // Religious - special needs

	MAX UMETA(Hidden)
};

// ========== PRODUCTION METHODS ==========

UENUM(BlueprintType)
enum class EProductionMethod : uint8
{
	Manual UMETA(DisplayName = "Manual"),            // Hand tools, low output, low wages
	Workshop UMETA(DisplayName = "Workshop"),        // Basic machinery, medium output
	Guild UMETA(DisplayName = "Guild"),              // Organized craftsmen, high quality

	MAX UMETA(Hidden)
};

// ========== BUILDING TYPES (Industries) ==========

UENUM(BlueprintType)
enum class EBuildingType_Market : uint8
{
	// Primary Production (raw resources)
	Farm UMETA(DisplayName = "Farm"),                // Grain, vegetables
	Ranch UMETA(DisplayName = "Ranch"),              // Cattle, sheep, wool
	Fishery UMETA(DisplayName = "Fishery"),          // Fish
	Forest UMETA(DisplayName = "Forest"),            // Timber
	IronMine UMETA(DisplayName = "Iron Mine"),       // Iron ore
	SaltMine UMETA(DisplayName = "Salt Mine"),       // Salt
	SilverMine UMETA(DisplayName = "Silver Mine"),   // Silver ore
	Quarry UMETA(DisplayName = "Quarry"),            // Stone

	// Secondary Production (processing)
	Mill UMETA(DisplayName = "Mill"),                // Grain → Flour
	Bakery UMETA(DisplayName = "Bakery"),            // Flour → Bread
	Brewery UMETA(DisplayName = "Brewery"),          // Grain + Hops → Beer
	Winery UMETA(DisplayName = "Winery"),            // Grapes → Wine
	Tannery UMETA(DisplayName = "Tannery"),          // Hides → Leather
	Weaver UMETA(DisplayName = "Weaver"),            // Wool/Flax → Cloth
	Smelter UMETA(DisplayName = "Smelter"),          // Ore + Coal → Metal bars
	Sawmill UMETA(DisplayName = "Sawmill"),          // Timber → Planks

	// Tertiary Production (manufacturing)
	Tailor UMETA(DisplayName = "Tailor"),            // Cloth → Garments
	Shoemaker UMETA(DisplayName = "Shoemaker"),      // Leather → Shoes
	Blacksmith UMETA(DisplayName = "Blacksmith"),    // Iron → Tools + Weapons
	Armorer UMETA(DisplayName = "Armorer"),          // Iron → Armor
	Carpenter UMETA(DisplayName = "Carpenter"),      // Planks → Furniture
	Jeweler UMETA(DisplayName = "Jeweler"),          // Gold/Silver → Jewelry
	Shipyard UMETA(DisplayName = "Shipyard"),        // Planks + Tools → Ships
	PotteryWorkshop UMETA(DisplayName = "Pottery"),  // Clay → Pottery

	// Services
	Tavern UMETA(DisplayName = "Tavern"),            // Beer + Food → Services
	Church UMETA(DisplayName = "Church"),            // Provides religious services
	Market UMETA(DisplayName = "Market Square"),     // Facilitates trade

	MAX UMETA(Hidden)
};

// ========== MARKET DATA ==========

/**
 * Market Good Entry (price, supply, demand for ONE good in ONE market)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FMarketGoodData
{
	GENERATED_BODY()

	// Current price in this market (gold per unit)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Price;

	// Available supply this week (units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Supply;

	// Total demand this week (units) from buildings + pops
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Demand;

	// Stockpile (goods available for purchase)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Stockpile;

	// Buy orders this turn (from buildings/pops)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> BuyOrders;

	// Sell orders this turn (from buildings)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SellOrders;

	FMarketGoodData()
		: Price(5.0f)
		, Supply(0)
		, Demand(0)
		, Stockpile(0)
	{}
};

/**
 * Local Market (one per settlement)
 * Tracks prices, supply, demand for ALL goods in this settlement
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FLocalMarket
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarketName; // Settlement name

	// Price/supply/demand for EVERY resource type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, FMarketGoodData> GoodsMarket;

	// Total market size (affects price stability)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MarketSize; // 0.1 (small village) to 10.0 (major city)

	// Connected markets (trade routes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> ConnectedMarkets;

	// Market efficiency (0.5 - 1.0, affects transaction costs)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MarketEfficiency;

	FLocalMarket()
		: MarketSize(1.0f)
		, MarketEfficiency(0.8f)
	{}
};

// ========== BUILDING DATA ==========

/**
 * Production Recipe (what building consumes/produces)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FProductionRecipe
{
	GENERATED_BODY()

	// Inputs consumed per week (building buys from market)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> Inputs;

	// Outputs produced per week (building sells to market)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> Outputs;

	// Labor required (number of workers)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LaborRequired;

	// Wages paid per worker per week
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WagePerWorker;

	// Base efficiency (can be modified by tech, manager, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseEfficiency;

	FProductionRecipe()
		: LaborRequired(10)
		, WagePerWorker(1.0f)
		, BaseEfficiency(1.0f)
	{}
};

/**
 * Market Building (produces goods by consuming inputs from market)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FMarketBuilding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BuildingID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildingType_Market BuildingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarketName; // Which market it's in

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProductionRecipe Recipe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProductionMethod ProductionMethod;

	// Owner (player or AI family)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwnerID;

	// Employed pops (workers in this building)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> EmployedPopIDs;

	// Current efficiency (affected by manager, technology, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentEfficiency;

	// Profitability last turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastTurnProfit;

	// Is building operational?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive;

	FMarketBuilding()
		: BuildingType(EBuildingType_Market::Farm)
		, ProductionMethod(EProductionMethod::Manual)
		, CurrentEfficiency(1.0f)
		, LastTurnProfit(0.0f)
		, bIsActive(true)
	{}
};

// ========== POP DATA ==========

/**
 * Pop Needs Template (what each pop class needs to consume)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FPopNeedsTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPopClass PopClass;

	// Goods needed per week per pop (100 people)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> WeeklyNeeds;

	// Minimum satisfaction to avoid unrest (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumSatisfaction;

	FPopNeedsTemplate()
		: PopClass(EPopClass::Peasant)
		, MinimumSatisfaction(0.5f)
	{}
};

/**
 * Pop Group (population with class, employment, consumption)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FPopGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PopID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPopClass PopClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Size; // Number of people in this group

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarketName; // Which market they live in

	// Employment
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EmployedInBuildingID; // Empty if unemployed

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeeklyWage; // Income from employment

	// Consumption
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NeedsSatisfaction; // 0.0 - 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cash; // Money available to spend

	// Happiness/Unrest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Militancy; // 0.0 - 1.0, higher = more likely to revolt

	FPopGroup()
		: PopClass(EPopClass::Peasant)
		, Size(100)
		, WeeklyWage(0.0f)
		, NeedsSatisfaction(0.5f)
		, Cash(10.0f)
		, Militancy(0.0f)
	{}
};

// ========== TRADE BETWEEN MARKETS ==========

/**
 * Market Trade Route (connects two markets for goods flow)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FMarketTradeRoute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RouteID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarketA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MarketB;

	// Trade capacity (max units that can flow per week)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TradeCapacity;

	// Trade cost (gold per unit shipped)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TransportCost;

	// Is route active?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive;

	FMarketTradeRoute()
		: TradeCapacity(1000)
		, TransportCost(0.5f)
		, bIsActive(true)
	{}
};
