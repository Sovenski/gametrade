# World Map Integration Guide

## Overview

The World Map Integration system brings **historically accurate medieval European geography** into the game using royalty-free data sources. This document explains:

- **Data sources** (all public domain/freely available)
- **Coordinate system** (lat/lon → UE5 world space)
- **Terrain generation** from real elevation data
- **Integration** with economy and trade systems

---

## 📊 Data Sources (All Royalty-Free)

### Primary Sources

| Source | Data Type | License | Coverage | URL |
|--------|-----------|---------|----------|-----|
| **Harvard MAPS** | Medieval GIS shapefiles | Public Domain | Europe 0-1500 CE | [sohp.fas.harvard.edu/darmc](https://sohp.fas.harvard.edu/digital-atlas-roman-and-medieval-civilizations) |
| **Natural Earth Data** | Topography, elevation, rivers | Public Domain | Global | [naturalearthdata.com](https://www.naturalearthdata.com/) |
| **OpenHistoricalMap** | Medieval cities and roads | ODbL | Europe 1350-1650 | [openhistoricalmap.org](https://www.openhistoricalmap.org/) |
| **Research Dataset** | 173 medieval cities with coordinates | CC-BY | Western Europe ca. 1300 | [NCBI PMC5051806](https://pmc.ncbi.nlm.nih.gov/articles/PMC5051806/) |
| **Viabundus** | Northern Europe street map | Free | 1350-1650 | Uni Göttingen |

### What We Get

- ✅ **Real coordinates** for 95+ medieval cities
- ✅ **Elevation data** (Natural Earth SRTM Plus)
- ✅ **Major rivers** (Rhine, Danube, Seine, Thames, etc.)
- ✅ **Historical trade routes** (Silk Road, Hanseatic League, Mediterranean)
- ✅ **Terrain types** derived from elevation
- ✅ **Political boundaries** (HRE, France, England, etc.)

---

## 🌍 Coordinate System

### Input: Geographic Coordinates (WGS84)

Standard latitude/longitude used by GPS and GIS:

```cpp
struct FGeographicCoordinate
{
    double Latitude;  // -90 to +90 (North positive)
    double Longitude; // -180 to +180 (East positive)
    float Elevation;  // Meters above sea level
};
```

**Examples:**
- London: `(51.5074, -0.1278, 15m)`
- Paris: `(48.8566, 2.3522, 35m)`
- Venice: `(45.4408, 12.3155, 2m)`

### Output: UE5 World Space

Converted to Unreal Engine coordinates (in centimeters):

```cpp
struct FWorldSpaceCoordinate
{
    double X; // UE5 X coordinate (cm)
    double Y; // UE5 Y coordinate (cm)
    float Z;  // Elevation (cm)
};
```

### Projection Parameters

**Map Center:** `(50°N, 10°E)` - Central Europe (approximate HRE center)

**Scale:** `1 degree latitude = 111 km = 11,100,000 UE5 units (cm)`

**Projection Type:** Mercator (simplified linear for small areas)

### Conversion Formulas

**Lat/Lon → UE5:**
```cpp
WorldX = (Longitude - CenterLon) * ProjectionScale
WorldY = (Latitude - CenterLat) * ProjectionScale
WorldZ = Elevation * 100 (meters to cm)
```

**UE5 → Lat/Lon:**
```cpp
Longitude = CenterLon + (WorldX / ProjectionScale)
Latitude = CenterLat + (WorldY / ProjectionScale)
Elevation = WorldZ / 100 (cm to meters)
```

### Example Conversion

**Venice** `(45.4408°N, 12.3155°E, 2m)`:

```
CenterLon = 10.0°
CenterLat = 50.0°
ProjectionScale = 11,100,000 cm/degree

WorldX = (12.3155 - 10.0) * 11,100,000 = 25,702,050 cm (257 km east)
WorldY = (45.4408 - 50.0) * 11,100,000 = -50,607,280 cm (506 km south)
WorldZ = 2 * 100 = 200 cm (2 meters)

UE5 Position: (25,702,050, -50,607,280, 200)
```

---

## 🗺️ Terrain Types

Terrain determined from **elevation** and **slope** (derived from Natural Earth SRTM Plus data):

| Terrain | Elevation Range | Example Regions | Trade Difficulty |
|---------|----------------|-----------------|------------------|
| **Plains** | 0-200m | Po Valley, Flanders | 1.0x (Easy) |
| **Farmland** | 0-200m (cultivated) | Île-de-France, East Anglia | 1.0x |
| **Hills** | 200-600m | Tuscany, Champagne | 2.0x (Moderate) |
| **Mountains** | 600-1500m | Vosges, Carpathians | 3.5x (Difficult) |
| **High Mountains** | 1500m+ | Alps, Pyrenees | 5.0x (Very difficult) |
| **River Bank** | Along major rivers | Rhine, Danube, Seine | 0.8x (Easy - water transport) |
| **Coast** | Near sea | Venice, Genoa, Hamburg | 0.8x (Easy - sea trade) |
| **Forest** | Dense woodland | Black Forest, Ardennes | 2.0x |

### Climate Zones

| Climate | Region | Affects | Growing Season |
|---------|--------|---------|----------------|
| **Mediterranean** | Southern Europe | Wine, olive oil, citrus | Year-round |
| **Oceanic** | Western Europe | Wool, grain, dairy | April-October |
| **Continental** | Central Europe | Grain, timber, beer | May-September |
| **Alpine** | Mountains | Mining, livestock | June-August |
| **Nordic** | Scandinavia | Timber, furs, fish | June-September |

---

## 🏙️ Historical Cities Database

**95 cities** with real coordinates in `Content/Data/HistoricalCities.csv`

### CSV Format

```csv
CityName,MedievalName,Latitude,Longitude,Elevation,Population,PoliticalEntity,TradeImportance,OnSilkRoad,OnHanseaticRoute,OnMediterraneanRoute,RiverName,TerrainType,ClimateZone
London,London,51.5074,-0.1278,15,80000,Kingdom of England,9,0,0,0,Thames,Plains,Oceanic
Paris,Paris,48.8566,2.3522,35,200000,Kingdom of France,10,0,0,0,Seine,Plains,Oceanic
Venezia,Venice,45.4408,12.3155,2,100000,Republic of Venice,10,1,0,1,,,Mediterranean
```

### Fields Explained

- **Population:** Historical estimate ca. 1347-1350 (pre-Black Death)
- **TradeImportance:** 1-10 scale (10 = Venice, Paris; 1 = small market town)
- **OnSilkRoad/Hanseatic/Mediterranean:** Boolean flags for historical trade routes
- **RiverName:** Major river if city is on one (affects trade)
- **TerrainType/ClimateZone:** Affects production and consumption

### Top 10 Cities by Population (ca. 1347)

1. **Paris** - 200,000 (largest in Europe)
2. **Venice** - 100,000 (richest per capita)
3. **Milan** - 100,000 (manufacturing hub)
4. **London** - 80,000 (wool trade center)
5. **Genoa** - 80,000 (maritime power)
6. **Florence** - 60,000 (banking, textiles)
7. **Ghent** - 50,000 (Flemish cloth)
8. **Naples** - 50,000 (Mediterranean trade)
9. **Granada** - 50,000 (Moorish Spain)
10. **Bruges** - 45,000 (Hanseatic trade)

---

## 🚢 Historical Trade Routes

### Silk Road (Eastern Terminus)

**Cities:** Constantinople → Venice → Genoa → Lyon
**Goods:** Silk, spices, dyes, precious stones
**Importance:** 10/10 (most valuable)
**Active:** Yes (until Ottoman expansion)

### Hanseatic League (Northern Europe)

**Cities:** Lübeck ↔ Hamburg ↔ Bremen ↔ Danzig ↔ Riga ↔ Novgorod
**Goods:** Timber, fish, grain, salt, wool, amber
**Importance:** 9/10 (controls Baltic)
**Active:** Yes (peak 1250-1450)

### Mediterranean Trade

**Cities:** Venice ↔ Genoa ↔ Barcelona ↔ Marseille ↔ Naples
**Goods:** Wine, olive oil, silk, spices, glass
**Importance:** 8/10 (wealthy but competitive)
**Active:** Yes

### Via Francigena (Pilgrimage Route)

**Cities:** Canterbury → Calais → Reims → Besançon → Pavia → Rome
**Goods:** Wool, wine, cloth, religious items
**Importance:** 6/10 (cultural/religious)
**Active:** Yes

---

## 🏞️ Major European Rivers

Rivers provide **trade bonuses** and **faster travel** (navigable rivers = water transport).

| River | Length (km) | Cities On River | Navigable? | Trade Bonus |
|-------|-------------|-----------------|------------|-------------|
| **Rhine** | 1,230 | Köln, Strasbourg, Basel, Rotterdam | ✅ Yes | 2.0x |
| **Danube** | 2,860 | Wien, Buda, Regensburg | ✅ Yes | 1.8x |
| **Seine** | 777 | Paris, Rouen | ✅ Yes | 1.5x |
| **Thames** | 346 | London | ✅ Yes | 1.5x |
| **Po** | 652 | Ferrara, Mantua | ✅ Yes | 1.5x |
| **Rhone** | 812 | Lyon, Avignon | ✅ Yes | 1.5x |
| **Elbe** | 1,094 | Hamburg, Dresden, Magdeburg | ✅ Yes | 1.5x |
| **Loire** | 1,012 | Orléans, Tours, Nantes | Partially | 1.3x |
| **Tiber** | 406 | Rome | ❌ No | 1.2x |

### River Trade Mechanics

- **Connected cities:** Trade efficiency +50-100%
- **Travel speed:** 2x faster downstream, 1.5x upstream vs. land
- **Transport cost:** 60% cheaper than overland
- **Capacity:** Rivers handle bulk goods (grain, timber, salt)

---

## 📏 Distance Calculations

### Haversine Formula (Great-Circle Distance)

Calculates shortest distance between two points on Earth's surface:

```cpp
double HaversineDistance(double Lat1, double Lon1, double Lat2, double Lon2)
{
    const double EarthRadiusKm = 6371.0;

    double dLat = DegToRad(Lat2 - Lat1);
    double dLon = DegToRad(Lon2 - Lon1);

    double a = sin(dLat/2) * sin(dLat/2) +
               cos(DegToRad(Lat1)) * cos(DegToRad(Lat2)) *
               sin(dLon/2) * sin(dLon/2);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return EarthRadiusKm * c;
}
```

### Travel Time Calculations

**Medieval travel speeds:**

| Mode | Speed (km/day) | Conditions |
|------|----------------|------------|
| **Road (horse)** | 30-40 km | Good roads, plains |
| **Road (cart)** | 15-25 km | Loaded wagon, plains |
| **Off-road** | 10-20 km | Paths, hills |
| **Mountains** | 5-15 km | Alpine passes |
| **River (downstream)** | 40-60 km | Navigable river |
| **River (upstream)** | 10-20 km | Towed by oxen/horses |
| **Sea (sailing)** | 80-150 km | Good weather |

**Formula:**
```cpp
TravelTimeDays = Distance / (BaseSpeed * TerrainMultiplier)
```

**Examples:**
- Paris → Lyon (400 km, road): `400 / 25 = 16 days`
- Köln → Rotterdam (200 km, Rhine): `200 / 50 = 4 days`
- München → Venice (400 km, Alps): `400 / 10 = 40 days`

---

## 🔧 System API

### Initialization

```cpp
// In GameMode::BeginPlay()
UGTWorldMapSubsystem* WorldMap = GetGameInstance()->GetSubsystem<UGTWorldMapSubsystem>();
WorldMap->InitializeWorldMap(); // Loads cities, rivers, trade routes
```

### Coordinate Conversion

```cpp
// Get UE5 position for a city
FVector VenicePos = WorldMap->GetCityPosition(TEXT("Venezia"));

// Convert arbitrary lat/lon to UE5
FGeographicCoordinate MilanCoords(45.4642, 9.1900, 122.0f);
FWorldSpaceCoordinate MilanWorld = WorldMap->ConvertToWorldSpace(MilanCoords);
FVector MilanPos = MilanWorld.ToVector();

// Convert UE5 position back to lat/lon
FGeographicCoordinate GeoCoords = WorldMap->ConvertToGeoCoordinate(MilanWorld);
```

### Distance and Travel

```cpp
// Calculate distance between cities
float DistanceKm = WorldMap->CalculateDistance(ParisCoords, LondonCoords); // ~344 km

// Get travel time
int32 TravelDays = WorldMap->CalculateTravelTime(TEXT("Paris"), TEXT("London")); // ~14 days

// Check if connected by river
bool IsRiverRoute = WorldMap->AreConnectedByRiver(TEXT("Köln"), TEXT("Rotterdam")); // true
FString RiverName = WorldMap->GetConnectingRiver(TEXT("Köln"), TEXT("Rotterdam")); // "Rhine"
```

### Terrain Queries

```cpp
// Get terrain at location
FGeographicCoordinate AlpsCoord(47.0, 11.0); // Innsbruck area
ETerrainType Terrain = WorldMap->GetTerrainAtLocation(AlpsCoord); // Mountains

// Get elevation
float Elevation = WorldMap->GetElevationAtLocation(AlpsCoord); // ~2000m

// Check line-of-sight (mountains blocking?)
bool CanSee = WorldMap->HasLineOfSight(MünchenCoord, VeniceCoord); // false (Alps block)

// Get terrain difficulty for trade
float Difficulty = WorldMap->GetTerrainDifficulty(ETerrainType::HighMountains); // 5.0x
```

### Trade Routes

```cpp
// Find optimal trade route
TArray<FString> Route = WorldMap->FindTradeRoute(TEXT("London"), TEXT("Venice"));
// Returns: ["London", "Calais", "Reims", "Lyon", "Milan", "Venice"]

// Get cities on historical route
TArray<FString> HanseaticCities = WorldMap->GetCitiesOnTradeRoute(TEXT("Hanseatic League"));
// Returns: ["Lübeck", "Hamburg", "Bremen", "Danzig", "Riga", "Novgorod"]

// Calculate trade efficiency (accounts for terrain, rivers, distance)
float Efficiency = WorldMap->CalculateTradeEfficiency(TEXT("Köln"), TEXT("Rotterdam")); // 1.5 (river bonus)
```

### Geographic Queries

```cpp
// Get all cities within radius
TArray<FString> NearbyToMunich = WorldMap->GetNearbyCities(MünchenCoord, 100.0f); // 100km radius
// Returns: ["Augsburg", "Regensburg", "Salzburg", "Innsbruck"]

// Get cities in political entity
TArray<FString> HRECities = WorldMap->GetCitiesInPoliticalEntity(TEXT("Holy Roman Empire"));
// Returns: All HRE cities

// Get city data
FHistoricalCityData* Venice = WorldMap->GetCityData(TEXT("Venezia"));
UE_LOG(LogTemp, Log, TEXT("Venice population: %d"), Venice->HistoricalPopulation); // 100,000
```

---

## 🏗️ Integration with Economy Systems

### Hierarchical Economy Integration

The World Map automatically integrates with the Hierarchical Economy Subsystem:

```cpp
void UGTWorldMapSubsystem::IntegrateWithHierarchicalEconomy()
{
    UGTHierarchicalEconomySubsystem* HierarchyEconomy = GetGameInstance()->GetSubsystem<UGTHierarchicalEconomySubsystem>();

    // For each historical city, create corresponding holding with real geography
    for (const TPair<FString, FHistoricalCityData>& Pair : HistoricalCities)
    {
        FHoldingData Holding;
        Holding.HoldingName = CityData.CityName;
        Holding.HoldingType = CityData.HoldingType;
        Holding.Population = CityData.HistoricalPopulation;
        Holding.Geography = CityData.TerrainType; // Real terrain

        // Set production based on terrain and climate
        if (CityData.ClimateZone == Mediterranean)
        {
            Holding.LocalProduction.Add(Wine, 200);
            Holding.LocalProduction.Add(OliveOil, 100);
        }
        else if (CityData.TerrainType == Plains)
        {
            Holding.LocalProduction.Add(Grain, 500);
        }

        // Add to economy
        HierarchyEconomy->AddHolding(Holding);
    }
}
```

### Passive Trade Network Generation

Generate passive trade connections based on **real geography**:

```cpp
TArray<TPair<FString, FString>> Connections = WorldMap->GeneratePassiveTradeConnections();

// Algorithm:
// 1. Connect cities within 200km
// 2. Check line-of-sight (no mountains blocking)
// 3. Apply river bonuses
// 4. Apply terrain penalties

for (const auto& Connection : Connections)
{
    FString CityA = Connection.Key;
    FString CityB = Connection.Value;

    float Efficiency = WorldMap->CalculateTradeEfficiency(CityA, CityB);

    // Create passive trade route in economy system
    HierarchyEconomy->CreatePassiveTradeConnection(CityA, CityB, Efficiency);
}
```

### Production Based on Geography

Cities produce goods based on **real terrain and climate**:

| Terrain | Primary Production | Secondary |
|---------|-------------------|-----------|
| **Plains** | Grain, vegetables | Livestock, dairy |
| **Hills** | Wine, livestock | Timber, fruit |
| **Mountains** | Iron, silver, salt | Copper, lead |
| **Coast** | Fish, salt | Ships, amber |
| **River Bank** | Grain, mills | Trade services |
| **Forest** | Timber, hunting | Pitch, charcoal |

| Climate | Production | Consumption |
|---------|-----------|-------------|
| **Mediterranean** | Wine, olive oil, citrus | Grain, timber |
| **Oceanic** | Wool, dairy, grain | Wine, luxury goods |
| **Continental** | Grain, beer, livestock | Wine, fish, salt |
| **Alpine** | Mining, cheese | All goods (harsh) |
| **Nordic** | Timber, furs, fish | Grain, wine, luxury |

---

## 🎮 Gameplay Impact

### Trade Decisions Driven by Geography

**Player choices:**

1. **Route selection:**
   - Rhine route: Expensive (tolls) but fast (river)
   - Alpine passes: Cheap but slow and dangerous
   - Sea route: Fast but weather-dependent

2. **City placement:**
   - Build warehouses at river junctions (Köln, Lyon)
   - Invest in coastal ports (Venice, Genoa, Hamburg)
   - Control mountain passes for toll revenue

3. **Seasonal planning:**
   - Alpine passes closed in winter
   - Baltic Sea frozen (Jan-March)
   - Mediterranean sailing best in summer

### Historical Events

**Geography-driven events:**

- **Black Death** (1347-1353): Spreads along trade routes from Genoa
- **Hundred Years' War** (1337-1453): Disrupts Paris-London-Bordeaux trade
- **Ottoman expansion** (1400+): Blocks eastern Silk Road terminus
- **Hanseatic Wars**: Control of Baltic trade cities

### AI Merchant Behavior

AI merchants make decisions based on **real geography**:

```cpp
// AI evaluates trade routes
for (FString TargetCity : AvailableCities)
{
    float Distance = WorldMap->CalculateDistance(CurrentCity, TargetCity);
    float TravelTime = WorldMap->CalculateTravelTime(CurrentCity, TargetCity);
    float Efficiency = WorldMap->CalculateTradeEfficiency(CurrentCity, TargetCity);

    // Score = (Potential Profit) / (Travel Time * Risk)
    float RiskFactor = 1.0f;
    if (!WorldMap->HasLineOfSight(CurrentCity, TargetCity))
    {
        RiskFactor *= 1.5f; // Mountain bandits
    }

    float Score = (PriceDifference * Efficiency) / (TravelTime * RiskFactor);

    // AI prefers short, efficient routes (rivers, roads)
    if (Score > BestScore)
    {
        BestRoute = TargetCity;
        BestScore = Score;
    }
}
```

---

## 🛠️ Extending the System

### Adding More Cities

1. Find real coordinates (use OpenStreetMap, Wikipedia, etc.)
2. Add row to `HistoricalCities.csv`:
   ```csv
   Ragusa,Dubrovnik,42.6507,18.0944,5,15000,Republic of Ragusa,7,1,0,1,,Coast,Mediterranean
   ```
3. Reload in editor (or call `WorldMap->LoadHistoricalCities()`)

### Adding Custom Rivers

```cpp
// In LoadMajorRivers()
FMajorRiver Oder;
Oder.RiverName = TEXT("Oder");
Oder.bNavigable = true;
Oder.TradeBonusMultiplier = 1.4f;
Oder.RiverPath.Add(FGeographicCoordinate(50.8, 17.5)); // Source
Oder.RiverPath.Add(FGeographicCoordinate(51.1, 17.0)); // Breslau
Oder.RiverPath.Add(FGeographicCoordinate(53.4, 14.6)); // Stettin
MajorRivers.Add(Oder);
```

### Custom Trade Routes

```cpp
FHistoricalTradeRoute AmberRoad;
AmberRoad.RouteName = TEXT("Amber Road");
AmberRoad.CitiesOnRoute = {TEXT("Danzig"), TEXT("Kraków"), TEXT("Buda"), TEXT("Venice")};
AmberRoad.TradeGoods = {EResourceType::Amber, EResourceType::Furs, EResourceType::Timber};
AmberRoad.Importance = 6;
AmberRoad.bActive = true;
HistoricalTradeRoutes.Add(AmberRoad);
```

### Heightmap Integration (Advanced)

For full terrain generation, integrate Natural Earth SRTM data:

1. Download SRTM Plus heightmap (GeoTIFF format)
2. Convert to UE5 landscape heightmap (16-bit PNG)
3. Load into `HeightmapData` array
4. Use `SampleHeightmap()` for real elevation queries

```cpp
// Pseudocode for heightmap loading
FHeightmapCell Cell;
Cell.MinCoord = FGeographicCoordinate(45.0, 10.0);
Cell.MaxCoord = FGeographicCoordinate(45.1, 10.1); // 0.1 degree cell
Cell.Elevation = SampleGeoTIFF(Cell.MinCoord, Cell.MaxCoord);
Cell.TerrainType = DetermineTerrainFromElevation(Cell.Elevation, Cell.Slope);
HeightmapData.Add(Cell);
```

---

## 📚 References

### Academic Sources

- **Population-Area Relationship for Medieval European Cities** (2016)
  Rozenfeld et al., PLOS ONE
  https://pmc.ncbi.nlm.nih.gov/articles/PMC5051806/

- **Digital Atlas of Roman and Medieval Civilizations (DARMC)**
  Harvard University
  https://darmc.harvard.edu/

### Geographic Data

- **Natural Earth Data**
  Public domain map data at 1:10m, 1:50m, 1:110m scales
  https://www.naturalearthdata.com/

- **OpenHistoricalMap**
  Collaborative historical geography project
  https://www.openhistoricalmap.org/

- **Viabundus**
  Late medieval and early modern northern Europe (1350-1650)
  University of Göttingen

### Medieval Trade

- **Hanseatic League** - *The Hanse, 1200-1650* by Philippe Dollinger
- **Silk Road** - *The Silk Roads* by Peter Frankopan
- **Medieval Travel** - *Travel in the Middle Ages* by Norbert Ohler

---

## 🎯 Summary

The World Map Integration system provides:

✅ **95 historically accurate cities** with real coordinates
✅ **Royalty-free data** from Harvard, Natural Earth, OpenHistoricalMap
✅ **Coordinate conversion** (lat/lon ↔ UE5 world space)
✅ **Real terrain** (plains, hills, mountains, rivers, coasts)
✅ **Travel calculations** with medieval speeds and terrain penalties
✅ **Major rivers** with trade bonuses (Rhine, Danube, Seine, etc.)
✅ **Historical trade routes** (Silk Road, Hanseatic, Mediterranean)
✅ **Climate zones** affecting production and consumption
✅ **Integration** with hierarchical economy and market systems
✅ **Extensible** design for adding more cities, rivers, routes

**Result:** A medieval European world that feels **authentic**, with geography that **matters** for trade, travel, and strategy decisions.

---

## Next Steps

1. **Compile the project** to test coordinate conversion
2. **Visualize the map** in UE5 editor (debug draw cities)
3. **Generate terrain** from heightmap data (optional)
4. **Integrate with economy** - passive trade based on geography
5. **Test pathfinding** - routes avoid mountains, use rivers
6. **Add seasonal effects** - winter closes Alpine passes
7. **Historical events** - Black Death spreads along trade routes

Enjoy your historically accurate medieval trading empire! 🏰⚔️
