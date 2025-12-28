# DataTable Setup Guide

This guide explains how to populate the game's DataTables with initial content.

## Settlement DataTable

### Creating the DataTable

1. **In Unreal Editor**:
   - Content Browser → Right-click in `Content/Data/`
   - Miscellaneous → Data Table
   - Choose Row Structure: `FSettlementData`
   - Name it: `DT_Settlements`

2. **Open the DataTable**:
   - Double-click `DT_Settlements`
   - You'll see an empty table

### Adding Settlements

Click "Add Row" for each settlement. Here are starter examples:

#### Example 1: München (Major City)

```
Row Name: Muenchen
Settlement Name: München
Settlement Type: City
Territory Name: Bavaria
Territory Type: HRE_Duchy
Population: 15000
Map Location: X=550, Y=480
Local Production:
  - Grain
  - Beer
  - Leather
Local Needs:
  - Wine
  - Salt
  - Iron
  - Cloth
Base Tariff: 0.03 (3%)
Connected Settlements:
  - Augsburg
  - Regensburg
  - Salzburg
  - Innsbruck
```

#### Example 2: Oakwood (Village)

```
Row Name: Oakwood
Settlement Name: Oakwood
Settlement Type: Village
Territory Name: Black Forest
Territory Type: HRE_Duchy
Population: 450
Map Location: X=480, Y=490
Local Production:
  - Timber
  - Game
  - Charcoal
Local Needs:
  - Salt
  - Tools
  - Grain
Base Tariff: 0.02 (2%)
Connected Settlements:
  - Freiburg
```

#### Example 3: Paris (Capital City)

```
Row Name: Paris
Settlement Name: Paris
Settlement Type: CapitalCity
Territory Name: Île-de-France
Territory Type: France_Royal
Population: 200000
Map Location: X=240, Y=485
Local Production:
  - Bread
  - Wine
  - Garments
  - Tools
  - Weapons
Local Needs:
  - Grain (massive)
  - Meat
  - Spices
  - Silk
  - Timber
Base Tariff: 0.01 (1% - capital advantage)
Connected Settlements:
  - Rouen
  - Orléans
  - Reims
  - Amiens
  - Lyon
```

#### Example 4: Venice (Merchant Republic)

```
Row Name: Venice
Settlement Name: Venezia
Settlement Type: MajorCity
Territory Name: Republic of Venice
Territory Type: Italy_Republic
Population: 100000
Map Location: X=540, Y=550
Local Production:
  - Glass (Venetian Glass - monopoly!)
  - Silk (imports from East)
  - Spices (imports from East)
  - Ships
Local Needs:
  - Grain (imports from Sicily)
  - Timber (shipbuilding)
  - Iron
Base Tariff: 0.01 (trade-focused republic)
Connected Settlements:
  - Milano
  - Genova
  - Firenze
  - Split (Dalmatian coast)
  - Constantinople (sea route)
```

### Starter Settlement List (20 Essential Cities)

Copy these to get started quickly:

**Holy Roman Empire**:
1. München (Bavaria)
2. Nürnberg (Bavaria)
3. Augsburg (Bavaria)
4. Frankfurt (Free City)
5. Köln (Free City)
6. Lübeck (Hanseatic capital)
7. Hamburg (Hanseatic port)
8. Wien (Austria)
9. Praha (Bohemia)

**France**:
10. Paris (capital)
11. Lyon (silk & banking)
12. Bordeaux (wine)
13. Marseille (Mediterranean port)

**Italy**:
14. Venezia (trade empire)
15. Genova (banking)
16. Firenze (banking & wool)
17. Milano (silk & armor)

**Others**:
18. London (England - wool)
19. Bruges (Flanders - cloth)
20. Barcelona (Aragon - Mediterranean)

## Map Coordinates Reference

### Coordinate System
- **Origin (0,0)**: Atlantic Ocean (west of Iberia)
- **X-axis**: West (0) to East (1000)
- **Y-axis**: North (300) to South (700)

### Major Cities Approximate Coordinates

```
London:      X=180,  Y=430
Paris:       X=240,  Y=485
Bordeaux:    X=200,  Y=550
Lyon:        X=280,  Y=540
Marseille:   X=300,  Y=580
Barcelona:   X=240,  Y=610
Madrid:      X=180,  Y=610

Bruges:      X=270,  Y=430
Amsterdam:   X=285,  Y=410
Köln:        X=310,  Y=450
Frankfurt:   X=350,  Y=480
Nürnberg:    X=400,  Y=490
München:     X=450,  Y=495
Augsburg:    X=420,  Y=500
Wien:        X=510,  Y=495
Zürich:      X=350,  Y=515
Innsbruck:   X=430,  Y=520

Lübeck:      X=400,  Y=380
Hamburg:     X=390,  Y=390
Berlin:      X=470,  Y=410
Leipzig:     X=440,  Y=430
Dresden:     X=470,  Y=440
Praha:       X=470,  Y=480
Kraków:      X=570,  Y=480

Venezia:     X=440,  Y=545
Milano:      X=380,  Y=545
Genova:      X=360,  Y=560
Firenze:     X=400,  Y=570
Roma:        X=440,  Y=600

København:   X=440,  Y=360
Stockholm:   X=530,  Y=320
```

## Resource Distribution Guide

### England
**Produces**: Wool (high quality), Tin, Lead, Grain
**Needs**: Wine, Spices, Cloth (ironic!)
**Cities**: London, York, Bristol, Norwich

### Flanders (Low Countries)
**Produces**: Cloth (world's best!), Linen, Beer
**Needs**: Wool (from England), Grain, Wine
**Cities**: Bruges, Ghent, Antwerp, Ypres

### France
**Produces**: Wine, Grain (surplus), Salt
**Needs**: Wool, Cloth, Spices
**Cities**: Paris, Lyon, Bordeaux, Marseille, Rouen

### Holy Roman Empire (South)
**Produces**: Iron, Salt, Beer, Silver
**Needs**: Wine, Cloth, Spices
**Cities**: München, Augsburg, Nürnberg, Wien

### HRE (North - Hanseatic)
**Produces**: Timber, Fish (herring!), Grain
**Needs**: Salt, Beer, Cloth
**Cities**: Lübeck, Hamburg, Bremen, Danzig

### Italy
**Produces**: Silk, Glass (Venice), Armor (Milan), Wine
**Needs**: Grain, Timber, Wool
**Cities**: Venice, Genoa, Florence, Milan, Rome

### Iberia
**Produces**: Wool (Merino), Wine, Olive Oil, Weapons (Toledo)
**Needs**: Grain, Timber, Cloth
**Cities**: Barcelona, Sevilla, Toledo, Lisboa

### Eastern Europe
**Produces**: Grain (massive surplus), Timber, Furs, Salt
**Needs**: Wine, Cloth, Metals
**Cities**: Kraków, Gdańsk, Buda, Vilnius

### Scandinavia
**Produces**: Timber, Iron (Sweden!), Fish, Furs
**Needs**: Grain, Salt, Beer
**Cities**: Stockholm, København, Bergen, Oslo

## Loading DataTable in Game

### In Blueprint (BP_GTGameMode)

1. **Add Variable**:
   - Name: `SettlementDataTable`
   - Type: `DataTable` (Object Reference)
   - Default Value: Select `DT_Settlements`

2. **On BeginPlay**:
   ```
   Get Game Instance
   → Get Subsystem (GTEconomySubsystem)
   → Load Settlements From Data Table
      ├─ Settlement Table: SettlementDataTable
   → Initialize Economy
   ```

### In C++ (Already Implemented)

The `GTEconomySubsystem::LoadSettlementsFromDataTable()` function handles loading.

## Testing Your DataTable

### Console Commands

Open console (~ key) and type:

```
# Check if settlements loaded
ShowSettlements

# Check specific settlement
ShowSettlement Munich

# Check prices
ShowPrices Munich Grain

# Advance economy
ProcessEconomyTurn
```

### Debug Logging

Check Output Log for:
```
LogTemp: Loaded settlement: München
LogTemp: Loaded settlement: Paris
LogTemp: Loaded 20 settlements
LogTemp: Economy initialized with prices
```

## Common Issues

### Issue: "No settlements loaded"
**Fix**: Make sure DataTable row structure is `FSettlementData`

### Issue: "Price is 0.0"
**Fix**: Resource not in `BasePrices` map. Add it to `GTEconomySubsystem::Initialize()`

### Issue: "Settlement not found"
**Fix**: Row name doesn't match what you're searching for (case-sensitive!)

## Next Steps

1. **Start Small**: Add 20 settlements first
2. **Test**: Play a few turns, verify prices change
3. **Expand**: Gradually add more settlements (goal: 300+)
4. **Balance**: Adjust production/needs based on playtesting

---

*For more info, see the main [Design Document](DESIGN.md)*
