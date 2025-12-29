# Hierarchical Economy System - Complete Guide

**CK3-Style Settlement Hierarchy with Multi-Layered Economies**

---

## 🏛️ Overview

The game now features a **fully hierarchical settlement system** like Crusader Kings 3, with multiple layers of economies that interact organically. This creates emergent gameplay where your decisions ripple through the entire economic ecosystem.

### Key Features

✅ **5-Level Hierarchy**: Empire → Kingdom → Duchy → County → Holdings
✅ **Passive Trade**: Adjacent settlements trade automatically based on needs
✅ **Population Growth**: Pops grow when needs are satisfied
✅ **AI Merchant Dynasties**: Competing families that build trade empires
✅ **Trade Route Ownership**: Buy shares, usurp routes, form cartels
✅ **Geography Matters**: Mountains block trade, rivers boost it
✅ **Multi-Layered Prices**: Local, regional, and imperial markets

---

## 🗺️ Settlement Hierarchy

### Empire (Top Level)
```
Holy Roman Empire
├── Imperial Tariff: 15% (high due to fragmentation)
├── Political Complexity: 0.9 (very complex)
└── Contains: Multiple kingdoms
```

**Examples:**
- Holy Roman Empire (fragmented, high tariffs)
- Kingdom of France (centralized, low tariffs)
- Kingdom of England

### Kingdom
```
Kingdom of Bavaria (within HRE)
├── Base Tariff: 8%
├── King: Ludwig IV
└── Contains: Multiple duchies
```

**Tariffs:**
- Same kingdom trade: Kingdom tariff only
- Cross-kingdom trade: Both kingdom tariffs + imperial tariff

### Duchy
```
Duchy of Upper Bavaria
├── Capital: München
├── Duke: Regional noble
└── Contains: Multiple counties
```

### County
```
County of München
├── Count: Local noble (feudal lord)
├── Regional Market: Aggregates all holdings
├── Contains: 3-6 holdings (cities, villages, castles)
└── Passive trade within county: FREE (no tariffs)
```

### Holdings (Lowest Level - Where You Operate)

**Types of Holdings:**

#### 1. **City** (You can own these!)
- **Population**: 5,000 - 100,000
- **Produces**: Manufactured goods (garments, tools, weapons)
- **Consumes**: Food, raw materials
- **Governance**: Merchant families (player or AI)
- **Example**: München - 15,000 people, produces beer & tools

#### 2. **Village** (Farming)
- **Population**: 300 - 3,000
- **Produces**: Agricultural goods (grain, cattle, hides)
- **Consumes**: Basic goods (bread, tools)
- **Governance**: Local peasants under feudal lord
- **Example**: Dachau - 800 people, produces 500 grain/week

#### 3. **Castle** (Feudal Military)
- **Population**: 500 - 2,000
- **Produces**: Weapons, armor
- **Consumes**: Food, luxury goods
- **Governance**: Feudal nobility
- **Example**: Freising - 1,200 people, military stronghold

#### 4. **Port** (Coastal Trade)
- **Population**: 2,000 - 50,000
- **Produces**: Fish, ships, trade goods
- **Consumes**: Various
- **Special**: Enables sea trade routes
- **Example**: Venice, Hamburg, Bordeaux

#### 5. **Mine** (Resource Extraction)
- **Population**: 200 - 1,500
- **Produces**: Ores, salt, precious metals
- **Consumes**: Food, tools
- **Example**: Silver mines near Kutná Hora

#### 6. **Monastery** (Religious)
- **Population**: 50 - 500
- **Produces**: Medicine, books, wine
- **Consumes**: Donations, food
- **Special**: Education and religious bonuses

---

## 💰 Multi-Layered Economy

### Local Economy (Holding Level)

Each holding has:
- **Production**: What it makes each week
- **Population Needs**: What its people require
- **Stockpile**: Current inventory

**Example - München City:**
```cpp
Production:
  - Beer: 200 units/week
  - Tools: 50 units/week
  - Garments: 30 units/week

Population Needs (for 15,000 people):
  - Bread: 300 units/week (starvation if < 150)
  - Beer: 150 units/week (comfort good)
  - Grain: 100 units/week (cooking)
  - Fish: 80 units/week (protein)
```

### Regional Economy (County/Duchy Level)

Counties aggregate all holdings:
- **Regional Supply** = Sum of all holding production
- **Regional Demand** = Sum of all population needs
- **Regional Prices** = Based on supply/demand ratio

**Example - München County:**
```
Supply:
  Grain: 500 (from Dachau village)
  Beer: 200 (from München city)
  Weapons: 20 (from Freising castle)

Demand:
  Bread: 340 (all holdings need food)
  Tools: 60 (all holdings need tools)

Price Calculation:
  High supply, low demand → LOW price
  Low supply, high demand → HIGH price
```

### Imperial Economy (Kingdom/Empire Level)

Long-distance trade between regions:
- **Arbitrage Opportunities**: Buy low in Bavaria, sell high in France
- **Tariff Costs**: Cross-kingdom trade = expensive
- **Specialization**: Venice silk, English wool, French wine

---

## 🔄 Passive Trade System

### How It Works

**Adjacent holdings automatically trade to satisfy needs!**

#### Conditions for Passive Trade:
1. **Same County**: Always trade freely (no tariffs)
2. **Adjacent Geography**: Within 100km if not blocked
3. **Not Blocked**: Mountains/large seas block passive trade

#### Geography Effects:
```cpp
Plains:    +20% trade speed
River:     +30% trade speed (fastest!)
Hills:     -10% trade speed
Forest:    -20% trade speed
Swamp:     -30% trade speed
Mountains: -50% trade speed (blocks if too high)
Coast:     Requires ships for sea trade
```

#### Example Passive Trade Flow:

**Turn 1:**
```
München City needs grain (deficit: 100 units)
Dachau Village has grain surplus (250 units)
Distance: 15km (plains) → +20% modifier

Passive Trade:
  → 100 grain flows Dachau → München
  → Dachau keeps 150 grain (reserves)
  → München population satisfied
```

**Turn 2:**
```
München's population is now 100% fed
Population growth: +0.15%/week
Next turn München has 15,023 people
Grain needs increase to 301 units/week
```

### Visualizing Passive Trade

```
     [Dachau Village]
            |
    500 Grain produced
            |
            ↓ Passive trade (100 grain)
            |
     [München City]
     Needs: 300 grain
     Receives: 100 from Dachau
     Still deficit: 200

     → Player opportunity: Set up trade route from distant grain producer!
```

---

## 👥 Population Growth

### Growth Mechanics

Population grows when needs are satisfied:

#### Needs Satisfaction Levels:

```cpp
< 50% Satisfied: STARVATION → -1.0% population/week
50-70%: BARELY SURVIVING → 0% growth (stable)
70-90%: COMFORTABLE → +0.1% growth/week
> 90%: THRIVING → +0.2% growth/week
```

#### Three Tiers of Needs:

**1. Basic Needs (Survival)**
Per 100 people per week:
- Bread: 20 units
- Fish/Meat: 10 units
- Grain: 15 units

**2. Comfort Needs (Growth)**
Per 100 people per week:
- Beer/Wine: 8 units
- Garments: 2 units
- Tools: 1 unit

**3. Luxury Needs (Prosperity)**
Per 100 people per week:
- Wine (if thriving): 3 units
- Spices: 1 unit
- Luxury Cloth: 1 unit

### Example Growth Simulation:

**Week 1:**
```
München: 15,000 population
Basic needs met: 95%
Comfort needs met: 70%
Luxury needs met: 20%

Overall satisfaction: 82%
→ Growth rate: +0.15%/week
→ Next week: 15,023 people
```

**Week 52 (1 year later):**
```
If satisfaction maintained at 82%:
Population: 15,000 × (1.0015)^52 = 16,185 people
→ 7.9% annual growth!
```

### Player Impact on Growth

**Positive actions:**
- Set up trade routes to supply food → satisfaction ↑
- Build bakeries → bread production ↑ → satisfaction ↑
- Import luxury goods → comfort ↑ → growth ↑

**Negative events:**
- War blocks trade routes → food shortage → starvation
- Bad harvest → grain deficit → satisfaction ↓
- Plague → population directly drops 30%

---

## 🤝 AI Merchant Dynasties

### AI Families

The game spawns **4 competing merchant families** at start:

1. **Medici** (Florence)
   - Aggressiveness: 0.7 (competitive)
   - Risk Tolerance: 0.6 (moderate)
   - Starting Gold: 15,000g

2. **Fugger** (Augsburg)
   - Aggressiveness: 0.9 (very aggressive!)
   - Risk Tolerance: 0.4 (conservative)
   - Starting Gold: 20,000g

3. **Hanseatic League** (Lübeck)
   - Aggressiveness: 0.5 (cooperative)
   - Risk Tolerance: 0.8 (high risk, high reward)
   - Starting Gold: 12,000g

4. **Venetian Guild** (Venice)
   - Aggressiveness: 0.8 (expansionist)
   - Risk Tolerance: 0.5 (balanced)
   - Starting Gold: 18,000g

### AI Behavior

**Every Turn, AI Merchants:**
1. **Evaluate Trade Opportunities**: Find profitable routes
2. **Build Trade Routes**: Create custom routes like player
3. **Expand Holdings**: Buy cities in strategic locations
4. **Compete with Player**: May try to buy your route shares
5. **Form Rivalries**: Aggressive AI will target player

### AI Decision Making

**High Aggressiveness AI:**
- Actively competes for same routes as player
- Offers to buy player's profitable routes
- May bribe feudal lords to block player
- Undercuts player prices

**High Risk Tolerance AI:**
- Invests in exotic long-distance trade (Silk Road)
- Speculates on future price changes
- Takes risky ventures through war zones

---

## 📊 Trade Route Ownership & Shares

### Custom Trade Routes

**Player can create custom routes:**
```cpp
Origin: München
Destination: Paris
Goods: 100 Beer/week
Distance: 700km
Travel Time: 3 weeks
```

### Ownership Structure

Routes can have **multiple owners** with **share percentages**:

```
Route: München → Paris Beer Trade
├── Player Family: 60% ownership
├── Fugger Family: 30% ownership
└── Local Guild: 10% ownership

Weekly Profit: 500 gold
├── Player receives: 300g (60%)
├── Fugger receives: 150g (30%)
└── Guild receives: 50g (10%)
```

### Buying Shares

**Example Transaction:**
```cpp
AI Merchant offers:
  "Sell me 25% of München-Paris route"
  Offering: 5,000 gold

If you accept:
  → You get 5,000g immediately
  → You now own 35% (was 60%)
  → Your weekly income drops from 300g → 175g
  → AI owns 55% (was 30%)
```

**Strategic Decisions:**
- Sell shares to raise capital for expansion
- Keep 100% ownership for maximum long-term profit
- Form partnerships with powerful AI families (protection)

### Usurping Routes (Political Intrigue)

**Three methods to acquire competitors' routes:**

#### 1. **Direct Purchase**
- Offer high price to owner
- Success chance based on relationship
- Clean, legal, safe

#### 2. **Bribing Feudal Lords**
```cpp
Bribe Duke of Bavaria: 2,000 gold
Success chance: 60%
Outcome: Duke "convinces" merchant to sell route to you
Risk: 10% chance duke keeps bribe AND route
```

#### 3. **Political Schemes**
- Marry into competitor's family → inherit routes
- Support rival's enemies → weaken them financially
- Expose competitor's illegal activities → force route seizure

---

## 🎮 Gameplay Examples

### Example 1: Building a Beer Empire

**Turn 1: Setup**
```
München produces 200 beer/week
Paris price: 8g/unit (high demand)
München price: 3g/unit (surplus)
Opportunity: 5g profit per unit!
```

**Action: Create Trade Route**
```
Route: München → Paris
Cargo: 100 beer/week
Travel time: 3 weeks
Investment: 1,000g (caravan setup)
```

**Turn 5: First Delivery**
```
100 beer arrives in Paris
Sell price: 8g × 100 = 800g
Buy cost: 3g × 100 = 300g
Tariffs: 15% (HRE) + 8% (France) = 184g
Net profit: 800 - 300 - 184 = 316g/delivery
```

**ROI: 31.6% per delivery, delivery every 3 weeks = ~500% annual return!**

**Turn 20: Expansion**
```
Fugger family notices your profits
Offers to buy 40% for 8,000g

Options:
A) Refuse: Keep 100% ownership, 316g every 3 weeks
B) Accept: Get 8,000g now, 190g every 3 weeks (60% share)
C) Counter-offer: Sell 20% for 5,000g (compromise)
```

### Example 2: Population Growth Strategy

**Goal: Grow München from 15,000 → 20,000 people**

**Turn 1: Analysis**
```
Current satisfaction: 75% (comfortable growth)
Bottleneck: Bread supply (only 70% satisfied)
Solution: Import grain to make more bread
```

**Action: Multi-Route Strategy**
```
1. Build trade route: Dachau → München (grain)
2. Build bakery in München
3. Set up safety net: Import grain from Poland if local shortage
```

**Turn 52 (1 year later):**
```
München population: 16,200 (+8% growth)
Bread satisfaction: 95%
Bakery profit: 200g/week additional
Increased tax revenue from bigger city
```

**Outcome: More people = more customers = more profit!**

### Example 3: AI Competition

**Turn 30: Fugger Rivalry**
```
Fugger family builds competing route: München → Paris
Their route: 150 beer/week (vs your 100)
Market impact: Paris beer price drops 8g → 6g
Your profit drops: 316g → 150g per delivery
```

**Strategic Options:**

**Option A: Price War**
```
Increase your shipments to 200 beer/week
Flood Paris market
Drive price down to 4g (barely profitable)
Fugger loses money, might abandon route
Risk: You also lose money short-term
```

**Option B: Alliance**
```
Propose cartel to Fugger:
"Both limit shipments to 100/week"
"Maintain high price at 8g"
"We both profit 300g+"
Risk: Fugger might defect
```

**Option C: Geographic Expansion**
```
Abandon Paris competition
Find new markets: London, Prague, Kraków
Fugger dominates Paris, you dominate East
Diversified income streams
```

**Option D: Hostile Takeover**
```
Bribe Duke of Bavaria to block Fugger's route
Cost: 3,000g bribe
Success: 70% chance
Outcome: You monopolize München-Paris trade again
Risk: 30% failure, lose bribe, Fugger becomes enemy
```

---

## 🧮 Economic Formulas

### Price Calculation

```cpp
BasePrice = 5.0g (varies by resource type)
SupplyDemandRatio = Supply / Demand

if (SupplyDemandRatio < 0.5) {
    Price = BasePrice × 2.5;  // Severe shortage
}
else if (SupplyDemandRatio < 0.8) {
    Price = BasePrice × 1.5;  // Shortage
}
else if (SupplyDemandRatio < 1.2) {
    Price = BasePrice × 1.0;  // Balanced
}
else if (SupplyDemandRatio < 2.0) {
    Price = BasePrice × 0.7;  // Surplus
}
else {
    Price = BasePrice × 0.4;  // Glut
}

// Apply seasonal modifiers
Price × SeasonalModifier (0.8 - 1.2)
```

### Population Growth Formula

```cpp
GrowthRate = 0.0;

if (NeedsSatisfaction < 0.5) {
    // Starvation
    GrowthRate = -0.01 × (1.0 - NeedsSatisfaction);
}
else if (NeedsSatisfaction >= 0.7 && NeedsSatisfaction < 0.9) {
    // Comfortable
    GrowthRate = 0.001 × (NeedsSatisfaction - 0.7);
}
else if (NeedsSatisfaction >= 0.9) {
    // Thriving
    GrowthRate = 0.002 × (NeedsSatisfaction - 0.9);
}

NewPopulation = Population × (1.0 + GrowthRate);
```

### Trade Profit Formula

```cpp
BuyPrice = OriginPrice + (OriginPrice × OriginTariff);
SellPrice = DestinationPrice;

// Tariff calculation
TotalTariff = 0.0;
if (DifferentCounty) TotalTariff += 0.02;
if (DifferentDuchy) TotalTariff += 0.05;
if (DifferentKingdom) TotalTariff += KingdomA.Tariff + KingdomB.Tariff;
if (DifferentEmpire) TotalTariff += Empire.Tariff;

TariffCost = (SellPrice - BuyPrice) × TotalTariff;
TransportCost = Distance × 0.1g per km;
GrossProfit = SellPrice - BuyPrice;
NetProfit = GrossProfit - TariffCost - TransportCost;

ProfitMargin = NetProfit / BuyPrice × 100%;
```

### Route Valuation (for buying/selling shares)

```cpp
RouteValue = WeeklyProfit × 52 weeks × 5 years / (1 + Risk);
// Typical route worth 3-5 years of profit

Example:
  Weekly profit: 300g
  Annual profit: 15,600g
  5-year projection: 78,000g
  Risk factor: 0.2 (low risk route)
  Route Value: 78,000 / 1.2 = 65,000g

  10% share = 6,500g
  25% share = 16,250g
  50% share = 32,500g
```

---

## 🔧 Technical Implementation

### Data Flow

```
Turn Processing Order:
1. Time advances
2. Holdings produce goods → add to stockpile
3. PASSIVE TRADE: Adjacent holdings exchange goods automatically
4. CUSTOM ROUTES: Player/AI routes transport goods
5. ECONOMY AGGREGATION:
   - Holding → County (regional prices)
   - County → Duchy (duchy market)
   - Duchy → Kingdom (imperial market)
6. Population checks needs → calculates satisfaction
7. Population grows/shrinks based on satisfaction
8. AI merchants evaluate opportunities → create routes
9. Events can disrupt economy (war, plague, harvest)
```

### Key Classes

**`UGTHierarchicalEconomySubsystem`**
- Manages all hierarchy levels
- Processes passive trade
- Handles population growth
- Controls AI merchants

**`FHoldingData`**
- Individual settlement
- Production, consumption, stockpile
- Population and needs satisfaction

**`FPassiveTradeConnection`**
- Auto-generated trade link
- Geography modifiers
- Real-time goods flow

**`FAIMerchantDynasty`**
- AI competitor
- Personality traits (aggressiveness, risk)
- Relationship with player

**`FCustomTradeRoute`**
- Player/AI created route
- Ownership shares
- Profit distribution

---

## 📈 Strategy Guide

### Early Game (Turns 1-50)

**Focus: Build Foundation**
1. Start in your home city (e.g., München)
2. Identify local needs vs. production
3. Set up 2-3 simple trade routes within duchy (low tariffs)
4. Build 1-2 production buildings (brewery, bakery)
5. Keep 100% ownership of your routes

**Early Mistakes to Avoid:**
- ❌ Long-distance routes too early (high tariffs eat profits)
- ❌ Selling route shares for quick gold (lose long-term income)
- ❌ Ignoring population needs (no growth = stagnant market)

### Mid Game (Turns 50-200)

**Focus: Expand Territory**
1. Buy shares in AI routes (learn from competitors)
2. Expand to 2-3 cities in different duchies
3. Specialize each city: München (beer), Augsburg (textiles), Nürnberg (metalwork)
4. Form first cartel with friendly AI (shared routes, price control)
5. Marry into noble family for reduced tariffs

**Mid Game Goals:**
- Own holdings in 3+ cities
- Control 5+ profitable trade routes
- Population in your cities growing 5%+ annually
- Friendly relationship with 2+ AI merchant families

### Late Game (Turns 200+)

**Focus: Domination**
1. **Option A: Trade Monopoly**
   - Control 80%+ of specific good (e.g., all Bavarian beer)
   - Set prices through market dominance
   - Block competitors via political influence

2. **Option B: Geographic Empire**
   - Own major city in every kingdom
   - Create pan-European trade network
   - Arbitrage between regional markets

3. **Option C: Hostile Takeover**
   - Systematically acquire AI routes via schemes
   - Bribe feudal lords to grant monopolies
   - Become wealthiest family in HRE

**Endgame Milestones:**
- 100,000+ gold in treasury
- Own 10+ cities
- Control 20+ major trade routes
- Son/daughter married to king/emperor
- AI families either allied or bankrupted

---

## 🎯 Success Metrics

**You're winning when:**
- ✅ Your cities' populations growing 5-10% annually
- ✅ Weekly profit > 1,000 gold from trade routes
- ✅ Own shares in 10+ routes
- ✅ All your cities have 90%+ needs satisfaction
- ✅ AI merchants respect you (opinion > 50)
- ✅ You can manipulate regional prices through supply control

**You're struggling when:**
- ❌ Cities' populations stagnant or declining
- ❌ Weekly profit < 100 gold
- ❌ AI merchants hostile (opinion < -20)
- ❌ Can't afford to expand (< 1,000g in treasury)
- ❌ Passive trade not flowing to your cities

---

## 🚀 Future Expansions

**Planned Features:**
- [ ] Banking system (loans to AI, invest in ventures)
- [ ] Insurance for trade routes (reduce risk)
- [ ] Trade guilds (collective bargaining power)
- [ ] Warehouse management (store goods, speculation)
- [ ] Shipping fleets (own the caravans, not just cargo)
- [ ] Feudal titles (buy county, become nobility)
- [ ] Espionage (sabotage competitor routes)
- [ ] Smuggling (avoid tariffs, high risk)

---

**This system creates a living, breathing medieval economy where your actions have consequences that ripple through entire kingdoms. Master the hierarchy, dominate the trade routes, and build a dynasty that spans generations!**
