# Medieval Trading Dynasty - Compilation & Testing Guide

**Version**: 0.2.0 Pre-Alpha
**Last Updated**: 2025-01-XX

## 📋 Prerequisites

Before compiling, ensure you have:
- ✅ **Unreal Engine 5.4+** installed
- ✅ **Visual Studio 2022** (or 2019) with C++ workload
- ✅ **Windows SDK 10.0.18362.0+**
- ✅ Git (for version control)

---

## 🔨 Step 1: Generate Visual Studio Project Files

### Method 1: Right-Click (Recommended)
1. Navigate to `/home/user/gametrade/`
2. **Right-click** on `GameTrade.uproject`
3. Select **"Generate Visual Studio project files"**
4. Wait for generation to complete (~30 seconds)

### Method 2: Command Line
```bash
cd /home/user/gametrade
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" -projectfiles -project="GameTrade.uproject" -game -engine
```

**Expected Output:**
- `GameTrade.sln` file created
- `.vs/` folder created
- `Intermediate/` folder created

---

## 🏗️ Step 2: Compile C++ Code

### Method 1: Via Unreal Editor (Easiest)
1. **Double-click** `GameTrade.uproject`
2. Unreal Editor will detect C++ code
3. Click **"Yes"** to compile modules
4. Wait for compilation (~3-5 minutes first time)
5. Editor opens when complete

### Method 2: Via Visual Studio (For Debugging)
1. **Open** `GameTrade.sln` in Visual Studio 2022
2. Set build configuration to **"Development Editor"**
3. Set platform to **"Win64"**
4. **Build Solution** (Ctrl+Shift+B)
5. Wait for compilation (~3-5 minutes)
6. Check **Output Window** for errors

**Expected Output:**
```
1>------ Build started: Project: GameTrade, Configuration: Development Editor x64 ------
1>Parsing headers for GameTradeEditor
1>Building GameTradeEditor...
1>Compiling C++ source files...
1>   GTTimeSubsystem.cpp
1>   GTEconomySubsystem.cpp
1>   GTProductionSubsystem.cpp
1>   GTTradeSubsystem.cpp
1>   GTDynastySubsystem.cpp
1>   GTEventSubsystem.cpp
1>   GTGameMode.cpp
1>   GTPlayerController.cpp
1>   GTGameInstance.cpp
1>   GTSaveGame.cpp
1>Creating library...
1>Build succeeded.
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

---

## ✅ Step 3: Verify Compilation Success

### Check Output Log (in UE5 Editor)
Open **Window → Developer Tools → Output Log**

Look for these initialization messages:
```
LogTemp: GTGameMode BeginPlay
LogTemp: GTTimeSubsystem Initialized - Starting date: Week 1, Year 1347
LogTemp: GTEconomySubsystem Initialized - 0 settlements loaded
LogTemp: GTProductionSubsystem Initialized
LogTemp: GTTradeSubsystem Initialized
LogTemp: GTDynastySubsystem Initialized
LogTemp: GTEventSubsystem Initialized - 6 event templates registered
```

**✅ If you see all 6 subsystems initialize = SUCCESS!**

---

## 🎮 Step 4: Run First Turn (Testing)

### Option A: Blueprint Testing (Create Test Level)
1. Create new **Level Blueprint**
2. Add these nodes:
   ```
   Event BeginPlay
   → Get Game Mode
   → Cast to GTGameMode
   → Call NextTurn()
   ```
3. **Play (Alt+P)**
4. Check **Output Log** for turn processing

### Option B: C++ Console Command (Advanced)
1. Open `GTPlayerController.cpp`
2. Add console command:
   ```cpp
   void AGTPlayerController::TestTurn()
   {
       AGTGameMode* GameMode = Cast<AGTGameMode>(GetWorld()->GetAuthGameMode());
       if (GameMode)
       {
           GameMode->NextTurn();
       }
   }
   ```
3. In editor console: `TestTurn`

### Option C: Debug in Visual Studio
1. Set breakpoint at `GTGameMode.cpp:67` (turn processing start)
2. Press **F5** to launch with debugger
3. Step through turn processing

---

## 📊 Expected Turn Processing Output

When you run `NextTurn()`, you should see:

```
LogTemp: === TURN PROCESSING START ===
LogTemp: Turn advanced to: Week 2, Spring 1347
LogTemp: Processing 0 buildings for production
LogTemp: Processing 0 caravans for travel
LogTemp: Updating prices for 0 settlements
LogTemp: Processing aging for 0 characters
LogTemp: Rolling for random events (0% chance this turn)
LogTemp: === TURN PROCESSING COMPLETE ===
```

**Note:** Output shows "0" for everything because we haven't populated data yet!

---

## 🗂️ Step 5: Import Settlement Data

Now let's populate the game with actual data:

### 5.1 Create Settlement DataTable
1. In **Content Browser**, navigate to `Content/Data/`
2. **Right-click** → **Miscellaneous** → **Data Table**
3. Choose row structure: **`FSettlementData`**
4. Name it: **`DT_Settlements`**

### 5.2 Import CSV
1. **Open** `DT_Settlements` (double-click)
2. Click **"Import"** button (top toolbar)
3. Select: `Content/Data/Settlements_Data.csv`
4. Import settings:
   - **Import Row Struct**: `FSettlementData`
   - **Curve Interpolation**: Linear
5. Click **"Import"**

### 5.3 Verify Import
Check that you see:
- ✅ **95 rows** loaded
- ✅ Columns: SettlementName, SettlementType, TerritoryName, Population, MapX, MapY, BaseTariff
- ✅ RowNames match city names (London, Paris, Venice, etc.)

**⚠️ IMPORTANT:** Arrays won't import from CSV! See `CSV_IMPORT_GUIDE.md` for manual array filling.

---

## 🧪 Step 6: Test With Real Data

### Create Test Game
1. Create **new Blueprint** based on `GTGameMode` (call it `BP_GTGameMode`)
2. In **Class Defaults**, set:
   - **Settlement DataTable**: `DT_Settlements`
3. Set as **default GameMode** in **Project Settings → Maps & Modes**

### Run Turn Processing
1. **Play in Editor (PIE)**
2. Open **Output Log**
3. Press **Space** (bound to NextTurn in DefaultInput.ini)

**Expected Output (with data):**
```
LogTemp: === TURN PROCESSING START ===
LogTemp: Turn advanced to: Week 2, Spring 1347
LogTemp: Processing 0 buildings for production (player has none yet)
LogTemp: Processing 0 caravans for travel (player has none yet)
LogTemp: Updating prices for 95 settlements
LogTemp: Settlement [London] - Grain price: 3.2g (supply: 100, demand: 80)
LogTemp: Settlement [Paris] - Grain price: 2.8g (supply: 120, demand: 90)
LogTemp: Settlement [Venice] - Wine price: 12.5g (supply: 60, demand: 100)
LogTemp: Processing aging for 0 characters (create your character!)
LogTemp: Random event rolled: No event this turn
LogTemp: === TURN PROCESSING COMPLETE ===
```

---

## 🐛 Common Compilation Issues

### Issue 1: "Cannot open include file 'GTDataTypes.h'"
**Cause**: Wrong include path
**Fix**:
```cpp
// ❌ Wrong
#include "GTDataTypes.h"

// ✅ Correct
#include "Data/GTDataTypes.h"
```

### Issue 2: "Unresolved external symbol UGTTimeSubsystem"
**Cause**: Subsystem not registered
**Fix**: Ensure `GameTrade.Build.cs` has:
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore"
});
```

### Issue 3: "FSettlementData not found"
**Cause**: Missing `USTRUCT()` or wrong module
**Fix**: Check `GTDataTypes.h` has:
```cpp
USTRUCT(BlueprintType)
struct GAMETRADE_API FSettlementData : public FTableRowBase
{
    GENERATED_BODY()
    // ...
};
```

### Issue 4: Compilation Takes Forever
**Cause**: First-time compile compiles engine headers
**Fix**: Be patient (3-5 minutes normal). Subsequent compiles ~30 seconds.

### Issue 5: "GameInstance class not found"
**Cause**: Config not set
**Fix**: Open `Config/DefaultEngine.ini`, ensure:
```ini
[/Script/Engine.Engine]
GameInstanceClass=/Script/GameTrade.GTGameInstance
```

---

## 🚀 Performance Optimization (For Later)

Once game works, optimize with:

### 1. Use Shipping Build for Performance Testing
```
Build Configuration: Shipping
Platform: Win64
```

### 2. Profile Subsystems
```cpp
SCOPE_CYCLE_COUNTER(STAT_EconomyProcessTurn);
void UGTEconomySubsystem::ProcessEconomyTurn()
{
    // Your code
}
```

### 3. Batch Operations
Already implemented in `GTEconomySubsystem::ProcessEconomyTurn()`:
```cpp
for (auto& Pair : AllSettlements) {
    // Batch update prices
}
```

---

## 🎯 Next Steps After Successful Compilation

1. **Create Player Character**
   - Use Dynasty system to create starting merchant
   - Set starting city (e.g., München)
   - Give starting gold (500g)

2. **Test Trading**
   - Buy 50 Grain in München
   - Sell it in Venice
   - Verify gold change

3. **Test Production**
   - Build a Brewery in München
   - Run 10 turns
   - Verify beer is produced

4. **Test Events**
   - Fast-forward 20 turns
   - Check for random events

5. **Begin UI Development**
   - See `docs/UI_MOCKUPS.md` (to be created)
   - Start with main HUD widget

---

## 📞 Troubleshooting Help

If compilation fails:
1. **Check Output Log** for specific error
2. **Clean solution** (Build → Clean Solution)
3. **Delete** `Intermediate/`, `Binaries/`, `.vs/` folders
4. **Regenerate** project files
5. **Rebuild** from scratch

**Still stuck?**
- Check `IMPLEMENTATION_STATUS.md` for system status
- Review `DESIGN.md` for intended behavior
- Examine subsystem `.cpp` files for implementation details

---

## ✅ Success Checklist

Before moving to next phase, verify:
- [ ] Project compiles with **0 errors**
- [ ] All 6 subsystems initialize in Output Log
- [ ] Turn processing executes without crashes
- [ ] Settlement DataTable imports 95 cities
- [ ] Price updates work for all settlements
- [ ] Can call NextTurn() without errors
- [ ] Save/load system works (test manually)

**Once all checked = Ready for UI development!**

---

**Next Guide**: See `UI_DEVELOPMENT_GUIDE.md` for creating UMG widgets
