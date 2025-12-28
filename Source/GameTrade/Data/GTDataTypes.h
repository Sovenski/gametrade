// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GTDataTypes.generated.h"

/**
 * Core data types and enums for the game
 */

// ========== ENUMS ==========

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	// Tier 1 - Raw Resources
	// Agricultural
	Grain UMETA(DisplayName = "Grain"),
	Grapes UMETA(DisplayName = "Grapes"),
	Olives UMETA(DisplayName = "Olives"),
	Hops UMETA(DisplayName = "Hops"),
	Flax UMETA(DisplayName = "Flax"),
	Hemp UMETA(DisplayName = "Hemp"),

	// Livestock
	Cattle UMETA(DisplayName = "Cattle"),
	Sheep UMETA(DisplayName = "Sheep"),
	Pigs UMETA(DisplayName = "Pigs"),
	Horses UMETA(DisplayName = "Horses"),
	Wool UMETA(DisplayName = "Wool"),
	Hides UMETA(DisplayName = "Hides"),

	// Forestry
	Timber UMETA(DisplayName = "Timber"),
	Game UMETA(DisplayName = "Game"),
	Furs UMETA(DisplayName = "Furs"),

	// Mining
	IronOre UMETA(DisplayName = "Iron Ore"),
	Copper UMETA(DisplayName = "Copper"),
	Tin UMETA(DisplayName = "Tin"),
	Silver UMETA(DisplayName = "Silver"),
	Salt UMETA(DisplayName = "Salt"),
	Stone UMETA(DisplayName = "Stone"),

	// Fishing
	Herring UMETA(DisplayName = "Herring"),
	Cod UMETA(DisplayName = "Cod"),
	Fish UMETA(DisplayName = "Fish"),

	// Tier 2 - Processed Goods
	Flour UMETA(DisplayName = "Flour"),
	Bread UMETA(DisplayName = "Bread"),
	Wine UMETA(DisplayName = "Wine"),
	Beer UMETA(DisplayName = "Beer"),
	OliveOil UMETA(DisplayName = "Olive Oil"),
	Leather UMETA(DisplayName = "Leather"),
	WoolenCloth UMETA(DisplayName = "Woolen Cloth"),
	Planks UMETA(DisplayName = "Planks"),
	Charcoal UMETA(DisplayName = "Charcoal"),
	SmokedMeat UMETA(DisplayName = "Smoked Meat"),
	IronBars UMETA(DisplayName = "Iron Bars"),
	Bronze UMETA(DisplayName = "Bronze"),
	SilverIngots UMETA(DisplayName = "Silver Ingots"),

	// Tier 3 - Manufactured Goods
	Garments UMETA(DisplayName = "Garments"),
	LuxuryCloth UMETA(DisplayName = "Luxury Cloth"),
	Tools UMETA(DisplayName = "Tools"),
	Weapons UMETA(DisplayName = "Weapons"),
	Armor UMETA(DisplayName = "Armor"),
	Shoes UMETA(DisplayName = "Shoes"),
	Boots UMETA(DisplayName = "Boots"),
	Saddles UMETA(DisplayName = "Saddles"),
	Silverware UMETA(DisplayName = "Silverware"),

	// Tier 4 - Luxury & Exotic
	Silk UMETA(DisplayName = "Silk"),
	Spices UMETA(DisplayName = "Spices"),
	Dyes UMETA(DisplayName = "Dyes"),
	Amber UMETA(DisplayName = "Amber"),
	VenetianGlass UMETA(DisplayName = "Venetian Glass"),
	Tapestries UMETA(DisplayName = "Tapestries"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESettlementType : uint8
{
	Village UMETA(DisplayName = "Village"),
	Town UMETA(DisplayName = "Town"),
	City UMETA(DisplayName = "City"),
	MajorCity UMETA(DisplayName = "Major City"),
	CapitalCity UMETA(DisplayName = "Capital City")
};

UENUM(BlueprintType)
enum class ETerritoryType : uint8
{
	// Holy Roman Empire
	HRE_FreeCity UMETA(DisplayName = "Free Imperial City"),
	HRE_Duchy UMETA(DisplayName = "Duchy"),
	HRE_County UMETA(DisplayName = "County"),
	HRE_Bishopric UMETA(DisplayName = "Bishopric"),

	// France
	France_Royal UMETA(DisplayName = "Royal Domain"),
	France_Duchy UMETA(DisplayName = "French Duchy"),

	// Italy
	Italy_Republic UMETA(DisplayName = "Republic"),
	Italy_Duchy UMETA(DisplayName = "Italian Duchy"),
	Italy_Papal UMETA(DisplayName = "Papal States"),
	Italy_Kingdom UMETA(DisplayName = "Kingdom of Naples"),

	// Others
	England_Crown UMETA(DisplayName = "English Crown"),
	Castile_Crown UMETA(DisplayName = "Crown of Castile"),
	Aragon_Crown UMETA(DisplayName = "Crown of Aragon"),
	Poland_Kingdom UMETA(DisplayName = "Kingdom of Poland"),
	Hungary_Kingdom UMETA(DisplayName = "Kingdom of Hungary")
};

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	// Production
	Farm UMETA(DisplayName = "Farm"),
	Pasture UMETA(DisplayName = "Pasture"),
	Vineyard UMETA(DisplayName = "Vineyard"),
	Mine UMETA(DisplayName = "Mine"),
	Quarry UMETA(DisplayName = "Quarry"),
	LoggingCamp UMETA(DisplayName = "Logging Camp"),
	FishingBoats UMETA(DisplayName = "Fishing Boats"),

	// Processing
	Mill UMETA(DisplayName = "Mill"),
	Bakery UMETA(DisplayName = "Bakery"),
	Brewery UMETA(DisplayName = "Brewery"),
	WinePress UMETA(DisplayName = "Wine Press"),
	Tannery UMETA(DisplayName = "Tannery"),
	WeavingWorkshop UMETA(DisplayName = "Weaving Workshop"),
	Sawmill UMETA(DisplayName = "Sawmill"),
	Forge UMETA(DisplayName = "Forge"),
	Smelter UMETA(DisplayName = "Smelter"),

	// Manufacturing
	Blacksmith UMETA(DisplayName = "Blacksmith"),
	Armorer UMETA(DisplayName = "Armorer"),
	TailorShop UMETA(DisplayName = "Tailor Shop"),
	Cobbler UMETA(DisplayName = "Cobbler"),
	Silversmith UMETA(DisplayName = "Silversmith"),

	// Infrastructure
	Warehouse UMETA(DisplayName = "Warehouse"),
	Inn UMETA(DisplayName = "Inn"),
	Market UMETA(DisplayName = "Market"),
	Road UMETA(DisplayName = "Road"),
	Bridge UMETA(DisplayName = "Bridge"),

	// Special
	Bank UMETA(DisplayName = "Bank"),
	TradingPost UMETA(DisplayName = "Trading Post")
};

UENUM(BlueprintType)
enum class ECharacterTrait : uint8
{
	// Positive
	Genius UMETA(DisplayName = "Genius"),
	Charismatic UMETA(DisplayName = "Charismatic"),
	Diligent UMETA(DisplayName = "Diligent"),
	Brave UMETA(DisplayName = "Brave"),
	Patient UMETA(DisplayName = "Patient"),
	Shrewd UMETA(DisplayName = "Shrewd"),

	// Negative
	Slow UMETA(DisplayName = "Slow"),
	Greedy UMETA(DisplayName = "Greedy"),
	Wrathful UMETA(DisplayName = "Wrathful"),
	Craven UMETA(DisplayName = "Craven"),
	Slothful UMETA(DisplayName = "Slothful"),
	Trusting UMETA(DisplayName = "Trusting"),

	// Neutral/Special
	Ambitious UMETA(DisplayName = "Ambitious"),
	Cynical UMETA(DisplayName = "Cynical"),
	Zealous UMETA(DisplayName = "Zealous")
};

UENUM(BlueprintType)
enum class ESeason : uint8
{
	Spring UMETA(DisplayName = "Spring"),
	Summer UMETA(DisplayName = "Summer"),
	Autumn UMETA(DisplayName = "Autumn"),
	Winter UMETA(DisplayName = "Winter")
};

// ========== STRUCTS ==========

USTRUCT(BlueprintType)
struct FGameDate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Year = 1347;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Week = 1; // 1-52

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	ESeason Season = ESeason::Spring;

	FString ToString() const
	{
		return FString::Printf(TEXT("Year %d, Week %d (%s)"), Year, Week, *UEnum::GetValueAsString(Season));
	}

	void AdvanceWeek()
	{
		Week++;
		if (Week > 52)
		{
			Week = 1;
			Year++;
		}

		// Update season
		if (Week >= 1 && Week <= 13) Season = ESeason::Spring;
		else if (Week >= 14 && Week <= 26) Season = ESeason::Summer;
		else if (Week >= 27 && Week <= 39) Season = ESeason::Autumn;
		else Season = ESeason::Winter;
	}
};

USTRUCT(BlueprintType)
struct FResourceAmount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EResourceType ResourceType = EResourceType::Grain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Amount = 0;

	FResourceAmount() {}

	FResourceAmount(EResourceType Type, int32 Qty)
		: ResourceType(Type), Amount(Qty) {}
};

USTRUCT(BlueprintType)
struct FPriceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float BasePrice = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float CurrentPrice = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 Supply = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 Demand = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float PriceChangePercent = 0.0f;
};

USTRUCT(BlueprintType)
struct FSettlementData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	FString SettlementName = TEXT("Unknown");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	ESettlementType SettlementType = ESettlementType::Village;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	FString TerritoryName = TEXT("Unknown");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	ETerritoryType TerritoryType = ETerritoryType::HRE_Duchy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	int32 Population = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	FVector2D MapLocation = FVector2D::ZeroVector;

	// Resources this settlement produces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	TArray<EResourceType> LocalProduction;

	// Resources this settlement needs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	TArray<EResourceType> LocalNeeds;

	// Prices for all goods (indexed by resource type)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	TMap<EResourceType, FPriceInfo> Prices;

	// Connected settlements (road network)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	TArray<FString> ConnectedSettlements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settlement")
	float BaseTariff = 0.02f; // 2% base tariff
};

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Commerce = 1; // Trading skill (0-10)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Stewardship = 1; // Managing businesses (0-10)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Intrigue = 1; // Espionage, sabotage (0-10)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Leadership = 1; // Managing people (0-10)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Combat = 1; // Fighting, protection (0-10)
};

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString CharacterID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString FirstName = TEXT("Unknown");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString FamilyName = TEXT("Unknown");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsMale = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Age = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FCharacterStats Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TArray<ECharacterTrait> Traits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString SpouseID = TEXT(""); // Empty if unmarried

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TArray<FString> ChildrenIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString CurrentSettlement = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FString AssignedBuilding = TEXT(""); // Managing which building

	FString GetFullName() const
	{
		return FirstName + TEXT(" ") + FamilyName;
	}
};

USTRUCT(BlueprintType)
struct FBuildingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString BuildingID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingType BuildingType = EBuildingType::Farm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString SettlementName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString OwnerFamilyID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	int32 Level = 1; // 1-5

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FString ManagerCharacterID = TEXT(""); // Who runs it

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	int32 Workers = 0;

	// Production chain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TArray<FResourceAmount> InputsPerWeek;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TArray<FResourceAmount> OutputsPerWeek;
};

USTRUCT(BlueprintType)
struct FCaravanData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	FString CaravanID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	FString OwnerFamilyID = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	FString CurrentLocation = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	FString Destination = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	int32 WeeksToDestination = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	int32 Capacity = 20; // How many units it can carry

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	TArray<FResourceAmount> Cargo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	bool bIsAutomatic = false; // Repeating route
};
