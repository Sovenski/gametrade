// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTGeographicData.h"
#include "GTWorldMapSubsystem.generated.h"

/**
 * World Map Subsystem
 *
 * Manages real-world geographic data integration for medieval Europe
 *
 * Data Sources:
 * - Harvard MAPS: Free GIS shapefiles for medieval settlements
 * - Natural Earth Data: Public domain elevation and terrain (SRTM Plus)
 * - OpenHistoricalMap: Medieval cities and roads (1350-1650)
 * - Research Dataset: 173 cities with coordinates (ca. 1300)
 *
 * Features:
 * - Convert lat/lon to UE5 world space
 * - Generate terrain from real elevation data
 * - Place cities at real coordinates
 * - Calculate distances with terrain penalties
 * - Determine trade routes based on geography
 * - Integrate with hierarchical economy system
 */
UCLASS()
class GAMETRADE_API UGTWorldMapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========== INITIALIZATION ==========

	/** Load all geographic data from CSV and data tables */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void InitializeWorldMap();

	/** Load historical city data from CSV */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void LoadHistoricalCities(const FString& CSVPath);

	/** Load major rivers from data */
	void LoadMajorRivers();

	/** Load historical trade routes */
	void LoadHistoricalTradeRoutes();

	/** Precompute distance matrix between all cities */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void PrecomputeDistances();

	// ========== COORDINATE CONVERSION ==========

	/**
	 * Convert geographic coordinates (lat/lon) to UE5 world space
	 *
	 * Projection: Mercator (suitable for Europe, minimal distortion)
	 * Center Point: 50°N, 10°E (Central Europe)
	 * Scale: 1 degree = 100km = 10,000,000 UE5 units (cm)
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	FWorldSpaceCoordinate ConvertToWorldSpace(FGeographicCoordinate GeoCoord);

	/** Convert UE5 world space back to lat/lon */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	FGeographicCoordinate ConvertToGeoCoordinate(FWorldSpaceCoordinate WorldCoord);

	/** Get UE5 position for a city by name */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	FVector GetCityPosition(const FString& CityName);

	// ========== TERRAIN QUERIES ==========

	/** Get terrain type at geographic coordinate */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	ETerrainType GetTerrainAtLocation(FGeographicCoordinate GeoCoord);

	/** Get elevation at geographic coordinate (meters) */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	float GetElevationAtLocation(FGeographicCoordinate GeoCoord);

	/** Check if two locations have line-of-sight (no mountains blocking) */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	bool HasLineOfSight(FGeographicCoordinate A, FGeographicCoordinate B);

	/** Determine terrain type from elevation */
	ETerrainType DetermineTerrainFromElevation(float Elevation, float Slope);

	// ========== DISTANCE CALCULATIONS ==========

	/**
	 * Calculate great-circle distance between two coordinates (km)
	 * Uses Haversine formula for sphere
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	float CalculateDistance(FGeographicCoordinate A, FGeographicCoordinate B);

	/**
	 * Calculate travel time between two cities (in days)
	 * Accounts for terrain, roads, rivers
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	int32 CalculateTravelTime(const FString& CityA, const FString& CityB);

	/**
	 * Get terrain difficulty multiplier for travel
	 * 1.0 = plains/roads, 2.0 = hills, 5.0 = mountains
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	float GetTerrainDifficulty(ETerrainType Terrain);

	// ========== TRADE ROUTE GENERATION ==========

	/**
	 * Find optimal trade route between two cities
	 * Uses A* pathfinding with terrain costs
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FString> FindTradeRoute(const FString& StartCity, const FString& EndCity);

	/**
	 * Check if two cities are connected by river
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	bool AreConnectedByRiver(const FString& CityA, const FString& CityB);

	/**
	 * Get river connecting two cities (if any)
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	FString GetConnectingRiver(const FString& CityA, const FString& CityB);

	// ========== HISTORICAL DATA QUERIES ==========

	/** Get city data by name (C++ only, returns pointer) */
	FHistoricalCityData* GetCityData(const FString& CityName);

	/** Get city data by name (Blueprint-safe, returns copy) */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	bool GetCityDataCopy(const FString& CityName, FHistoricalCityData& OutCityData);

	/** Get all cities in a region */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FString> GetCitiesInRegion(const FString& RegionName);

	/** Get all cities on a historical trade route */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FString> GetCitiesOnTradeRoute(const FString& RouteName);

	/** Get nearest cities to a location (within radius km) */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FString> GetNearbyCities(FGeographicCoordinate Location, float RadiusKm);

	/** Get all cities within a political entity (e.g., "Holy Roman Empire") */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FString> GetCitiesInPoliticalEntity(const FString& EntityName);

	// ========== TERRAIN GENERATION ==========

	/**
	 * Generate UE5 landscape from heightmap data
	 * (To be implemented with UE5 Landscape API)
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void GenerateTerrainFromHeightmap();

	/**
	 * Spawn city actors at real coordinates
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void SpawnCityActors();

	/**
	 * Generate river meshes from polyline data
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void GenerateRiverMeshes();

	// ========== INTEGRATION WITH ECONOMY ==========

	/**
	 * Setup hierarchical economy with real geography
	 * Calls hierarchical economy subsystem to create holdings at real locations
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	void IntegrateWithHierarchicalEconomy();

	/**
	 * Determine passive trade connections based on geography
	 * Adjacent cities trade unless separated by mountains/sea
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	TArray<FCityConnection> GeneratePassiveTradeConnections();

	/**
	 * Apply geography modifiers to trade efficiency
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map")
	float CalculateTradeEfficiency(const FString& CityA, const FString& CityB);

	// ========== DEBUGGING ==========

	/** Print city data for debugging */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintCityData(const FString& CityName);

	/** Print all cities on a trade route */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintTradeRoute(const FString& RouteName);

	/** Visualize map in editor (debug draw) */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawMap();

private:
	// ========== DATA STORAGE ==========

	/** All historical cities with real coordinates */
	UPROPERTY()
	TMap<FString, FHistoricalCityData> HistoricalCities;

	/** Major European rivers */
	UPROPERTY()
	TArray<FMajorRiver> MajorRivers;

	/** Historical trade routes (Silk Road, Hanseatic, etc.) */
	UPROPERTY()
	TArray<FHistoricalTradeRoute> HistoricalTradeRoutes;

	/** Geographic regions (Bavaria, Tuscany, etc.) */
	UPROPERTY()
	TMap<FString, FGeographicRegion> GeographicRegions;

	/** Precomputed distance matrix for fast lookups */
	UPROPERTY()
	TMap<FString, FCityDistanceData> DistanceMatrix;

	/** Heightmap data (for terrain generation) */
	UPROPERTY()
	TArray<FHeightmapCell> HeightmapData;

	// ========== MAP PROJECTION SETTINGS ==========

	/** Center point for map projection (Central Europe) */
	FGeographicCoordinate MapCenter;

	/** Mercator projection scale (UE5 units per degree) */
	double ProjectionScale;

	/** Earth radius in km (for distance calculations) */
	static constexpr double EarthRadiusKm = 6371.0;

	// ========== HELPER FUNCTIONS ==========

	/** Convert degrees to radians */
	static double DegToRad(double Degrees) { return Degrees * PI / 180.0; }

	/** Convert radians to degrees */
	static double RadToDeg(double Radians) { return Radians * 180.0 / PI; }

	/** Haversine formula for great-circle distance */
	double HaversineDistance(double Lat1, double Lon1, double Lat2, double Lon2);

	/** Create distance matrix key from two city names (alphabetical order) */
	FString MakeDistanceKey(const FString& CityA, const FString& CityB);

	/** Sample elevation data from heightmap */
	float SampleHeightmap(FGeographicCoordinate Coord);

	/** Interpolate between heightmap cells */
	float InterpolateElevation(FGeographicCoordinate Coord);

	/** Check if coordinate is in water (sea/lake) */
	bool IsWater(FGeographicCoordinate Coord);
};
