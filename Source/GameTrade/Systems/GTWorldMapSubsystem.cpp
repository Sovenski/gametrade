// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTWorldMapSubsystem.h"
#include "Systems/GTHierarchicalEconomySubsystem.h"
#include "Data/GTHierarchyTypes.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UGTWorldMapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Set map projection parameters
	// Center: 50°N, 10°E (approximate center of HRE)
	MapCenter = FGeographicCoordinate(50.0, 10.0, 0.0f);

	// Scale: 1 degree latitude ≈ 111 km
	// We want 100 km = 10,000,000 UE5 units (cm)
	// So 1 degree = 111 km = 11,100,000 UE5 units
	ProjectionScale = 11100000.0; // UE5 units per degree

	UE_LOG(LogTemp, Log, TEXT("GTWorldMapSubsystem initialized with center (%.2f, %.2f), scale %.0f"),
		MapCenter.Latitude, MapCenter.Longitude, ProjectionScale);
}

void UGTWorldMapSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

// ========== INITIALIZATION ==========

void UGTWorldMapSubsystem::InitializeWorldMap()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing World Map..."));

	// Load historical cities from CSV
	FString CSVPath = FPaths::ProjectContentDir() + TEXT("Data/HistoricalCities.csv");
	LoadHistoricalCities(CSVPath);

	// Load rivers and trade routes
	LoadMajorRivers();
	LoadHistoricalTradeRoutes();

	// Precompute distances
	PrecomputeDistances();

	UE_LOG(LogTemp, Log, TEXT("World Map initialized: %d cities, %d rivers, %d trade routes"),
		HistoricalCities.Num(), MajorRivers.Num(), HistoricalTradeRoutes.Num());
}

void UGTWorldMapSubsystem::LoadHistoricalCities(const FString& CSVPath)
{
	FString CSVContent;
	if (!FFileHelper::LoadFileToString(CSVContent, *CSVPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load cities CSV from %s"), *CSVPath);
		return;
	}

	TArray<FString> Lines;
	CSVContent.ParseIntoArrayLines(Lines);

	// Skip header
	for (int32 i = 1; i < Lines.Num(); ++i)
	{
		FString Line = Lines[i];
		TArray<FString> Fields;
		Line.ParseIntoArray(Fields, TEXT(","), false);

		if (Fields.Num() < 6)
		{
			continue; // Skip malformed lines
		}

		// Parse: CityName, Latitude, Longitude, Population, PoliticalEntity, TradeImportance
		FHistoricalCityData CityData;
		CityData.CityName = Fields[0].TrimStartAndEnd();
		CityData.Coordinates.Latitude = FCString::Atod(*Fields[1]);
		CityData.Coordinates.Longitude = FCString::Atod(*Fields[2]);
		CityData.HistoricalPopulation = FCString::Atoi(*Fields[3]);
		CityData.PoliticalEntity = Fields[4].TrimStartAndEnd();
		CityData.TradeImportance = FCString::Atoi(*Fields[5]);

		// Determine terrain from elevation (simplified)
		CityData.TerrainType = DetermineTerrainFromElevation(CityData.Coordinates.Elevation, 0.0f);

		// Add to map
		HistoricalCities.Add(CityData.CityName, CityData);
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d historical cities"), HistoricalCities.Num());
}

void UGTWorldMapSubsystem::LoadMajorRivers()
{
	// Hardcode major European rivers (in production, load from shapefile)

	// Rhine (Alps to North Sea)
	FMajorRiver Rhine;
	Rhine.RiverName = TEXT("Rhine");
	Rhine.bNavigable = true;
	Rhine.TradeBonusMultiplier = 2.0f;
	Rhine.RiverPath.Add(FGeographicCoordinate(47.6, 9.2));  // Source (Switzerland)
	Rhine.RiverPath.Add(FGeographicCoordinate(50.9, 6.9));  // Köln
	Rhine.RiverPath.Add(FGeographicCoordinate(51.9, 4.5));  // Rotterdam
	MajorRivers.Add(Rhine);

	// Danube (Black Forest to Black Sea)
	FMajorRiver Danube;
	Danube.RiverName = TEXT("Danube");
	Danube.bNavigable = true;
	Danube.TradeBonusMultiplier = 1.8f;
	Danube.RiverPath.Add(FGeographicCoordinate(47.9, 8.2));  // Source
	Danube.RiverPath.Add(FGeographicCoordinate(48.2, 16.4)); // Wien
	Danube.RiverPath.Add(FGeographicCoordinate(47.5, 19.0)); // Budapest
	Danube.RiverPath.Add(FGeographicCoordinate(44.4, 26.1)); // Bucharest
	MajorRivers.Add(Danube);

	// Seine (Paris to English Channel)
	FMajorRiver Seine;
	Seine.RiverName = TEXT("Seine");
	Seine.bNavigable = true;
	Seine.TradeBonusMultiplier = 1.5f;
	Seine.RiverPath.Add(FGeographicCoordinate(47.5, 4.8));  // Source
	Seine.RiverPath.Add(FGeographicCoordinate(48.9, 2.3));  // Paris
	Seine.RiverPath.Add(FGeographicCoordinate(49.4, 0.1));  // Le Havre
	MajorRivers.Add(Seine);

	// Thames (London)
	FMajorRiver Thames;
	Thames.RiverName = TEXT("Thames");
	Thames.bNavigable = true;
	Thames.TradeBonusMultiplier = 1.5f;
	Thames.RiverPath.Add(FGeographicCoordinate(51.7, -1.8)); // Source
	Thames.RiverPath.Add(FGeographicCoordinate(51.5, -0.1)); // London
	MajorRivers.Add(Thames);

	// Tiber (Rome)
	FMajorRiver Tiber;
	Tiber.RiverName = TEXT("Tiber");
	Tiber.bNavigable = false;
	Tiber.TradeBonusMultiplier = 1.2f;
	Tiber.RiverPath.Add(FGeographicCoordinate(43.8, 12.1)); // Source
	Tiber.RiverPath.Add(FGeographicCoordinate(41.9, 12.5)); // Rome
	MajorRivers.Add(Tiber);

	UE_LOG(LogTemp, Log, TEXT("Loaded %d major rivers"), MajorRivers.Num());
}

void UGTWorldMapSubsystem::LoadHistoricalTradeRoutes()
{
	// Silk Road (Eastern terminus in medieval Europe)
	FHistoricalTradeRoute SilkRoad;
	SilkRoad.RouteName = TEXT("Silk Road");
	SilkRoad.CitiesOnRoute = {TEXT("Constantinople"), TEXT("Venice"), TEXT("Genoa")};
	SilkRoad.TradeGoods = {EResourceType::Silk, EResourceType::Spices, EResourceType::Dyes};
	SilkRoad.Importance = 10;
	SilkRoad.bActive = true;
	HistoricalTradeRoutes.Add(SilkRoad);

	// Hanseatic League (Northern Europe)
	FHistoricalTradeRoute Hanseatic;
	Hanseatic.RouteName = TEXT("Hanseatic League");
	Hanseatic.CitiesOnRoute = {TEXT("Lübeck"), TEXT("Hamburg"), TEXT("Bremen"), TEXT("Danzig"), TEXT("Riga"), TEXT("Novgorod")};
	Hanseatic.TradeGoods = {EResourceType::Timber, EResourceType::Fish, EResourceType::Grain, EResourceType::Salt, EResourceType::Wool};
	Hanseatic.Importance = 9;
	Hanseatic.bActive = true;
	HistoricalTradeRoutes.Add(Hanseatic);

	// Mediterranean Route
	FHistoricalTradeRoute Mediterranean;
	Mediterranean.RouteName = TEXT("Mediterranean Trade");
	Mediterranean.CitiesOnRoute = {TEXT("Venice"), TEXT("Genoa"), TEXT("Barcelona"), TEXT("Marseille"), TEXT("Naples")};
	Mediterranean.TradeGoods = {EResourceType::Wine, EResourceType::Olive_Oil, EResourceType::Silk, EResourceType::Spices};
	Mediterranean.Importance = 8;
	Mediterranean.bActive = true;
	HistoricalTradeRoutes.Add(Mediterranean);

	// Via Francigena (Pilgrimage route)
	FHistoricalTradeRoute ViaFrancigena;
	ViaFrancigena.RouteName = TEXT("Via Francigena");
	ViaFrancigena.CitiesOnRoute = {TEXT("Canterbury"), TEXT("Calais"), TEXT("Reims"), TEXT("Besançon"), TEXT("Pavia"), TEXT("Rome")};
	ViaFrancigena.TradeGoods = {EResourceType::Wool, EResourceType::Wine, EResourceType::Cloth};
	ViaFrancigena.Importance = 6;
	ViaFrancigena.bActive = true;
	HistoricalTradeRoutes.Add(ViaFrancigena);

	UE_LOG(LogTemp, Log, TEXT("Loaded %d historical trade routes"), HistoricalTradeRoutes.Num());
}

void UGTWorldMapSubsystem::PrecomputeDistances()
{
	TArray<FString> CityNames;
	HistoricalCities.GetKeys(CityNames);

	int32 TotalPairs = (CityNames.Num() * (CityNames.Num() - 1)) / 2;
	UE_LOG(LogTemp, Log, TEXT("Precomputing distances for %d city pairs..."), TotalPairs);

	for (int32 i = 0; i < CityNames.Num(); ++i)
	{
		for (int32 j = i + 1; j < CityNames.Num(); ++j)
		{
			FString CityA = CityNames[i];
			FString CityB = CityNames[j];

			FHistoricalCityData* DataA = HistoricalCities.Find(CityA);
			FHistoricalCityData* DataB = HistoricalCities.Find(CityB);

			if (!DataA || !DataB) continue;

			FCityDistanceData DistData;
			DistData.CityA = CityA;
			DistData.CityB = CityB;
			DistData.DistanceKm = CalculateDistance(DataA->Coordinates, DataB->Coordinates);

			// Estimate terrain difficulty (simplified - in production, pathfind)
			DistData.TerrainDifficulty = 1.0f; // Placeholder

			// Medieval travel speed: ~30 km/day on roads, ~15 km/day off-road
			float TravelSpeedKmPerDay = 25.0f; // Average
			DistData.TravelTimeDays = FMath::CeilToInt(DistData.DistanceKm / TravelSpeedKmPerDay);

			FString Key = MakeDistanceKey(CityA, CityB);
			DistanceMatrix.Add(Key, DistData);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Precomputed %d distances"), DistanceMatrix.Num());
}

// ========== COORDINATE CONVERSION ==========

FWorldSpaceCoordinate UGTWorldMapSubsystem::ConvertToWorldSpace(FGeographicCoordinate GeoCoord)
{
	// Mercator projection (suitable for mid-latitudes like Europe)
	// X = (lon - lon0) * scale
	// Y = lat * scale (simplified - true Mercator uses log(tan))

	double DeltaLon = GeoCoord.Longitude - MapCenter.Longitude;
	double DeltaLat = GeoCoord.Latitude - MapCenter.Latitude;

	// For small areas, simple linear approximation works
	// (For larger areas, use proper Mercator: Y = log(tan(lat/2 + PI/4)))

	FWorldSpaceCoordinate WorldCoord;
	WorldCoord.X = DeltaLon * ProjectionScale;
	WorldCoord.Y = DeltaLat * ProjectionScale;

	// Z = elevation scaled to UE5 (1m = 100 UE5 units)
	WorldCoord.Z = GeoCoord.Elevation * 100.0f;

	return WorldCoord;
}

FGeographicCoordinate UGTWorldMapSubsystem::ConvertToGeoCoordinate(FWorldSpaceCoordinate WorldCoord)
{
	FGeographicCoordinate GeoCoord;

	// Reverse of ConvertToWorldSpace
	GeoCoord.Longitude = MapCenter.Longitude + (WorldCoord.X / ProjectionScale);
	GeoCoord.Latitude = MapCenter.Latitude + (WorldCoord.Y / ProjectionScale);
	GeoCoord.Elevation = WorldCoord.Z / 100.0f; // UE5 units to meters

	return GeoCoord;
}

FVector UGTWorldMapSubsystem::GetCityPosition(const FString& CityName)
{
	FHistoricalCityData* CityData = HistoricalCities.Find(CityName);
	if (!CityData)
	{
		UE_LOG(LogTemp, Warning, TEXT("City not found: %s"), *CityName);
		return FVector::ZeroVector;
	}

	FWorldSpaceCoordinate WorldCoord = ConvertToWorldSpace(CityData->Coordinates);
	return WorldCoord.ToVector();
}

// ========== TERRAIN QUERIES ==========

ETerrainType UGTWorldMapSubsystem::GetTerrainAtLocation(FGeographicCoordinate GeoCoord)
{
	float Elevation = GetElevationAtLocation(GeoCoord);
	float Slope = 0.0f; // Placeholder - in production, calculate from heightmap

	return DetermineTerrainFromElevation(Elevation, Slope);
}

float UGTWorldMapSubsystem::GetElevationAtLocation(FGeographicCoordinate GeoCoord)
{
	// In production, sample from heightmap data
	// For now, use simple heuristics based on known geography

	// Alps: 45-48°N, 6-14°E
	if (GeoCoord.Latitude >= 45.0 && GeoCoord.Latitude <= 48.0 &&
		GeoCoord.Longitude >= 6.0 && GeoCoord.Longitude <= 14.0)
	{
		return 2000.0f; // High mountains
	}

	// Pyrenees: 42-43°N, 0-3°E
	if (GeoCoord.Latitude >= 42.0 && GeoCoord.Latitude <= 43.0 &&
		GeoCoord.Longitude >= 0.0 && GeoCoord.Longitude <= 3.0)
	{
		return 1500.0f; // Mountains
	}

	// Carpathians: 47-49°N, 22-26°E
	if (GeoCoord.Latitude >= 47.0 && GeoCoord.Latitude <= 49.0 &&
		GeoCoord.Longitude >= 22.0 && GeoCoord.Longitude <= 26.0)
	{
		return 1200.0f; // Mountains
	}

	// Default: plains
	return 100.0f;
}

bool UGTWorldMapSubsystem::HasLineOfSight(FGeographicCoordinate A, FGeographicCoordinate B)
{
	// Sample elevation along path
	int32 SampleCount = 10;
	for (int32 i = 1; i < SampleCount; ++i)
	{
		float T = (float)i / (float)SampleCount;
		FGeographicCoordinate MidPoint;
		MidPoint.Latitude = FMath::Lerp(A.Latitude, B.Latitude, T);
		MidPoint.Longitude = FMath::Lerp(A.Longitude, B.Longitude, T);

		float Elevation = GetElevationAtLocation(MidPoint);
		if (Elevation > 1000.0f) // Mountain blocking
		{
			return false;
		}
	}

	return true;
}

ETerrainType UGTWorldMapSubsystem::DetermineTerrainFromElevation(float Elevation, float Slope)
{
	if (Elevation > 1500.0f)
	{
		return ETerrainType::HighMountains;
	}
	else if (Elevation > 600.0f)
	{
		return ETerrainType::Mountains;
	}
	else if (Elevation > 200.0f || Slope > 0.3f)
	{
		return ETerrainType::Hills;
	}
	else
	{
		return ETerrainType::Plains;
	}
}

// ========== DISTANCE CALCULATIONS ==========

float UGTWorldMapSubsystem::CalculateDistance(FGeographicCoordinate A, FGeographicCoordinate B)
{
	return HaversineDistance(A.Latitude, A.Longitude, B.Latitude, B.Longitude);
}

double UGTWorldMapSubsystem::HaversineDistance(double Lat1, double Lon1, double Lat2, double Lon2)
{
	// Haversine formula for great-circle distance
	double dLat = DegToRad(Lat2 - Lat1);
	double dLon = DegToRad(Lon2 - Lon1);

	double a = FMath::Sin(dLat / 2.0) * FMath::Sin(dLat / 2.0) +
		FMath::Cos(DegToRad(Lat1)) * FMath::Cos(DegToRad(Lat2)) *
		FMath::Sin(dLon / 2.0) * FMath::Sin(dLon / 2.0);

	double c = 2.0 * FMath::Atan2(FMath::Sqrt(a), FMath::Sqrt(1.0 - a));

	return EarthRadiusKm * c;
}

int32 UGTWorldMapSubsystem::CalculateTravelTime(const FString& CityA, const FString& CityB)
{
	FString Key = MakeDistanceKey(CityA, CityB);
	FCityDistanceData* DistData = DistanceMatrix.Find(Key);

	if (DistData)
	{
		return DistData->TravelTimeDays;
	}

	// Fallback: calculate on the fly
	FHistoricalCityData* DataA = HistoricalCities.Find(CityA);
	FHistoricalCityData* DataB = HistoricalCities.Find(CityB);

	if (!DataA || !DataB)
	{
		return 0;
	}

	float DistanceKm = CalculateDistance(DataA->Coordinates, DataB->Coordinates);
	float TravelSpeedKmPerDay = 25.0f;
	return FMath::CeilToInt(DistanceKm / TravelSpeedKmPerDay);
}

float UGTWorldMapSubsystem::GetTerrainDifficulty(ETerrainType Terrain)
{
	switch (Terrain)
	{
	case ETerrainType::Plains:
	case ETerrainType::Farmland:
		return 1.0f; // Easy

	case ETerrainType::Hills:
	case ETerrainType::Forest:
		return 2.0f; // Moderate

	case ETerrainType::Mountains:
	case ETerrainType::Marshland:
		return 3.5f; // Difficult

	case ETerrainType::HighMountains:
		return 5.0f; // Very difficult

	case ETerrainType::Coast:
	case ETerrainType::RiverBank:
		return 0.8f; // Easy (water transport)

	default:
		return 1.0f;
	}
}

// ========== TRADE ROUTE GENERATION ==========

TArray<FString> UGTWorldMapSubsystem::FindTradeRoute(const FString& StartCity, const FString& EndCity)
{
	// Simplified pathfinding - in production, use A* with terrain costs
	TArray<FString> Route;
	Route.Add(StartCity);

	// Check if connected by historical trade route
	for (const FHistoricalTradeRoute& TradeRoute : HistoricalTradeRoutes)
	{
		int32 StartIdx = TradeRoute.CitiesOnRoute.Find(StartCity);
		int32 EndIdx = TradeRoute.CitiesOnRoute.Find(EndCity);

		if (StartIdx != INDEX_NONE && EndIdx != INDEX_NONE)
		{
			// Found route connecting both cities
			int32 MinIdx = FMath::Min(StartIdx, EndIdx);
			int32 MaxIdx = FMath::Max(StartIdx, EndIdx);

			for (int32 i = MinIdx; i <= MaxIdx; ++i)
			{
				if (i != StartIdx) // Don't duplicate start
				{
					Route.Add(TradeRoute.CitiesOnRoute[i]);
				}
			}

			return Route;
		}
	}

	// No historical route - direct connection
	Route.Add(EndCity);
	return Route;
}

bool UGTWorldMapSubsystem::AreConnectedByRiver(const FString& CityA, const FString& CityB)
{
	FString RiverName = GetConnectingRiver(CityA, CityB);
	return !RiverName.IsEmpty();
}

FString UGTWorldMapSubsystem::GetConnectingRiver(const FString& CityA, const FString& CityB)
{
	FHistoricalCityData* DataA = HistoricalCities.Find(CityA);
	FHistoricalCityData* DataB = HistoricalCities.Find(CityB);

	if (!DataA || !DataB)
	{
		return FString();
	}

	// Check if both cities are on the same river
	if (!DataA->RiverName.IsEmpty() && DataA->RiverName == DataB->RiverName)
	{
		return DataA->RiverName;
	}

	return FString();
}

// ========== HISTORICAL DATA QUERIES ==========

FHistoricalCityData* UGTWorldMapSubsystem::GetCityData(const FString& CityName)
{
	return HistoricalCities.Find(CityName);
}

bool UGTWorldMapSubsystem::GetCityDataCopy(const FString& CityName, FHistoricalCityData& OutCityData)
{
	FHistoricalCityData* CityData = HistoricalCities.Find(CityName);
	if (CityData)
	{
		OutCityData = *CityData;
		return true;
	}
	return false;
}

TArray<FString> UGTWorldMapSubsystem::GetCitiesInRegion(const FString& RegionName)
{
	FGeographicRegion* Region = GeographicRegions.Find(RegionName);
	if (Region)
	{
		return Region->MajorCities;
	}

	return TArray<FString>();
}

TArray<FString> UGTWorldMapSubsystem::GetCitiesOnTradeRoute(const FString& RouteName)
{
	for (const FHistoricalTradeRoute& Route : HistoricalTradeRoutes)
	{
		if (Route.RouteName == RouteName)
		{
			return Route.CitiesOnRoute;
		}
	}

	return TArray<FString>();
}

TArray<FString> UGTWorldMapSubsystem::GetNearbyCities(FGeographicCoordinate Location, float RadiusKm)
{
	TArray<FString> NearbyCities;

	for (const TPair<FString, FHistoricalCityData>& Pair : HistoricalCities)
	{
		float Distance = CalculateDistance(Location, Pair.Value.Coordinates);
		if (Distance <= RadiusKm)
		{
			NearbyCities.Add(Pair.Key);
		}
	}

	return NearbyCities;
}

TArray<FString> UGTWorldMapSubsystem::GetCitiesInPoliticalEntity(const FString& EntityName)
{
	TArray<FString> Cities;

	for (const TPair<FString, FHistoricalCityData>& Pair : HistoricalCities)
	{
		if (Pair.Value.PoliticalEntity == EntityName)
		{
			Cities.Add(Pair.Key);
		}
	}

	return Cities;
}

// ========== TERRAIN GENERATION ==========

void UGTWorldMapSubsystem::GenerateTerrainFromHeightmap()
{
	// Placeholder - in production, use UE5 Landscape API
	UE_LOG(LogTemp, Warning, TEXT("GenerateTerrainFromHeightmap not yet implemented"));
}

void UGTWorldMapSubsystem::SpawnCityActors()
{
	// Placeholder - spawn city actors at real coordinates
	UE_LOG(LogTemp, Warning, TEXT("SpawnCityActors not yet implemented"));
}

void UGTWorldMapSubsystem::GenerateRiverMeshes()
{
	// Placeholder - generate spline meshes for rivers
	UE_LOG(LogTemp, Warning, TEXT("GenerateRiverMeshes not yet implemented"));
}

// ========== INTEGRATION WITH ECONOMY ==========

void UGTWorldMapSubsystem::IntegrateWithHierarchicalEconomy()
{
	UGTHierarchicalEconomySubsystem* HierarchyEconomy = GetGameInstance()->GetSubsystem<UGTHierarchicalEconomySubsystem>();
	if (!HierarchyEconomy)
	{
		UE_LOG(LogTemp, Warning, TEXT("HierarchicalEconomySubsystem not found"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Integrating %d historical cities with hierarchical economy..."), HistoricalCities.Num());

	// Helper lambda to convert terrain type to geography type
	auto ConvertTerrainToGeography = [](ETerrainType Terrain) -> EGeographyType
	{
		switch (Terrain)
		{
		case ETerrainType::Plains:
		case ETerrainType::Farmland:
			return EGeographyType::Plains;
		case ETerrainType::Hills:
		case ETerrainType::Valleys:
			return EGeographyType::Hills;
		case ETerrainType::Mountains:
		case ETerrainType::HighMountains:
			return EGeographyType::Mountains;
		case ETerrainType::Forest:
		case ETerrainType::MixedForest:
			return EGeographyType::Forest;
		case ETerrainType::Marshland:
			return EGeographyType::Swamp;
		case ETerrainType::Coast:
			return EGeographyType::Coast;
		case ETerrainType::RiverBank:
			return EGeographyType::River;
		default:
			return EGeographyType::Plains;
		}
	};

	// For each historical city, create corresponding holding
	for (const TPair<FString, FHistoricalCityData>& Pair : HistoricalCities)
	{
		const FHistoricalCityData& CityData = Pair.Value;

		// Create holding with real geography
		FHoldingData HoldingData;
		HoldingData.HoldingName = CityData.CityName;
		HoldingData.HoldingType = CityData.HoldingType;
		HoldingData.Population = CityData.HistoricalPopulation;
		HoldingData.Geography = ConvertTerrainToGeography(CityData.TerrainType);

		// Set production based on terrain and climate
		if (CityData.TerrainType == ETerrainType::Plains || CityData.TerrainType == ETerrainType::Farmland)
		{
			HoldingData.LocalProduction.Add(EResourceType::Grain, 500);
			HoldingData.LocalProduction.Add(EResourceType::Cattle, 100);
		}
		else if (CityData.TerrainType == ETerrainType::Hills)
		{
			HoldingData.LocalProduction.Add(EResourceType::Wine, 200);
			HoldingData.LocalProduction.Add(EResourceType::Wool, 150);
		}
		else if (CityData.TerrainType == ETerrainType::Mountains || CityData.TerrainType == ETerrainType::HighMountains)
		{
			HoldingData.LocalProduction.Add(EResourceType::Iron, 100);
			HoldingData.LocalProduction.Add(EResourceType::Silver, 50);
			HoldingData.LocalProduction.Add(EResourceType::Salt, 80);
		}
		else if (CityData.TerrainType == ETerrainType::Coast)
		{
			HoldingData.LocalProduction.Add(EResourceType::Fish, 300);
			HoldingData.LocalProduction.Add(EResourceType::Salt, 100);
		}
		else if (CityData.TerrainType == ETerrainType::RiverBank)
		{
			HoldingData.LocalProduction.Add(EResourceType::Grain, 400);
			HoldingData.LocalProduction.Add(EResourceType::Fish, 150);
		}
		else if (CityData.TerrainType == ETerrainType::Forest)
		{
			HoldingData.LocalProduction.Add(EResourceType::Timber, 500);
			HoldingData.LocalProduction.Add(EResourceType::Furs, 50);
		}

		// Add climate-specific production
		if (CityData.ClimateZone == EClimateZone::Mediterranean)
		{
			HoldingData.LocalProduction.Add(EResourceType::Wine, 200);
			HoldingData.LocalProduction.Add(EResourceType::Olive_Oil, 150);
		}
		else if (CityData.ClimateZone == EClimateZone::Oceanic)
		{
			HoldingData.LocalProduction.Add(EResourceType::Wool, 200);
			HoldingData.LocalProduction.Add(EResourceType::Grain, 300);
		}
		else if (CityData.ClimateZone == EClimateZone::Nordic)
		{
			HoldingData.LocalProduction.Add(EResourceType::Timber, 400);
			HoldingData.LocalProduction.Add(EResourceType::Furs, 100);
		}

		// Special trade goods for cities on major trade routes
		if (CityData.bOnSilkRoad)
		{
			HoldingData.LocalProduction.Add(EResourceType::Silk, 50);
			HoldingData.LocalProduction.Add(EResourceType::Spices, 30);
		}
		if (CityData.bOnHanseaticRoute)
		{
			HoldingData.LocalProduction.Add(EResourceType::Amber, 40);
			HoldingData.LocalProduction.Add(EResourceType::Herring, 200);
		}

		// Set population needs based on city size
		int32 NeedsMultiplier = HoldingData.Population / 100; // Per 100 people
		HoldingData.PopulationNeeds.Add(EResourceType::Bread, NeedsMultiplier * 2);
		HoldingData.PopulationNeeds.Add(EResourceType::Beer, NeedsMultiplier);
		HoldingData.PopulationNeeds.Add(EResourceType::Cloth, NeedsMultiplier / 2);

		// Add to hierarchical economy
		HierarchyEconomy->AddHolding(HoldingData);
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully integrated %d cities with hierarchical economy"), HistoricalCities.Num());
}

TArray<FCityConnection> UGTWorldMapSubsystem::GeneratePassiveTradeConnections()
{
	TArray<FCityConnection> Connections;

	TArray<FString> CityNames;
	HistoricalCities.GetKeys(CityNames);

	// Connect cities within 200km that have line-of-sight
	const float MaxPassiveTradeDistanceKm = 200.0f;

	for (int32 i = 0; i < CityNames.Num(); ++i)
	{
		for (int32 j = i + 1; j < CityNames.Num(); ++j)
		{
			FHistoricalCityData* DataA = HistoricalCities.Find(CityNames[i]);
			FHistoricalCityData* DataB = HistoricalCities.Find(CityNames[j]);

			if (!DataA || !DataB) continue;

			float Distance = CalculateDistance(DataA->Coordinates, DataB->Coordinates);
			if (Distance <= MaxPassiveTradeDistanceKm)
			{
				// Check if mountains block
				if (HasLineOfSight(DataA->Coordinates, DataB->Coordinates))
				{
					Connections.Add(FCityConnection(CityNames[i], CityNames[j]));
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Generated %d passive trade connections"), Connections.Num());
	return Connections;
}

float UGTWorldMapSubsystem::CalculateTradeEfficiency(const FString& CityA, const FString& CityB)
{
	FHistoricalCityData* DataA = HistoricalCities.Find(CityA);
	FHistoricalCityData* DataB = HistoricalCities.Find(CityB);

	if (!DataA || !DataB)
	{
		return 0.5f;
	}

	float BaseEfficiency = 1.0f;

	// River bonus
	if (AreConnectedByRiver(CityA, CityB))
	{
		BaseEfficiency *= 1.5f;
	}

	// Terrain penalty
	float DistanceKm = CalculateDistance(DataA->Coordinates, DataB->Coordinates);
	if (DistanceKm > 500.0f)
	{
		BaseEfficiency *= 0.7f; // Long distance penalty
	}

	// Mountains penalty
	if (!HasLineOfSight(DataA->Coordinates, DataB->Coordinates))
	{
		BaseEfficiency *= 0.5f; // Mountain crossing
	}

	return FMath::Clamp(BaseEfficiency, 0.1f, 2.0f);
}

// ========== DEBUGGING ==========

void UGTWorldMapSubsystem::PrintCityData(const FString& CityName)
{
	FHistoricalCityData* CityData = HistoricalCities.Find(CityName);
	if (!CityData)
	{
		UE_LOG(LogTemp, Warning, TEXT("City not found: %s"), *CityName);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("=== %s ==="), *CityData->CityName);
	UE_LOG(LogTemp, Log, TEXT("Coordinates: (%.2f, %.2f)"), CityData->Coordinates.Latitude, CityData->Coordinates.Longitude);
	UE_LOG(LogTemp, Log, TEXT("Political Entity: %s"), *CityData->PoliticalEntity);
	UE_LOG(LogTemp, Log, TEXT("Population: %d"), CityData->HistoricalPopulation);
	UE_LOG(LogTemp, Log, TEXT("Trade Importance: %d/10"), CityData->TradeImportance);

	FVector WorldPos = GetCityPosition(CityName);
	UE_LOG(LogTemp, Log, TEXT("UE5 Position: (%.0f, %.0f, %.0f)"), WorldPos.X, WorldPos.Y, WorldPos.Z);
}

void UGTWorldMapSubsystem::PrintTradeRoute(const FString& RouteName)
{
	for (const FHistoricalTradeRoute& Route : HistoricalTradeRoutes)
	{
		if (Route.RouteName == RouteName)
		{
			UE_LOG(LogTemp, Log, TEXT("=== %s ==="), *Route.RouteName);
			UE_LOG(LogTemp, Log, TEXT("Importance: %d/10"), Route.Importance);
			UE_LOG(LogTemp, Log, TEXT("Cities: %s"), *FString::Join(Route.CitiesOnRoute, TEXT(" -> ")));
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Trade route not found: %s"), *RouteName);
}

void UGTWorldMapSubsystem::DebugDrawMap()
{
	// Placeholder - use DrawDebugLine to visualize map
	UE_LOG(LogTemp, Warning, TEXT("DebugDrawMap not yet implemented"));
}

// ========== HELPER FUNCTIONS ==========

FString UGTWorldMapSubsystem::MakeDistanceKey(const FString& CityA, const FString& CityB)
{
	// Alphabetical order to ensure consistent keys
	if (CityA < CityB)
	{
		return CityA + TEXT("_") + CityB;
	}
	else
	{
		return CityB + TEXT("_") + CityA;
	}
}

float UGTWorldMapSubsystem::SampleHeightmap(FGeographicCoordinate Coord)
{
	// Placeholder - in production, sample from heightmap texture
	return GetElevationAtLocation(Coord);
}

float UGTWorldMapSubsystem::InterpolateElevation(FGeographicCoordinate Coord)
{
	// Placeholder - bilinear interpolation from heightmap cells
	return GetElevationAtLocation(Coord);
}

bool UGTWorldMapSubsystem::IsWater(FGeographicCoordinate Coord)
{
	// Placeholder - check against water bodies shapefile
	// For now, assume land everywhere
	return false;
}
