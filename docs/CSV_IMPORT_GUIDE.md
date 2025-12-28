# CSV Settlement Data Import Guide

How to import the 95+ medieval European settlements into Unreal Engine 5.

## 📁 File Location

**CSV File**: `Content/Data/Settlements_Data.csv`
- 95 settlements included
- Historically accurate medieval Europe (1347-1450)
- Covers all major trade regions

## 🔧 Import Steps in UE5

### Step 1: Create DataTable

1. **Open Unreal Editor**
2. **Navigate to Content Browser**
   - Go to `Content/Data/` folder
3. **Right-click** → `Miscellaneous` → `Data Table`
4. **Choose Row Structure**: `FSettlementData`
   - This struct is defined in `GTDataTypes.h`
5. **Name it**: `DT_Settlements`

### Step 2: Import CSV

1. **Open `DT_Settlements`** (double-click)
2. **Click "Import" button** (top right)
3. **Select**: `Settlements_Data.csv`
4. **Import Options**:
   - Curve Interpolation Type: Linear
   - Import Row Struct: FSettlementData
5. **Click "Import"**

### Step 3: Verify Data

Check that you see:
- 95 rows loaded
- All columns populated
- RowName matches SettlementName

### Step 4: Fix Array Fields

**IMPORTANT**: CSV can't directly import arrays. You need to manually fill these for each settlement:

**LocalProduction** (what they produce):
- Format: Array of `EResourceType`
- Example for London: Add `Wool`, `Cloth`, `Tools`, `Weapons`

**LocalNeeds** (what they consume):
- Format: Array of `EResourceType`
- Example for London: Add `Wine`, `Spices`, `Grain`, `Timber`

**ConnectedSettlements** (trade routes):
- Format: Array of `FString`
- Example for London: Add "Dover", "Norwich", "York", "Southampton", "Calais"

**Method**:
1. Click on a settlement row
2. In the "Row Editor" panel (bottom)
3. Find `LocalProduction` field
4. Click "+" to add elements
5. Select resource from dropdown
6. Repeat for all settlements

---

## 📊 CSV Format Reference

### Columns Explained

```csv
RowName,SettlementName,SettlementType,TerritoryName,TerritoryType,Population,MapX,MapY,LocalProduction,LocalNeeds,BaseTariff,ConnectedSettlements
```

| Column | Type | Description | Example |
|--------|------|-------------|---------|
| RowName | String | Unique identifier (no spaces) | `London` |
| SettlementName | String | Display name | `London` |
| SettlementType | Enum | Village/Town/City/MajorCity/CapitalCity | `CapitalCity` |
| TerritoryName | String | Political region | `Kingdom of England` |
| TerritoryType | Enum | England_Crown, HRE_Duchy, etc. | `England_Crown` |
| Population | Integer | Medieval population estimate | `80000` |
| MapX | Float | X coordinate on map (0-1000) | `180` |
| MapY | Float | Y coordinate on map (300-700) | `430` |
| LocalProduction | String (CSV) | Comma-separated resources | `Wool,Cloth,Tools` |
| LocalNeeds | String (CSV) | Comma-separated resources | `Wine,Spices,Grain` |
| BaseTariff | Float | Tax rate (0.01 = 1%) | `0.02` |
| ConnectedSettlements | String (CSV) | Comma-separated city names | `Dover,Norwich,York` |

---

## 🗺️ Map Coordinate System

### Understanding Map Positions

**Coordinate Range**:
- **X-axis**: 0 (Atlantic) to 1000 (Eastern Europe)
- **Y-axis**: 300 (Scandinavia) to 700 (Mediterranean)

**Reference Points**:
- London: (180, 430)
- Paris: (240, 485)
- Venice: (540, 545)
- Rome: (440, 600)
- München: (450, 495)

**How to Calculate**:
1. Get real-world latitude/longitude
2. Normalize to map bounds
3. Formula:
   ```
   MapX = ((RealLongitude + 10) / 50) * 1000
   MapY = ((55 - RealLatitude) / 25) * 700 + 300
   ```

**Or just estimate**:
- Compare to nearby cities in CSV
- Use geographic knowledge
- Accuracy within 50 units is fine

---

## 🏙️ Settlement Types Guide

### Population Ranges

| Type | Population | Example Cities | Count in Dataset |
|------|-----------|----------------|------------------|
| Village | 1,000 - 5,000 | Oakwood, small hamlets | 0 (player creates) |
| Town | 5,000 - 15,000 | Freiburg, Geneva, Dover | ~15 |
| City | 15,000 - 50,000 | München, Bordeaux, York | ~40 |
| MajorCity | 50,000 - 150,000 | Milano, Genova, Bruges | ~25 |
| CapitalCity | 150,000+ | Paris, London, Venice | ~5 |

### Settlement Type Affects

- **Market Size**: Larger = more buyers/sellers
- **Building Slots**: Capitals have more
- **Events**: Major cities get more events
- **AI Activity**: More competition in cities

---

## 🌍 Territory Types & Tariffs

### Tariff Rates by Territory

| Territory Type | Base Tariff | Trading Difficulty | Examples |
|---------------|-------------|-------------------|----------|
| HRE_FreeCity | 1-2% | Easy | Lübeck, Frankfurt, Nürnberg |
| England_Crown | 2% | Easy | London, York, Bristol |
| France_Royal | 1-2% | Easy | Paris, Orléans, Tours |
| France_Duchy | 2-3% | Medium | Bordeaux, Nantes, Dijon |
| Italy_Republic | 1-2% | Easy | Venice, Genoa, Florence |
| Italy_Papal | 2% | Easy | Rome, Avignon, Bologna |
| HRE_Duchy | 3-5% | Hard | München, Wien, Stuttgart |
| HRE_Bishopric | 3-4% | Medium | Köln, Mainz, Salzburg |
| Poland_Kingdom | 3% | Medium | Kraków, Danzig |
| Hungary_Kingdom | 3% | Medium | Buda, Bratislava |
| Portugal_Kingdom | 2% | Easy | Lisboa, Porto |
| Others | 2-3% | Medium | Various |

**Trading Complexity**:
- Low tariff = centralized power = easy trading
- High tariff = fragmented power = many checkpoints

---

## 🔗 Connected Settlements

### How to Determine Connections

**Rules of thumb**:
1. **Geographic proximity** (within 300km)
2. **Historical trade routes** (documented)
3. **Natural routes** (rivers, seas, roads)
4. **Political allies** (same kingdom)

**Typical Connections**:
- Major cities: 5-8 connections
- Regional cities: 3-5 connections
- Towns: 2-4 connections

**Example** (München):
```
Connected to:
- Augsburg (close, Bavaria)
- Regensburg (Danube route)
- Salzburg (Alps crossing)
- Innsbruck (Italian trade route)
- Nürnberg (major trade route)
```

---

## 📦 Resource Distribution

### How Resources Are Assigned

**LocalProduction** = What they make/extract
- Based on geography (forests → timber)
- Based on history (English wool, Bohemian silver)
- Based on industry (Bruges → cloth)

**LocalNeeds** = What they consume
- Basics: Grain (everyone needs food)
- Materials: Timber, Iron (for production)
- Luxuries: Wine, Spices, Silk (cities)

### Regional Specializations

**England**:
- Production: Wool, Cloth, Tools
- Needs: Wine, Spices, Timber

**Flanders**:
- Production: Cloth, LuxuryCloth, Beer
- Needs: Wool, Grain, Wine

**Italy**:
- Production: Silk, Glass, Wine, Weapons
- Needs: Grain, Timber, Wool

**Holy Roman Empire**:
- Production: Beer, Tools, Weapons, Iron
- Needs: Wine, Cloth, Salt

**France**:
- Production: Wine, Grain, Cloth
- Needs: Wool, Spices, Timber

---

## 🛠️ Manual Editing After Import

### Tasks After CSV Import

1. **Fill Array Fields** (see Step 4 above)
   - LocalProduction
   - LocalNeeds
   - ConnectedSettlements

2. **Verify Map Coordinates**
   - Open map view
   - Check city positions make sense
   - Adjust if needed

3. **Balance Check**
   - Each resource produced somewhere
   - Each region has variety
   - No impossible trade chains

4. **Price Initialization**
   - Prices auto-initialize from `EconomySubsystem`
   - Based on `BasePrices` map
   - You can override in DataTable if needed

---

## 🔍 Data Validation Checklist

Before using in-game:

- [ ] All 95 settlements imported
- [ ] No duplicate RowNames
- [ ] All enums valid (SettlementType, TerritoryType)
- [ ] Population > 0 for all
- [ ] Map coordinates within bounds (X: 0-1000, Y: 300-700)
- [ ] LocalProduction filled (3-5 resources per city)
- [ ] LocalNeeds filled (3-5 resources per city)
- [ ] ConnectedSettlements filled (2-8 cities per city)
- [ ] BaseTariff between 0.01-0.05 (1-5%)

---

## 🎮 Testing After Import

### In-Game Verification

1. **Load Game**
   - Create new game
   - Check Output Log for "Loaded X settlements"

2. **Check Economy**
   - Use console: `ShowSettlements`
   - Should list all 95 cities

3. **Test Trading**
   - Use console: `ShowPrices London Grain`
   - Should show price data

4. **Test Travel**
   - Calculate travel time London → Paris
   - Should be ~3-4 weeks

5. **Test Production**
   - Buy brewery in München
   - Check it can produce beer
   - Verify grain consumption

---

## 🐛 Common Import Issues

### Issue 1: "Row structure mismatch"
**Cause**: CSV columns don't match FSettlementData struct
**Fix**: Check column order matches struct definition

### Issue 2: "Invalid enum value"
**Cause**: TerritoryType or SettlementType not in enum
**Fix**: Make sure you added all territory types to enum (Portugal_Kingdom, etc.)

### Issue 3: "Arrays empty after import"
**Cause**: CSV can't encode arrays
**Fix**: Manually fill LocalProduction, LocalNeeds, ConnectedSettlements

### Issue 4: "Prices not initializing"
**Cause**: Resource types in CSV not in BasePrices map
**Fix**: Add all resource types to `GTEconomySubsystem::Initialize()`

### Issue 5: "Cities in wrong location"
**Cause**: Map coordinate calculation error
**Fix**: Adjust MapX, MapY manually in DataTable

---

## 📝 Adding More Settlements

Want to expand beyond 95? Here's how:

### Step 1: Research
- Find medieval population data
- Identify major industries
- Determine trade connections

### Step 2: Add Row to CSV
```csv
Lublin,Lublin,City,Kingdom of Poland,Poland_Kingdom,8000,595,500,"Grain,Cloth,Leather","Timber,Iron,Wine,Salt",0.03,"Kraków,Lviv,Warszawa"
```

### Step 3: Re-import
- Delete existing DT_Settlements
- Create new one
- Import updated CSV
- Fill arrays manually

### Step 4: Verify
- Check it appears in game
- Test trading
- Check connections work

---

## 🌟 Pro Tips

1. **Start Small**: Test with 10-20 cities first, then import all 95
2. **Use Reference**: Keep a map of medieval Europe open while editing
3. **Historical Accuracy**: Research actual medieval trade routes
4. **Balance**: Don't make one city produce everything
5. **Variety**: Each region should have unique character
6. **Realism**: England really did export wool, Venice really had silk
7. **Gameplay**: Balance realism with fun (don't make everything super expensive)

---

## 📚 Additional Resources

**Historical References**:
- Medieval city populations: https://en.wikipedia.org/wiki/List_of_largest_European_cities_in_history
- Hanseatic League: https://en.wikipedia.org/wiki/Hanseatic_League
- Medieval trade routes: Various history books

**In-Game References**:
- [DESIGN.md](DESIGN.md) - Complete game design
- [PRODUCTION_CHAINS.md](PRODUCTION_CHAINS.md) - Resource chains
- [DATATABLE_SETUP.md](DATATABLE_SETUP.md) - DataTable basics

---

## ✅ You're Ready!

Once you've:
1. Imported CSV
2. Filled array fields
3. Verified data
4. Tested in-game

You have **95 historically accurate medieval European cities** ready to trade in!

The foundation of your trading empire awaits. 🏰⚔️💰
