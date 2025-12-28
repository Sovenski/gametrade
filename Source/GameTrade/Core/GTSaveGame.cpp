// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Core/GTSaveGame.h"

UGTSaveGame::UGTSaveGame()
{
	SaveSlotName = TEXT("SaveSlot1");
	SaveIndex = 0;
	Gold = 50.0f; // Starting gold
	TotalTurnsPlayed = 0;
	TotalGoldEarned = 0.0f;
	TotalTradesMade = 0;
	BuildingsOwned = 0;
}
