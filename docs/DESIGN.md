# Medieval Trading Dynasty - Complete Design Document

## Executive Summary

**Genre**: Grand Strategy / Trading Simulation / Dynasty Management
**Setting**: Late Medieval Europe (1347-1450)
**Core Hook**: Build a merchant dynasty from peasant to European power through trade, production, intrigue, and marriage

## Core Gameplay Loop

### The Weekly Cycle (1 Turn = 1 Week)

```
MORNING (Player Input Phase)
├─ Check family status (births, deaths, marriages, aging)
├─ Review trade reports (what sold, prices, market changes)
├─ Review production (workshops output, consumption)
├─ Handle events (opportunities, crises, rival actions)
└─ Make decisions:
    ├─ Trade: Buy/sell goods, send caravans
    ├─ Production: Build/upgrade workshops, assign managers
    ├─ Dynasty: Arrange marriages, educate children
    ├─ Intrigue: Sabotage, espionage, assassination
    └─ Infrastructure: Build roads, villages, markets

MIDDAY (Simulation Phase)
├─ Caravans travel across map
├─ Production chains process (wheat→flour→bread)
├─ Goods flow into cities, prices adjust
├─ AI families execute their strategies
├─ Random events occur
└─ Player wealth changes

EVENING (Results Phase)
├─ Profit/loss summary
├─ Reputation changes
├─ New opportunities revealed
├─ Family drama events
└─ Rival family news

NEXT TURN BUTTON
└─ Repeat cycle
```

## Game World

### Map Structure

**300+ Settlements** across historically accurate medieval Europe:

**Settlement Tiers**:
1. **Villages** (1k-5k pop): Produce 1-2 raw resources
2. **Towns** (5k-20k pop): Process resources, regional hubs
3. **Cities** (20k-100k pop): Manufacturing, multiple industries
4. **Major Cities** (100k+ pop): Banking, luxury goods, huge markets

**Geographic Features**:
- **Mountains**: Alps, Pyrenees, Carpathians (slow travel, mining)
- **Rivers**: Rhine, Danube, Seine (fast transport, river trade)
- **Seas**: Mediterranean, Baltic, North Sea (shipping routes)
- **Forests**: Black Forest, Ardennes (timber, game)

### Political Map

#### Holy Roman Empire (Nightmare for traders)
- **300+ territories**, each with own tariffs
- **Free Imperial Cities**: Low tariffs (1-2%), trade-friendly
  - Lübeck, Hamburg, Köln, Frankfurt, Nürnberg, Augsburg
- **Duchies/Counties**: High tariffs (3-5%), bureaucratic
  - Bavaria, Saxony, Brandenburg, Austria
- **Crossing from North to South**: 7+ checkpoints, 25%+ total tariffs

#### Kingdom of France (Centralized trading)
- **Royal Domain**: No internal tariffs
- **Major Duchies**: Limited autonomy (2-3% tariffs)
  - Burgundy, Brittany, Aquitaine
- **Crossing France**: 1-2 checkpoints, 3-5% total tariffs

#### Italian City-States (Merchant paradise)
- **Republics**: Venice, Genoa, Florence (1-2% tariffs, trade focus)
- **Papal States**: Moderate tariffs, pilgrim economy
- **Fierce competition**: Venice vs Genoa rivalry

#### Other Major Regions
- **England**: Wool monopoly, wine imports
- **Flanders**: Cloth industry, trade hub (Bruges)
- **Iberia**: Castile (wool), Aragon (Mediterranean trade)
- **Eastern Europe**: Poland (grain), Hungary (copper/gold)
- **Scandinavia**: Timber, furs, fish (Hanseatic dominance)

### Resource & Production System

#### Tier 1: Raw Resources (Village Production)

**Agricultural** (farmland required):
- Grain (wheat, rye, barley) - Plains, river valleys
- Grapes (wine) - France, Rhine, Italy
- Olives (oil) - Mediterranean only
- Hops (beer) - Germany, Low Countries
- Flax (linen) - Flanders
- Hemp (rope) - Baltic

**Livestock** (pastures required):
- Cattle → meat, leather
- Sheep → wool, mutton (England #1 producer)
- Pigs → meat, lard (forest acorn-fed)
- Horses → transport, war (Hungary, Andalusia)

**Forestry** (forest regions):
- Timber (oak, pine) - Black Forest, Scandinavia
- Game (deer, boar) - hunting
- Furs (beaver, fox, bear) - Russia, Scandinavia

**Mining** (mountains/hills):
- Iron Ore → weapons, tools (Sweden, Austria)
- Copper → bronze, bells (Hungary, Saxony)
- Silver → currency, silverware (Bohemia - HUGE)
- Salt → food preservation (Austria, Poland)
- Tin → bronze (Cornwall, Bohemia)

**Fishing** (coastal):
- Herring → salted (MASSIVE Baltic trade)
- Cod → preserved
- General fish

#### Tier 2: Processed Goods (Town Workshops)

**Food Processing**:
- Grain → Flour (mill)
- Flour → Bread (bakery)
- Grapes → Wine (press + cellar)
- Hops + Grain → Beer (brewery)
- Meat + Salt → Preserved Meat (smokehouse)
- Fish + Salt → Salted Fish (huge export)

**Textile Processing**:
- Wool → Woolen Cloth (weaving)
- Wool + Dyes → Luxury Cloth (Flemish specialty!)
- Flax → Linen (weaving)

**Material Processing**:
- Hides → Leather (tannery)
- Timber → Planks (sawmill)
- Timber → Charcoal (kiln - needed for smelting!)

**Metal Processing**:
- Iron Ore + Charcoal → Iron Bars (bloomery)
- Copper + Tin → Bronze (forge)
- Silver Ore → Silver Ingots (smelter)

#### Tier 3: Manufactured Goods (City Craftsmen)

**Textiles**:
- Woolen Cloth → Garments (tailor)
- Luxury Cloth → Noble Garments (master tailor)
- Silk → Silk Garments (imported silk)

**Metalwork**:
- Iron Bars → Tools (blacksmith)
- Iron Bars → Weapons (swordsmith)
- Iron Bars → Armor (armorer)
- Bronze → Bells, Statues (foundry)
- Silver → Silverware (silversmith)

**Leather Goods**:
- Leather → Shoes (cobbler)
- Leather → Boots, Saddles (master cobbler)
- Leather → Armor (leatherworker)

**Advanced**:
- Glass (Venetian monopoly!)
- Paper (Italy, replacing parchment)
- Clocks (German specialty)

#### Tier 4: Luxury & Exotic (Long-Distance Trade)

**Eastern Imports** (Venice/Genoa monopoly):
- Silk (China) - 1g Alexandria → 150g London!
- Spices (India/Indonesia) - pepper, cinnamon, cloves
- Dyes (indigo, cochineal) - textile industry
- Precious stones

**Northern Imports** (Hanseatic League):
- Furs (Russian sable, ermine) - extreme luxury
- Amber (Baltic) - jewelry, medicine
- Wax (Russia) - church candles
- Honey (Poland)

**Luxury Manufactured**:
- Venetian Glass Mirrors (monopoly!)
- Flemish Tapestries (months to make one)
- Illuminated Manuscripts (monks)

### Settlement Economy Examples

#### Village: "Oakwood" (Black Forest)
**Population**: 450
**Type**: Forestry village
**Produces**:
- Timber: 100 units/week @ 2g = 200g
- Game: 20 units/week @ 5g = 100g
- Charcoal: 30 units/week @ 3g = 90g

**Consumes**:
- Salt: 10 units/week @ 8g = 80g
- Tools: 5 units/week @ 15g = 75g
- Grain: 20 units/week @ 3g = 60g

**Trade Balance**: +175g/week surplus
**Opportunity**: Build sawmill (Timber→Planks = 3x value!)

#### Town: "Freiburg" (Regional Hub)
**Population**: 8,500
**Type**: Processing center
**Produces**:
- Planks (4 sawmills): 300 units/week @ 6g = 1,800g
- Leather (2 tanneries): 100 units/week @ 10g = 1,000g
- Beer (1 brewery): 150 units/week @ 5g = 750g

**Consumes**:
- Timber (from villages): 400 units/week @ 2g = 800g
- Grain (from Strasbourg): 500 units/week @ 3g = 1,500g
- Iron Tools (from Austria): 50 units/week @ 20g = 1,000g
- Hides (from farms): 120 units/week @ 5g = 600g

**Trade Balance**: +650g/week
**Hub Function**: Transit point for Alpine trade

#### City: "Lyon" (Major Trade Hub)
**Population**: 35,000
**Type**: Manufacturing & banking center
**Produces**:
- Silk Fabrics: 200 units/week @ 150g = 30,000g
- Weapons: 50 units/week @ 80g = 4,000g
- Wine: 500 units/week @ 15g = 7,500g

**Consumes**:
- Raw Silk (Venice): 250 units/week @ 50g = 12,500g
- Grain (Burgundy): 2,000 units/week @ 3g = 6,000g
- Iron Bars (Lorraine): 100 units/week @ 30g = 3,000g

**Trade Balance**: +20,000g/week
**Special**: Annual trade fair (4x activity during fair weeks)

## Economic Systems

### Price Simulation

**Base Formula**:
```
Current Price = Base Price × Supply/Demand Ratio × Seasonal Modifier × Tariff Modifier
```

**Supply/Demand Dynamics**:
- Ratio < 0.5 (severe shortage) → Price × 1.6
- Ratio 0.5-1.0 (shortage) → Price × 1.3
- Ratio 1.0-2.0 (normal) → Price × 1.0
- Ratio > 2.0 (glut) → Price × 0.7

**Seasonal Modifiers**:
- Grain: Spring (1.3x), Summer (0.9x), Autumn (0.7x), Winter (1.1x)
- Furs: Spring (1.2x), Summer (1.5x), Autumn (1.1x), Winter (0.7x)
- Wine: Varies by harvest quality

**Tariff System**:
```
HRE Example (Lübeck → Basel):
├─ Hamburg: 2%
├─ Brunswick: 3%
├─ Hessen: 4%
├─ Frankfurt: 1%
├─ Württemberg: 5%
├─ Black Forest Pass: 10g flat
└─ Basel: 2%
Total: ~25% + 10g

France Example (Paris → Bordeaux):
├─ Royal Domain: 0%
└─ Aquitaine: 2%
Total: 2%
```

### Production Chain Mechanics

**Example: Bread Production**

```
RAW RESOURCES
└─ Farm produces Grain (10g cost, 20 units/week)
    ↓
PROCESSING
└─ Mill converts Grain → Flour (1:1 ratio, +2g processing cost)
    ↓
MANUFACTURING
└─ Bakery converts Flour → Bread (1:2 ratio, +5g processing cost)
    ↓
FINAL PRODUCT
└─ Bread sells for 8g/unit

Profit Calculation:
- Input: 10g (grain) + 2g (milling) + 5g (baking) = 17g
- Output: 40 units bread × 8g = 320g
- Profit: 303g/week per complete chain
```

**Player Strategy**:
- Own entire chain = maximum profit
- Specialize in one tier = simpler but less profit
- Location matters: Build near resources to reduce transport

### Travel & Transport

**Caravan System**:
- **Base Speed**: 35 km/week on roads
- **Capacity**: 20 units (small cart) to 200 units (large wagon train)
- **Modifiers**:
  - Roads: -25% time
  - Mountains: +100% time
  - Winter: +50% time
  - Sea routes: Weather dependent (storms delay 1-3 weeks)

**Example Journey** (Paris → Venice):
- Distance: ~750 km
- Base time: 750 / 35 = 22 weeks
- Via Alpine pass: +50% = 33 weeks
- In winter: +50% = 50 weeks!
- Alternatively: Sea route Marseille → Venice = 8 weeks

**Risks**:
- Bandits: 5-15% chance (higher in war zones)
- Weather: Storms (sea), snowstorms (winter mountains)
- Confiscation: If caught smuggling
- Spoilage: Perishable goods (bread, meat without salt)

## Dynasty System

### Character Mechanics

**Stats** (0-10 scale):
1. **Commerce**: Trading skill (better prices, spot opportunities)
2. **Stewardship**: Managing businesses (production efficiency +10% per point)
3. **Intrigue**: Espionage, sabotage, marriage negotiation
4. **Leadership**: Manages more employees, better loyalty
5. **Combat**: Protects caravans, survives assassination

**Traits** (inherited or gained):
- **Genius** (+2 to all stats, rare)
- **Quick** (+1 to Commerce & Stewardship)
- **Greedy** (+1 Commerce, -1 Leadership)
- **Diligent** (+1 Stewardship, -1 Intrigue)
- **Brave** (+2 Combat, -1 Intrigue)
- **Paranoid** (+2 Intrigue, -1 Leadership)

**Aging**:
- 0-15: Child (can't work)
- 16-20: Coming of age (can marry, assign to tasks)
- 21-50: Prime (peak stats)
- 51-60: Experienced (+1 to highest stat)
- 61+: Declining (stats decrease, death chance rises)

**Death**:
- Natural: 1% chance/year at age 50, +2% each year after
- Disease: Plague events (1347-1351 = 40% death chance!)
- Assassination: If targeted by rivals
- Accidents: Caravan raids, shipwrecks

### Marriage System

**Finding Partners**:
- Other merchant families (alliance, merge businesses)
- Minor nobility (political protection, prestige)
- Foreign merchants (access to new regions)

**Benefits**:
- **Business Merger**: Inherit spouse's family businesses (if line dies out)
- **Trade Access**: Permanent reduced tariffs in spouse's home region
- **Political**: Noble marriage = protection from confiscation
- **Children**: Inherit traits from both parents

**Example**:
```
Your son (Commerce 6, Greedy) marries Venetian merchant's daughter
↓
Benefits:
├─ 50% tariff reduction in Venice
├─ Access to Venice's spice import licenses
├─ Potential to inherit Venetian businesses
└─ Children have chance of inheriting Venetian trade connections

Cost:
└─ Dowry: 500g (negotiable with Intrigue skill)
```

### Succession

**Player Death**:
1. Choose heir before death (or auto: eldest child)
2. Heir inherits:
   - All businesses player owned
   - Gold & inventory
   - Reputation (partially)
   - Debts & obligations
3. Other children get smaller inheritances or nothing (creates resentment!)

**Succession Strategies**:
- **Primogeniture**: Everything to eldest (strong but unfair)
- **Equal Split**: Divide among children (fair but weakens empire)
- **Meritocracy**: Give to most skilled (best for business, angers others)

## AI Family System

### Rival Merchants

**5-10 AI families per region**, each with:
- Own characters, marriages, succession
- Economic strategies (focus on certain goods)
- Political allegiances
- Relationships with player (neutral → rival → enemy)

**AI Behaviors**:
1. **Trade Competition**: Buy goods player wants, undercut prices
2. **Production**: Build workshops in same cities
3. **Intrigue**: Sabotage player if hostile
4. **Marriage**: Marry each other, form alliances against player
5. **Expansion**: Spread to new regions

**Example Rival** (Schmidt Family, Bavaria):
```
Family Head: Friedrich Schmidt (Age 45, Commerce 7, Greedy)
├─ Owns: 3 breweries, 2 grain warehouses
├─ Strategy: Monopolize beer trade in Bavaria
├─ Relationship: Hostile (you undercut his prices)
└─ Recent Action: Bribed München guild to raise your fees

Player Options:
[A] Price War: Sell beer cheaper (profits drop, but crush him)
[B] Sabotage: Burn his brewery (risky, blood feud starts)
[C] Marriage: Propose alliance (merge families, end rivalry)
[D] Avoid: Trade in different region (safe but lose market)
```

## Events System

### Random Events

**Economic** (20% chance/turn):
- Good harvest (+50% grain supply, prices crash)
- Bad harvest (-50% grain supply, prices spike)
- Discovery: New silver mine (Bohemia gets rich)
- Bankruptcy: Rival family collapses (buy their assets cheap!)

**Natural Disasters** (5% chance/turn):
- Fire destroys city district (your buildings damaged?)
- Flood damages crops (grain shortage)
- Harsh winter (travel impossible for 10 turns)

**Political** (10% chance/turn):
- War declared (trade routes blocked, weapon demand surges)
- Peace treaty (routes reopen, weapon prices crash)
- New tariff imposed (costs rise)
- Trade privilege granted (exclusive rights to goods)

### Historical Events

**Black Death (1347-1351)**:
- Triggers automatically at start
- Each turn: 5% of population dies in random cities
- Effects:
  - Labor scarce (wages rise)
  - Luxury demand crashes (fewer buyers)
  - Land cheap (opportunity to buy buildings)
  - Your characters have death chance too!

**Hundred Years War** (ongoing):
- England vs France embargo
- Calais = smuggling hub (huge profits, huge risks)
- Periodic truces (trade reopens)

**Ottoman Expansion** (late period):
- Silk Road disrupted
- Spice prices skyrocket
- Venice/Genoa struggle

**Printing Press** (1450):
- New industry: Books!
- Paper demand surges
- Literacy spreads (new market)

## Intrigue & Conflict

### Sabotage
- **Burn Warehouse**: Destroy rival's goods (75g cost, 30% caught chance)
- **Corrupt Shipment**: Ruin goods in transit (50g, 20% caught)
- **Spread Rumors**: Damage rival's reputation (25g, safe)

### Espionage
- **Plant Spy**: Learn rival's trade routes (100g, reveals their strategy)
- **Steal Secrets**: Copy production methods (200g, unlock recipe)

### Assassination
- **Hire Assassin**: Kill rival family member (300-1000g)
- **Risk**: 40% success, 30% caught (you're imprisoned/fined), 30% fail silently
- **Consequence**: Blood feud (rival's family seeks revenge for generations)

### Defense
- **Hire Guards**: Protect caravans (10g/turn, reduces bandit risk)
- **Bribe Officials**: Prevent investigations (50g, +1 month protection)
- **Bodyguard**: Protect character from assassination (20g/turn)

## Infrastructure & Late Game

### Building Settlements

**Requirements**:
- 5,000g initial cost
- Strategic location (crossroads, resource-rich)
- Regional permission (noble patent: 1,000g)

**Process**:
1. Choose location on map
2. Attract settlers (build houses, offer jobs)
3. Construct basic buildings (market, inn)
4. Population grows over years
5. Becomes self-sustaining

**Benefits**:
- Named after your family!
- Tax income from residents
- Custom trade hub
- Prestige (+50)

### Building Roads

**Cost**: 500g per 100km
**Effect**: -25% travel time for everyone
**Income**: Toll collection (1g per caravan passing)
**Strategy**: Connect high-trade areas (e.g., Venice → München)

### Founding Industries

**Late Game Goal**: Control entire supply chain across Europe
```
YOUR EMPIRE (Year 1420):
├─ England: Own sheep farms (wool production)
├─ Flanders: Own weaving workshops (make cloth)
├─ France: Own dyeing facilities (luxury cloth)
├─ Italy: Sell finished cloth at 5x markup
└─ Result: Dominate European textile trade
```

## UI/UX Design

### Main HUD
```
┌─────────────────────────────────────────────────┐
│ [Family Crest]  Hans Müller    Gold: 1,234     │
│ Week 23, Year 1348 (Summer)   Prestige: 45     │
├─────────────────────────────────────────────────┤
│                                                  │
│         [EUROPE MAP - 3D VISUALIZATION]         │
│                                                  │
│  [Settlements] [Trade Routes] [Your Empire]     │
│                                                  │
├─────────────────────────────────────────────────┤
│ [Trade] [Family] [Production] [Map] [Events]    │
└─────────────────────────────────────────────────┘
```

### Trade Window
```
SETTLEMENT: München (Bavaria)
Population: 15,000 | Tariff: 3%

YOUR INVENTORY          MÜNCHEN MARKET
Grain      0           Grain      2.8g  (↓ 12%)  Supply: 450
Wool      25           Wool       9.2g  (↑ 5%)   Supply: 120
Beer      10           Beer       6.1g  (→ 0%)   Supply: 200

[BUY]  [SELL]  [SEND CARAVAN]
```

### Family Tree
```
Hans Müller (YOU - Age 32)
├─ m. Greta Schmidt (Age 29)
│   ├─ Klaus (Age 9) - Commerce 3, Genius trait
│   ├─ Anna (Age 5)
│   └─ Peter (Age 2)
└─ Siblings:
    ├─ Heinrich (Age 28) - manages Augsburg office
    └─ [DEAD] Marie - died in plague, 1348
```

## Progression & Victory Conditions

### Progression Tiers

**Tier 1: Peasant Trader** (Weeks 1-50)
- Goal: Earn first 500g
- Unlock: Buy first building
- Achievement: "First Profit"

**Tier 2: Local Merchant** (Weeks 50-200)
- Goal: Own 3 businesses, 2,000g wealth
- Unlock: Marriage system, AI rivals activate
- Achievement: "Regional Player"

**Tier 3: Regional Magnate** (Weeks 200-500)
- Goal: Dominate one region (5+ cities)
- Unlock: Intrigue, infrastructure building
- Achievement: "Power Broker"

**Tier 4: European Merchant Prince** (Week 500+)
- Goal: Multi-regional empire, 50,000g+ wealth
- Unlock: Found settlements, influence wars
- Achievement: "Dynasty Founder"

### Victory Conditions (Sandbox - Optional)

1. **Wealth**: Accumulate 100,000 gold
2. **Prestige**: Reach 1,000 prestige (marry into royalty)
3. **Monopoly**: Control 75% of one trade good across Europe
4. **Dynasty**: Have 50+ living family members
5. **Infrastructure**: Found 5 cities bearing your name

## Technical Implementation Notes

### Performance Considerations
- **300+ settlements**: Update prices in batches
- **AI pathfinding**: Pre-calculate major routes
- **Save file size**: Compress economy data
- **Turn processing**: Multi-thread subsystems

### Modding Support
- DataTables for all content (settlements, resources, events)
- Blueprint-exposed functions
- Custom event system
- Map editor for adding regions

---

## Next Steps for Development

**Phase 1** (Current):
- ✅ Core systems (Time, Economy, Save)
- ✅ Basic data structures
- 🚧 Settlement DataTable population
- 🚧 Simple UI prototypes

**Phase 2**:
- Production buildings
- Caravan system
- AI families (basic)
- Character aging & marriage

**Phase 3**:
- Events system
- Intrigue mechanics
- 3D Europe map
- Polish UI/UX

**Phase 4**:
- Infrastructure (roads, villages)
- Historical events
- Advanced AI
- Balance & playtesting

**Phase 5**:
- Multiplayer (optional)
- Modding tools
- Release!

---

*Version 0.1 - Last Updated: 2025-01-XX*
