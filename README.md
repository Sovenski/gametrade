# Medieval Trading Dynasty

A grand strategy trading game set in late medieval Europe (1347-1450). Build your merchant dynasty through trade, production chains, political intrigue, and dynastic marriages.

## 🎮 Game Overview

Start as a humble peasant trader and build a mercantile empire that spans Europe. Navigate complex political borders, establish production chains, marry into powerful families, and compete against rival merchant dynasties. Every decision has consequences that echo through generations.

### Core Gameplay Loop

**1 Turn = 1 Week** | **52 Turns = 1 Year**

```
PLAN → EXECUTE → WATCH → PROFIT → EXPAND
  ↓        ↓         ↓        ↓        ↓
Scout → Trade → Travel → Sell → Invest
```

## 🌍 Game Features

### ✅ Currently Implemented (v0.1)

- **Turn-Based Time System**: Calendar with seasons (Spring/Summer/Autumn/Winter)
- **Dynamic Economy**: Supply/demand price simulation across 300+ settlements
- **Resource System**: 60+ tradeable goods (grain, wool, iron, spices, etc.)
- **Political Borders**: Realistic medieval territories with variable tariffs
  - Holy Roman Empire: 300+ fragmented territories (high tariffs, complex)
  - Kingdom of France: Centralized (low tariffs, easier trading)
  - Italian City-States: Merchant republics (moderate complexity)
- **Save/Load System**: Multiple save slots with auto-save
- **Character System**: Stats, traits, aging, death, succession
- **Settlement Types**: Villages, Towns, Cities with unique production/needs

### 🚧 In Development

- **Production Chains**: Farm → Mill → Bakery
- **Buildings**: Workshops, warehouses, farms, mines
- **Caravans**: Send goods between cities
- **AI Families**: Competing merchant dynasties
- **Marriage System**: Dynastic alliances and inheritance
- **Events**: Random encounters, opportunities, disasters
- **3D Map**: Beautiful visualization of medieval Europe

### 📋 Planned Features

- **Intrigue**: Assassination, sabotage, espionage
- **Infrastructure**: Build roads, villages, forts
- **Combat**: Protect trade routes, hire guards
- **Politics**: Influence nobles, gain trade monopolies
- **Historical Events**: Black Death, Hundred Years War, etc.

## 🛠️ Technical Stack

- **Engine**: Unreal Engine 5.4
- **Language**: C++ (core systems) + Blueprints (UI/gameplay)
- **Architecture**: Subsystem-based (Economy, Time, Dynasty, Events)
- **Data**: DataTables for settlements, resources, buildings
- **Save System**: UE5 SaveGame system with JSON serialization

## 📁 Project Structure

```
GameTrade/
├── Source/GameTrade/
│   ├── Core/              # Game Instance, Game Mode, Player Controller, Save System
│   ├── Data/              # Data structures (Settlement, Character, Resource, Building)
│   ├── Systems/           # Subsystems (Economy, Time, Trade, Dynasty, Events)
│   └── AI/                # AI merchant families
├── Content/
│   ├── Data/              # DataTables (settlements, resources, territories)
│   ├── Maps/              # Levels (MainMenu, GameWorld)
│   ├── UI/                # UMG widgets (HUD, menus, windows)
│   ├── Blueprints/        # Blueprint actors and components
│   └── Materials/         # Map materials, UI materials
└── Config/                # Engine/Game/Input configuration
```

## 🚀 Getting Started

### Prerequisites

- **Unreal Engine 5.4** or later
- **Visual Studio 2022** (Windows) or **Xcode** (Mac)
- **Git** (for version control)
- **16GB RAM minimum** (32GB recommended)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/Sovenski/gametrade.git
   cd gametrade
   ```

2. **Generate project files**
   - Right-click `GameTrade.uproject`
   - Select "Generate Visual Studio project files"

3. **Open in Unreal Editor**
   - Double-click `GameTrade.uproject`
   - Wait for shaders to compile (first time only)

4. **Compile C++ code**
   - In UE5 Editor: Tools → Compile
   - Or in Visual Studio: Build → Build Solution

### First Launch Setup

1. **Create Settlement Data**
   - Navigate to `Content/Data/`
   - Create new DataTable: `DT_Settlements`
   - Row Structure: `FSettlementData`
   - Add initial settlements (see `DATATABLE_SETUP.md`)

2. **Set Game Instance**
   - Edit → Project Settings → Maps & Modes
   - Game Instance Class: `GTGameInstance`

3. **Test the game**
   - Press Play in editor
   - Check Output Log for initialization messages

## 🎯 Quick Start Guide (For Players)

### Starting Your Dynasty

1. **New Game**
   - Choose your family name
   - Select starting settlement (Bavaria recommended for tutorial)
   - You start with 50 gold and a small cart

2. **First Trade Run**
   - Press `T` to open Trade window
   - Check prices in your village vs nearby city
   - Buy low in village, sell high in city
   - Example: Grain (2g village) → Sell in München (4g) = 100% profit!

3. **Expand Your Empire**
   - Save profits to buy a workshop
   - Establish production chains (raw → processed → manufactured)
   - Marry children to other merchant families
   - Compete with rival dynasties

### Controls

- **Space/Enter**: Advance turn (1 week)
- **Escape**: Pause game
- **M**: Open map
- **T**: Trade window
- **F**: Family tree
- **P**: Production overview
- **F5**: Quick save
- **F9**: Quick load

## 📊 Game Systems Explained

### Economy System

**Dynamic Pricing**: Prices change based on supply/demand
- Settlement produces grain → Supply increases → Price drops
- Settlement consumes grain → Demand increases → Price rises
- Seasonal effects: Grain cheap in autumn (harvest), expensive in spring
- Wars/events create opportunities (weapon demand during war)

**Trade Mechanics**:
```
Buy Price = Base Price × (1 + Tariffs) × Seasonal Modifier × Supply/Demand Ratio
```

### Time System

- **1 Turn = 1 Week**
- **Seasons affect gameplay**:
  - Spring: Planting, prices rising, mud slows travel
  - Summer: Best travel weather, trade fairs
  - Autumn: Harvest (price crash for grain), prepare for winter
  - Winter: Slow travel (+50% time), high demand for furs/firewood

### Dynasty System

- **Characters age**: +1 year every 52 turns
- **Skills**: Commerce, Stewardship, Intrigue, Leadership, Combat
- **Traits**: Genius, Greedy, Brave, etc. (affect skill gains)
- **Marriage**: Alliance, merge businesses, political power
- **Death**: Natural causes, assassination, accidents
- **Succession**: Choose heir or split inheritance

## 🔧 Development Guide

### Adding a New Settlement

1. Open `Content/Data/DT_Settlements`
2. Add new row with unique name (e.g., "Koln")
3. Fill in data:
   - Settlement Type: City
   - Territory: HRE_FreeCity
   - Population: 35000
   - Map Location: X=300, Y=450 (relative to map)
   - Local Production: [Grain, Beer, Tools]
   - Local Needs: [Wine, Cloth, Spices]
   - Base Tariff: 0.02 (2%)
   - Connected Settlements: [Frankfurt, Mainz, Aachen]

### Adding a New Resource

1. **Add to enum** (`GTDataTypes.h`):
   ```cpp
   UENUM(BlueprintType)
   enum class EResourceType : uint8
   {
       // ...existing...
       Sugar UMETA(DisplayName = "Sugar"),
   };
   ```

2. **Set base price** (`GTEconomySubsystem.cpp`):
   ```cpp
   BasePrices.Add(EResourceType::Sugar, 50.0f);
   ```

3. **Add to settlements**: Update DataTable

### Creating a Production Chain

Example: Grain → Flour → Bread

1. **Define building** in `DT_Buildings`:
   - Mill: Input [Grain], Output [Flour]
   - Bakery: Input [Flour], Output [Bread]

2. **Buildings system processes** each turn:
   - Check if inputs available
   - Consume inputs
   - Produce outputs
   - Manager skill affects efficiency

### Debugging

**Console Commands** (press ~ to open console):
- `NextTurn` - Advance 1 turn
- `FastForward 52` - Advance 1 year
- `ShowPrices Paris` - Display all prices in Paris
- `SetGold 1000` - Add gold (cheat)

**Logging**:
- Economy: `Log LogTemp` for price changes
- Time: `Log LogTemp` for season/year changes
- Save System: `Log LogTemp` for save/load operations

## 🗺️ Historical Accuracy

### Time Period: 1347-1450

- **Black Death** (1347-1351): Population crash, economic disruption
- **Hundred Years War** (1337-1453): England vs France trade embargo
- **Golden Bull** (1356): HRE political structure
- **Hanseatic League**: Northern European trade monopoly
- **Medici Rise**: Italian banking dominance (post-1400)

### Realistic Features

- **Tariffs**: HRE has 7+ checkpoints, France has 1-2
- **Travel Times**: Paris to Venice = 9 weeks (historically accurate)
- **Resources**: England = wool, Flanders = cloth, Venice = spices
- **Political**: Free Cities have low tariffs, duchies have high tariffs

## 📖 Tutorials & Resources

- [Full Design Document](docs/DESIGN.md)
- [DataTable Setup Guide](docs/DATATABLE_SETUP.md)
- [Blueprint Integration](docs/BLUEPRINTS.md)
- [Modding Guide](docs/MODDING.md)

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Areas Needing Help

- **Data Entry**: Adding 300+ historical settlements to DataTable
- **UI/UX**: Designing beautiful medieval-themed UI
- **3D Art**: Creating map of Europe, city icons, building models
- **Balancing**: Testing and balancing economy simulation
- **Events**: Writing historical events and random encounters

## 📝 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Credits

- **Design & Programming**: Sovenski
- **Historical Research**: [Your sources]
- **Unreal Engine**: Epic Games
- **Inspiration**: Grand Ages Medieval, Anno 1404, Crusader Kings 3

## 📞 Contact

- **GitHub**: [https://github.com/Sovenski/gametrade](https://github.com/Sovenski/gametrade)
- **Discord**: [Coming soon]
- **Email**: [Your email]

---

**Current Version**: 0.1.0 (Pre-Alpha)
**Last Updated**: 2025-01-XX

*"From peasant to prince, through trade and cunning."*
