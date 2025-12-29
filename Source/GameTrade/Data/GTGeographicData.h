// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GTDataTypes.h"
#include "GTGeographicData.generated.h"

/**
 * Real-World Geographic Data Integration
 *
 * Data Sources (All Royalty-Free):
 * - Harvard MAPS (Mapping Past Societies): Medieval GIS data, shapefiles
 * - Natural Earth Data: Public domain topography and elevation (SRTM Plus)
 * - OpenHistoricalMap: Medieval cities and roads (1350-1650)
 * - Research Dataset: 173 medieval cities with coordinates (ca. 1300)
 * - Viabundus: Northern Europe street map (1350-1650)
 *
 * Coordinate System:
 * - Input: WGS84 Lat/Lon (standard GPS coordinates)
 * - Output: UE5 World Space (X, Y in cm)
 * - Scale: 1 degree = 100km = 10,000,000 UE5 units (cm)
 */

// ========== GEOGRAPHIC COORDINATES ==========

/**
 * Real-world geographic coordinates (Latitude/Longitude)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FGeographicCoordinate
{
	GENERATED_BODY()

	// Latitude in decimal degrees (-90 to +90, North positive)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Latitude;

	// Longitude in decimal degrees (-180 to +180, East positive)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Longitude;

	// Elevation above sea level in meters
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Elevation;

	FGeographicCoordinate()
		: Latitude(0.0)
		, Longitude(0.0)
		, Elevation(0.0f)
	{}

	FGeographicCoordinate(double InLat, double InLon, float InElev = 0.0f)
		: Latitude(InLat)
		, Longitude(InLon)
		, Elevation(InElev)
	{}
};

/**
 * UE5 World Space coordinates
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FWorldSpaceCoordinate
{
	GENERATED_BODY()

	// X coordinate in UE5 world space (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double X;

	// Y coordinate in UE5 world space (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Y;

	// Z coordinate (elevation) in UE5 world space (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z;

	FWorldSpaceCoordinate()
		: X(0.0)
		, Y(0.0)
		, Z(0.0f)
	{}

	FWorldSpaceCoordinate(double InX, double InY, float InZ = 0.0f)
		: X(InX)
		, Y(InY)
		, Z(InZ)
	{}

	// Convert to FVector for UE5 usage
	FVector ToVector() const
	{
		return FVector(X, Y, Z);
	}
};

// ========== TERRAIN TYPES ==========

/**
 * Terrain types based on real-world topography
 */
UENUM(BlueprintType)
enum class ETerrainType : uint8
{
	// Flat terrain
	Plains UMETA(DisplayName = "Plains"),           // Elevation < 200m, flat
	Farmland UMETA(DisplayName = "Farmland"),       // Cultivated plains
	Steppe UMETA(DisplayName = "Steppe"),           // Dry plains

	// Hills and valleys
	Hills UMETA(DisplayName = "Hills"),             // Elevation 200-600m
	Valleys UMETA(DisplayName = "Valleys"),         // River valleys

	// Mountains
	Mountains UMETA(DisplayName = "Mountains"),     // Elevation > 600m
	HighMountains UMETA(DisplayName = "High Mountains"), // Elevation > 1500m (Alps, Pyrenees)

	// Water bodies
	Coast UMETA(DisplayName = "Coast"),             // Near sea
	RiverBank UMETA(DisplayName = "River Bank"),    // Along major rivers
	Lake UMETA(DisplayName = "Lake"),               // Lake shore

	// Forests
	Forest UMETA(DisplayName = "Forest"),           // Dense forest (Black Forest, Ardennes)
	MixedForest UMETA(DisplayName = "Mixed Forest"), // Scattered woodland

	// Special
	Marshland UMETA(DisplayName = "Marshland"),     // Wetlands, swamps
	Desert UMETA(DisplayName = "Desert"),           // Arid regions (rare in medieval Europe)

	MAX UMETA(Hidden)
};

/**
 * Climate zones (affects agriculture and trade)
 */
UENUM(BlueprintType)
enum class EClimateZone : uint8
{
	Mediterranean UMETA(DisplayName = "Mediterranean"), // Southern Europe, warm dry summers
	Continental UMETA(DisplayName = "Continental"),     // Central Europe, cold winters
	Oceanic UMETA(DisplayName = "Oceanic"),            // Western Europe, mild and wet
	Alpine UMETA(DisplayName = "Alpine"),              // Mountain regions, harsh winters
	Nordic UMETA(DisplayName = "Nordic"),              // Scandinavia, very cold

	MAX UMETA(Hidden)
};

// ========== HISTORICAL CITY DATA ==========

/**
 * Historical city data with real coordinates
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FHistoricalCityData : public FTableRowBase
{
	GENERATED_BODY()

	// City name (English)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CityName;

	// Medieval name (if different)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MedievalName;

	// Real-world coordinates
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeographicCoordinate Coordinates;

	// Country/Region (1347 political entity)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PoliticalEntity; // "Holy Roman Empire", "Kingdom of France", etc.

	// Settlement type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHoldingType HoldingType;

	// Historical population (ca. 1350)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HistoricalPopulation;

	// Terrain at location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETerrainType TerrainType;

	// Climate zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClimateZone ClimateZone;

	// Trade importance (1-10, based on historical data)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TradeImportance;

	// Major trade goods produced/traded
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EResourceType> HistoricalGoods;

	// Connected to major trade route?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnSilkRoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnHanseaticRoute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnMediterraneanRoute;

	// On major river?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RiverName; // "Rhine", "Danube", "Thames", etc. (empty if none)

	FHistoricalCityData()
		: HistoricalPopulation(5000)
		, HoldingType(EHoldingType::City)
		, TerrainType(ETerrainType::Plains)
		, ClimateZone(EClimateZone::Continental)
		, TradeImportance(5)
		, bOnSilkRoad(false)
		, bOnHanseaticRoute(false)
		, bOnMediterraneanRoute(false)
	{}
};

// ========== TERRAIN HEIGHTMAP DATA ==========

/**
 * Heightmap cell (for terrain generation from real elevation data)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FHeightmapCell
{
	GENERATED_BODY()

	// Geographic bounds of this cell
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeographicCoordinate MinCoord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeographicCoordinate MaxCoord;

	// Average elevation in meters
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Elevation;

	// Terrain type determined from elevation and data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETerrainType TerrainType;

	// Slope (0.0 = flat, 1.0 = steep)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Slope;

	FHeightmapCell()
		: Elevation(0.0f)
		, TerrainType(ETerrainType::Plains)
		, Slope(0.0f)
	{}
};

// ========== MAJOR RIVERS ==========

/**
 * Major European rivers (from Natural Earth Data)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FMajorRiver
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RiverName;

	// Polyline of river path (lat/lon coordinates)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGeographicCoordinate> RiverPath;

	// Trade bonus multiplier for settlements on this river
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TradeBonusMultiplier;

	// Is navigable? (affects trade)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNavigable;

	FMajorRiver()
		: TradeBonusMultiplier(1.5f)
		, bNavigable(true)
	{}
};

// ========== HISTORICAL TRADE ROUTES ==========

/**
 * Historical trade routes (Silk Road, Hanseatic, Mediterranean)
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FHistoricalTradeRoute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RouteName; // "Silk Road", "Hanseatic League", "Via Francigena", etc.

	// Major cities along this route (references to FHistoricalCityData)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> CitiesOnRoute;

	// Key goods traded on this route
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EResourceType> TradeGoods;

	// Route importance (1-10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Importance;

	// Is route active in 1347?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive;

	FHistoricalTradeRoute()
		: Importance(5)
		, bActive(true)
	{}
};

// ========== GEOGRAPHIC REGION ==========

/**
 * Geographic region (e.g., "Bavaria", "Tuscany", "Flanders")
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FGeographicRegion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RegionName;

	// Bounding box of region
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeographicCoordinate SouthWestCorner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeographicCoordinate NorthEastCorner;

	// Dominant terrain type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETerrainType DominantTerrain;

	// Climate zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClimateZone ClimateZone;

	// Major cities in this region
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> MajorCities;

	// Regional specialty goods
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EResourceType> RegionalGoods;

	FGeographicRegion()
		: DominantTerrain(ETerrainType::Plains)
		, ClimateZone(EClimateZone::Continental)
	{}
};

// ========== DISTANCE CALCULATION ==========

/**
 * Precomputed distance matrix between cities
 */
USTRUCT(BlueprintType)
struct GAMETRADE_API FCityDistanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CityA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CityB;

	// Straight-line distance in km
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceKm;

	// Travel time in days (medieval conditions)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TravelTimeDays;

	// Terrain difficulty (1.0 = easy, 5.0 = very difficult)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainDifficulty;

	FCityDistanceData()
		: DistanceKm(0.0f)
		, TravelTimeDays(0)
		, TerrainDifficulty(1.0f)
	{}
};
