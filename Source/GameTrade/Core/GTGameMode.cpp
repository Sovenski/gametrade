// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Core/GTGameMode.h"
#include "Core/GTPlayerController.h"
#include "Systems/GTTimeSubsystem.h"
#include "Systems/GTEconomySubsystem.h"
#include "Systems/GTProductionSubsystem.h"
#include "Systems/GTTradeSubsystem.h"
#include "Systems/GTDynastySubsystem.h"
#include "Systems/GTEventSubsystem.h"
#include "Systems/GTHierarchicalEconomySubsystem.h"
#include "Systems/GTWorldMapSubsystem.h"
#include "Kismet/GameplayStatics.h"

AGTGameMode::AGTGameMode()
{
	// Set default classes
	PlayerControllerClass = AGTPlayerController::StaticClass();
	bGamePaused = false;
}

void AGTGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("GTGameMode BeginPlay"));

	// Get subsystems
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	UGTTimeSubsystem* TimeSubsystem = GameInstance->GetSubsystem<UGTTimeSubsystem>();
	UGTEconomySubsystem* EconomySubsystem = GameInstance->GetSubsystem<UGTEconomySubsystem>();
	UGTHierarchicalEconomySubsystem* HierarchyEconomy = GameInstance->GetSubsystem<UGTHierarchicalEconomySubsystem>();
	UGTWorldMapSubsystem* WorldMap = GameInstance->GetSubsystem<UGTWorldMapSubsystem>();

	// Initialize world map with real geography
	if (WorldMap)
	{
		WorldMap->InitializeWorldMap(); // Load cities, rivers, trade routes
		WorldMap->IntegrateWithHierarchicalEconomy(); // Create holdings from real cities
	}

	// Bind to time events
	if (TimeSubsystem)
	{
		TimeSubsystem->OnTurnAdvanced.AddDynamic(this, &AGTGameMode::OnTurnAdvanced);
		TimeSubsystem->OnSeasonChanged.AddDynamic(this, &AGTGameMode::OnSeasonChanged);
		TimeSubsystem->OnYearChanged.AddDynamic(this, &AGTGameMode::OnYearChanged);
	}

	// Initialize economy (load from DataTable in Blueprint)
	if (EconomySubsystem)
	{
		EconomySubsystem->InitializeEconomy();
	}

	// Initialize hierarchical economy (CK3-style settlements)
	if (HierarchyEconomy)
	{
		HierarchyEconomy->BuildHierarchyFromLegacyData();
		HierarchyEconomy->SpawnAIMerchantDynasties(4); // Spawn 4 AI merchant families
	}
}

void AGTGameMode::NextTurn()
{
	if (bGamePaused)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game is paused, cannot advance turn"));
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	// Get all subsystems
	UGTTimeSubsystem* TimeSubsystem = GameInstance->GetSubsystem<UGTTimeSubsystem>();
	UGTEconomySubsystem* EconomySubsystem = GameInstance->GetSubsystem<UGTEconomySubsystem>();
	UGTProductionSubsystem* ProductionSubsystem = GameInstance->GetSubsystem<UGTProductionSubsystem>();
	UGTTradeSubsystem* TradeSubsystem = GameInstance->GetSubsystem<UGTTradeSubsystem>();
	UGTDynastySubsystem* DynastySubsystem = GameInstance->GetSubsystem<UGTDynastySubsystem>();
	UGTEventSubsystem* EventSubsystem = GameInstance->GetSubsystem<UGTEventSubsystem>();
	UGTHierarchicalEconomySubsystem* HierarchyEconomy = GameInstance->GetSubsystem<UGTHierarchicalEconomySubsystem>();

	UE_LOG(LogTemp, Log, TEXT("=== TURN PROCESSING START ==="));

	// 1. Advance time
	if (TimeSubsystem)
	{
		TimeSubsystem->AdvanceTurn();
	}

	// 2. Process production buildings
	if (ProductionSubsystem)
	{
		ProductionSubsystem->ProcessProductionTurn();
	}

	// 3. Process hierarchical economy (new CK3-style system)
	// This handles: passive trade, population growth, AI merchants, layered economies
	if (HierarchyEconomy)
	{
		HierarchyEconomy->ProcessHierarchicalEconomyTurn();
	}

	// 4. Process caravan travel (legacy system - still used for long-distance player routes)
	if (TradeSubsystem)
	{
		TradeSubsystem->ProcessCaravansTurn();
	}

	// 5. Process economy (legacy supply/demand/prices - now complemented by hierarchical system)
	if (EconomySubsystem)
	{
		EconomySubsystem->ProcessEconomyTurn();
	}

	// 6. Process dynasty (aging, births, deaths)
	if (DynastySubsystem)
	{
		DynastySubsystem->ProcessAgingTurn();
	}

	// 7. Process/generate events
	if (EventSubsystem)
	{
		EventSubsystem->ProcessEventsTurn();
	}

	UE_LOG(LogTemp, Log, TEXT("=== TURN PROCESSING COMPLETE ==="));
}

void AGTGameMode::FastForward(int32 NumTurns)
{
	if (NumTurns <= 0) return;

	UE_LOG(LogTemp, Log, TEXT("Fast forwarding %d turns..."), NumTurns);

	for (int32 i = 0; i < NumTurns; i++)
	{
		NextTurn();
	}

	UE_LOG(LogTemp, Log, TEXT("Fast forward complete"));
}

void AGTGameMode::PauseGame()
{
	bGamePaused = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UE_LOG(LogTemp, Log, TEXT("Game paused"));
}

void AGTGameMode::ResumeGame()
{
	bGamePaused = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UE_LOG(LogTemp, Log, TEXT("Game resumed"));
}

void AGTGameMode::OnTurnAdvanced(const FGameDate& NewDate)
{
	UE_LOG(LogTemp, Log, TEXT("Turn advanced to: %s"), *NewDate.ToString());
}

void AGTGameMode::OnSeasonChanged(ESeason NewSeason)
{
	UE_LOG(LogTemp, Log, TEXT("Season changed to: %s"), *UEnum::GetValueAsString(NewSeason));
}

void AGTGameMode::OnYearChanged(int32 NewYear)
{
	UE_LOG(LogTemp, Log, TEXT("New year: %d"), NewYear);
}
