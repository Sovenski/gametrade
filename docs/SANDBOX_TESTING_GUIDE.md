# Sandbox System Interaction Testing Guide

**Purpose**: Demonstrate that all game systems interact as a true sandbox

## 🎯 What is "Sandbox of Interacting Systems"?

A **sandbox game** means:
- ✅ Systems affect each other dynamically
- ✅ Player actions create emergent consequences
- ✅ No system operates in isolation
- ✅ Choices ripple across multiple systems

**Medieval Trading Dynasty achieves this by:**
```
Event System ←→ Economy System ←→ Production System
      ↕                                    ↕
Dynasty System ←→ Trade System ←→ Time System
```

Every system can influence every other system.

---

## 🧪 Running the Integration Tests

### Prerequisites
1. Project compiled successfully (see `COMPILATION_GUIDE.md`)
2. UE5 Editor open with GameTrade project
3. Settlement DataTable imported (95 cities)

### Method 1: Blueprint Testing (Easiest)

1. **Create Test Level**:
   - `Content Browser` → Right-click → `Level` → Name it `TestLevel`
   - Open `TestLevel`

2. **Create Test Actor**:
   - `Place Actors` → `All Classes` → Search for `Actor`
   - Drag an `Actor` into the level

3. **Add Level Blueprint Logic**:
   - `Blueprints` → `Open Level Blueprint`
   - Add these nodes:
     ```
     Event BeginPlay
     → Delay (1.0 seconds)
     → Get Game Instance
     → Get Subsystem (GTSystemTester)
     → Test Harvest Event Cascade
     ```

4. **Run Test**:
   - Click **Play (Alt+P)**
   - Open **Output Log** (`Window → Developer Tools → Output Log`)
   - Watch the test results!

### Method 2: C++ Console Commands (Advanced)

1. **Add to GTPlayerController.h**:
```cpp
UFUNCTION(Exec, Category = "Testing")
void RunSandboxTest(int32 TestNumber);
```

2. **Add to GTPlayerController.cpp**:
```cpp
void AGTPlayerController::RunSandboxTest(int32 TestNumber)
{
    switch (TestNumber)
    {
        case 1:
            UGTSystemTester::TestHarvestEventCascade(GetWorld());
            break;
        case 2:
            UGTSystemTester::TestProductionChainImpact(GetWorld());
            break;
        case 3:
            UGTSystemTester::TestDynastyEconomyIntegration(GetWorld());
            break;
        case 4:
            UGTSystemTester::TestMultiTurnIntegration(GetWorld());
            break;
        case 5:
            UGTSystemTester::TestTradeDisruptionCascade(GetWorld());
            break;
        case 6:
            UGTSystemTester::TestBlackDeathFullSandbox(GetWorld());
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("Invalid test number (1-6)"));
    }
}
```

3. **Run from Console**:
   - Open console (` key or ~)
   - Type: `RunSandboxTest 1`
   - Check Output Log for results

### Method 3: Automated Test Suite (Recommended for Development)

Create `Content/Blueprints/Testing/BP_TestRunner.uasset`:
- Runs all 6 tests in sequence
- Logs results to file
- Reports pass/fail for each test

---

## 📋 The 6 Integration Tests

### Test 1: Harvest Event Cascade
**What it tests**: Event → Economy → Production → Economy chain

**Expected flow**:
1. Event System triggers "Bad Harvest" in München
2. Economy System: Grain supply drops 60%
3. Economy System: Grain price spikes 2-3x
4. Production System: Bakeries can't afford grain → production drops
5. Economy System: Bread prices spike
6. (Trade System would import grain next turn)

**Success criteria**: Prices cascade correctly through supply chain

### Test 2: Production Chain Impact
**What it tests**: Production → Economy feedback loop

**Expected flow**:
1. Player builds 3 breweries in München
2. Production System: Breweries consume 90 grain/week
3. Economy System: Grain demand spikes → price rises
4. Production System: Breweries produce 150 beer/week
5. Economy System: Beer supply spikes → price drops
6. Market equilibrium shifts based on player action

**Success criteria**: Grain price ↑, Beer price ↓

### Test 3: Dynasty Economy Integration
**What it tests**: Dynasty → Economy → Production unlocks

**Expected flow**:
1. Player character marries Venetian merchant
2. Dynasty System: Grants "Venice Access" trait
3. Economy System: Unlocks Venice silk workshops
4. Production System: Player can build silk workshop
5. Trade System: Venice routes open with bonuses

**Success criteria**: Marriage unlocks exclusive production

### Test 4: Multi-Turn Integration
**What it tests**: All systems over 52 turns (1 year)

**Expected flow**:
1. Set up player with brewery in München
2. Run 52 turns (1 game year)
3. Track: Production → Sales → Gold → Events → Dynasty
4. Seasonal price fluctuations affect profits
5. Random events impact business
6. Character aging affects manager efficiency

**Success criteria**: All systems process correctly for full year

### Test 5: Trade Disruption Cascade
**What it tests**: Event → Trade → Economy → Production

**Expected flow**:
1. Event: War between England and France
2. Trade System: English Channel routes blocked
3. Economy System: English wool can't reach Bruges
4. Production System: Bruges cloth production drops (no wool)
5. Economy System: Cloth prices spike across Europe
6. Trade System: Alternative routes emerge (more expensive)

**Success criteria**: War event disrupts trade routes and prices

### Test 6: Black Death - Full Sandbox
**What it tests**: ALL systems during major crisis

**Expected flow**:
1. Event System: Triggers Black Death (1348)
2. Dynasty System: 30% of characters die (including managers)
3. Production System: Buildings lose managers → efficiency drops
4. Economy System: Population drops → demand crashes
5. Economy System: Labor shortage → wages spike
6. Trade System: Caravans avoid plague cities
7. Event System: Follow-up events (riots, religious fervor)
8. Player choices affect recovery speed

**Success criteria**: All 6 systems react and interact during crisis

---

## 📊 Reading Test Results

### Successful Test Output Example:
```
========================================
TEST 1: HARVEST EVENT CASCADE
========================================
BEFORE EVENT:
  München Grain Price: 3.00g
  Grain Supply: 100
  Grain Demand: 80

>>> TRIGGERING: Bad Harvest Event in München <<<

AFTER EVENT (Immediate):
  München Grain Price: 6.50g (was 3.00g)
  Grain Supply: 40 (dropped from bad harvest)
  >>> SYSTEM INTERACTION: Event → Economy ✓

>>> Production buildings react to grain shortage <<<
  Bakeries in München can't afford expensive grain
  Bread production will drop this turn
  >>> SYSTEM INTERACTION: Economy → Production ✓

AFTER ECONOMY UPDATE:
  Bread Price: 8.20g (increased due to low supply)
  >>> SYSTEM INTERACTION: Production → Economy ✓

========================================
CASCADE COMPLETE:
  Event System → triggered bad harvest
  Economy System → grain supply dropped, price spiked
  Production System → bakeries affected
  Economy System → bread prices rise
  (Trade System would import grain on next turn)
========================================

✓ TEST PASSED: Harvest Event Cascade
  Details: All systems interacted correctly: Event → Economy → Production → Economy
```

### What to Look For:
- ✅ **">>> SYSTEM INTERACTION: X → Y ✓"** - Systems communicating
- ✅ **Price changes** - Economy reacting to supply/demand
- ✅ **Cascading effects** - One change triggers others
- ✅ **"✓ TEST PASSED"** - Test succeeded

### Red Flags (Problems):
- ❌ **"✗ TEST FAILED"** - System interaction broken
- ❌ **Prices don't change** - Economy not reacting
- ❌ **No cascade** - Systems isolated
- ❌ **Crash or assert** - Code error

---

## 🔍 Verifying Sandbox Behavior Manually

### Manual Test 1: Price Manipulation
1. Open console: `ShowPrices München Grain`
2. Note price (e.g., 3.0g)
3. Console: `SellGoods München Grain 500`
4. Console: `NextTurn`
5. Console: `ShowPrices München Grain`
6. **Expected**: Price dropped (supply increased)
7. **Sandbox confirmed**: Your action → Economy reacted

### Manual Test 2: Production Cascade
1. Build brewery in München
2. Run 5 turns
3. Check grain prices in München (should rise)
4. Check beer prices in München (should drop)
5. **Sandbox confirmed**: Production → Economy prices

### Manual Test 3: Event Ripple
1. Trigger bad harvest: Console `TriggerEvent BadHarvest München`
2. Check grain price (should spike)
3. Run production turn
4. Check bakery output (should drop)
5. **Sandbox confirmed**: Event → Economy → Production

---

## 🎮 Gameplay Examples of Sandbox Interactions

### Example 1: The Beer Baron Strategy
```
TURN 1:
  Player: Build 5 breweries in Bavaria
  Production System: Grain demand spikes
  Economy System: Grain prices rise 40%

TURN 3:
  Trade System: AI caravans import grain to Bavaria
  Economy System: Grain prices stabilize
  Production System: Your breweries produce beer at lower cost

TURN 5:
  Economy System: Beer supply floods market
  Economy System: Beer prices drop 30%
  Event System: Guild complains about market saturation

TURN 6:
  Player: Export beer to France (higher prices)
  Trade System: Caravans travel 4 weeks
  Economy System: Bavarian beer shortage → prices rise again
```

**Sandbox behavior**: Player action → Economy reacts → Trade responds → Events trigger

### Example 2: The Black Death Opportunity
```
TURN 50:
  Event System: Black Death hits Europe
  Dynasty System: 30% of population dies
  Economy System: Demand crashes → prices drop

TURN 51:
  Player: Buy up cheap buildings from bankrupt merchants
  Production System: Acquire 10 workshops at 50% discount
  Dynasty System: Hire desperate workers at low wages

TURN 60:
  Economy System: Plague ends, population recovers
  Economy System: Demand returns, prices rise
  Production System: Your workshops highly profitable
  Player: 10x profit on crisis investment
```

**Sandbox behavior**: Crisis → Opportunity → Player exploits → Profit

### Example 3: The Marriage Alliance
```
TURN 100:
  Player: Marry daughter to Venetian merchant
  Dynasty System: Gain Venice access
  Economy System: Venetian silk workshops unlock

TURN 101:
  Player: Build silk workshop in Venice
  Production System: Produce 30 silk/week
  Trade System: Export to Paris (5x markup)

TURN 105:
  Event System: Rival family attempts assassination
  Dynasty System: Intrigue check (survive due to high intrigue stat)
  Event System: Rival family reputation damaged
  Economy System: You dominate silk trade
```

**Sandbox behavior**: Marriage → Unlocks → Profit → Rivals react → Intrigue

---

## 🏆 Sandbox Design Principles (How We Built It)

### 1. **No Hard-Coded Outcomes**
❌ Bad: `if (BadHarvest) { SetBreadPrice(10.0f); }`
✅ Good: `SupplyDrop(60%) → DemandConstant → PriceRise(auto-calculated)`

### 2. **Systems Read/Write Shared Data**
```cpp
// Economy writes:
Settlement->Supply[Grain] = 100;

// Production reads and modifies:
Settlement->Supply[Grain] -= 30; // Brewery consumed
Settlement->Supply[Beer] += 50;   // Brewery produced

// Economy recalculates:
UpdatePrices(); // Prices adjust to new supply/demand
```

### 3. **Event-Driven Communication**
```cpp
// Time subsystem broadcasts:
OnSeasonChanged.Broadcast(NewSeason);

// Economy subsystem listens:
TimeSubsystem->OnSeasonChanged.AddDynamic(this, &UGTEconomySubsystem::ApplySeasonalModifiers);
```

### 4. **Turn-Based Ordering**
```cpp
void AGTGameMode::NextTurn()
{
    // Order matters! Dependencies flow:
    TimeSubsystem->AdvanceTurn();           // 1. Update calendar
    ProductionSubsystem->ProcessTurn();     // 2. Create supply
    TradeSubsystem->ProcessTurn();          // 3. Move goods
    EconomySubsystem->ProcessTurn();        // 4. Adjust prices
    DynastySubsystem->ProcessTurn();        // 5. Age characters
    EventSubsystem->ProcessTurn();          // 6. Roll events
}
```

---

## ✅ Sandbox Verification Checklist

Before claiming "sandbox of interacting systems", verify:

- [ ] **Event → Economy**: Events change supply/demand/prices
- [ ] **Economy → Production**: Prices affect building profitability
- [ ] **Production → Economy**: Buildings change supply/demand
- [ ] **Dynasty → Production**: Character traits affect efficiency
- [ ] **Trade → Economy**: Caravans move goods between markets
- [ ] **Economy → Trade**: Price differences drive trade routes
- [ ] **Events → Dynasty**: Events can kill/marry/affect characters
- [ ] **Dynasty → Events**: Character traits affect event outcomes
- [ ] **Time → Economy**: Seasons affect prices
- [ ] **Time → Dynasty**: Years trigger aging/births/deaths
- [ ] **Multi-system cascades**: One action triggers 3+ systems
- [ ] **Emergent gameplay**: Unscripted situations arise

**If all checked = TRUE SANDBOX! ✓**

---

## 🚀 Next: Expanding the Sandbox

Once core interactions proven, expand with:

### Phase 1: More Events
- War events disrupt trade
- Natural disasters destroy buildings
- Political events change tariffs
- Religious events affect population

### Phase 2: AI Families
- AI families compete for resources
- AI makes same decisions as player
- Emergent rivalries and alliances

### Phase 3: Advanced Mechanics
- Infrastructure (roads improve trade speed)
- Politics (influence tariff rates)
- Warfare (destroy rival buildings)
- Espionage (steal trade secrets)

**Each addition multiplies sandbox interactions exponentially!**

---

**Remember**: A sandbox isn't about content volume, it's about **system depth** and **interaction richness**. Medieval Trading Dynasty achieves this by making every system aware of and reactive to every other system.

🎮 **The game doesn't play out the same way twice because the systems interact dynamically, not scripted!**
