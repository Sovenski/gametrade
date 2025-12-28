// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "GameTrade.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FGameTradeModule, GameTrade, "GameTrade");

void FGameTradeModule::StartupModule()
{
	// Custom startup code
	UE_LOG(LogTemp, Log, TEXT("GameTrade Module Started"));
}

void FGameTradeModule::ShutdownModule()
{
	// Custom shutdown code
	UE_LOG(LogTemp, Log, TEXT("GameTrade Module Shutdown"));
}
