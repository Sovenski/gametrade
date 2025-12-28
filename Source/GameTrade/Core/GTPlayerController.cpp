// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Core/GTPlayerController.h"
#include "Core/GTGameMode.h"
#include "Core/GTGameInstance.h"
#include "Systems/GTEconomySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AGTPlayerController::AGTPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AGTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set input mode
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("GTPlayerController BeginPlay"));
}

void AGTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind basic inputs (can be enhanced with Enhanced Input System)
	if (InputComponent)
	{
		// TODO: Bind input actions for next turn, pause, etc.
	}
}

bool AGTPlayerController::TradeGoods(const FString& Settlement, EResourceType Resource, int32 Amount, bool bBuying)
{
	UGTGameInstance* GameInstance = Cast<UGTGameInstance>(GetGameInstance());
	if (!GameInstance) return false;

	UGTEconomySubsystem* EconomySubsystem = GameInstance->GetSubsystem<UGTEconomySubsystem>();
	if (!EconomySubsystem) return false;

	UGTSaveGame* SaveGame = GameInstance->GetCurrentSave();
	if (!SaveGame) return false;

	float Cost = 0.0f;

	if (bBuying)
	{
		// Buy goods
		if (!EconomySubsystem->BuyGoods(Settlement, Resource, Amount, Cost))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to buy goods"));
			return false;
		}

		// Check if player has enough money
		if (SaveGame->Gold < Cost)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough gold! Need %.2f, have %.2f"), Cost, SaveGame->Gold);
			return false;
		}

		// Deduct gold and add to inventory
		SaveGame->Gold -= Cost;

		// Add to player inventory
		bool bFoundInInventory = false;
		for (FResourceAmount& Item : SaveGame->Inventory)
		{
			if (Item.ResourceType == Resource)
			{
				Item.Amount += Amount;
				bFoundInInventory = true;
				break;
			}
		}

		if (!bFoundInInventory)
		{
			SaveGame->Inventory.Add(FResourceAmount(Resource, Amount));
		}

		UE_LOG(LogTemp, Log, TEXT("Bought %d %s for %.2f gold"), Amount, *UEnum::GetValueAsString(Resource), Cost);
	}
	else
	{
		// Sell goods
		// Check if player has enough in inventory
		bool bHasGoods = false;
		for (FResourceAmount& Item : SaveGame->Inventory)
		{
			if (Item.ResourceType == Resource && Item.Amount >= Amount)
			{
				bHasGoods = true;

				if (!EconomySubsystem->SellGoods(Settlement, Resource, Amount, Cost))
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to sell goods"));
					return false;
				}

				// Remove from inventory and add gold
				Item.Amount -= Amount;
				SaveGame->Gold += Cost;

				UE_LOG(LogTemp, Log, TEXT("Sold %d %s for %.2f gold"), Amount, *UEnum::GetValueAsString(Resource), Cost);
				break;
			}
		}

		if (!bHasGoods)
		{
			UE_LOG(LogTemp, Warning, TEXT("Don't have enough %s to sell"), *UEnum::GetValueAsString(Resource));
			return false;
		}
	}

	// Update stats
	SaveGame->TotalTradesMade++;
	if (!bBuying)
	{
		SaveGame->TotalGoldEarned += Cost;
	}

	return true;
}

bool AGTPlayerController::SendCaravan(const FString& FromSettlement, const FString& ToSettlement, const TArray<FResourceAmount>& Cargo)
{
	// TODO: Implement caravan system
	UE_LOG(LogTemp, Warning, TEXT("Caravan system not yet implemented"));
	return false;
}

bool AGTPlayerController::BuildBuilding(const FString& Settlement, EBuildingType BuildingType)
{
	// TODO: Implement building construction
	UE_LOG(LogTemp, Warning, TEXT("Building system not yet implemented"));
	return false;
}

float AGTPlayerController::GetPlayerGold() const
{
	UGTGameInstance* GameInstance = Cast<UGTGameInstance>(GetGameInstance());
	if (!GameInstance) return 0.0f;

	UGTSaveGame* SaveGame = GameInstance->GetCurrentSave();
	return SaveGame ? SaveGame->Gold : 0.0f;
}

FCharacterData AGTPlayerController::GetPlayerCharacter() const
{
	UGTGameInstance* GameInstance = Cast<UGTGameInstance>(GetGameInstance());
	if (!GameInstance) return FCharacterData();

	UGTSaveGame* SaveGame = GameInstance->GetCurrentSave();
	if (!SaveGame) return FCharacterData();

	const FCharacterData* Character = SaveGame->Characters.Find(SaveGame->PlayerCharacterID);
	return Character ? *Character : FCharacterData();
}

TArray<FResourceAmount> AGTPlayerController::GetPlayerInventory() const
{
	UGTGameInstance* GameInstance = Cast<UGTGameInstance>(GetGameInstance());
	if (!GameInstance) return TArray<FResourceAmount>();

	UGTSaveGame* SaveGame = GameInstance->GetCurrentSave();
	return SaveGame ? SaveGame->Inventory : TArray<FResourceAmount>();
}

void AGTPlayerController::OnNextTurnPressed()
{
	AGTGameMode* GameMode = Cast<AGTGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->NextTurn();
	}
}

void AGTPlayerController::OnPausePressed()
{
	AGTGameMode* GameMode = Cast<AGTGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		if (GameMode->IsGamePaused())
		{
			GameMode->ResumeGame();
		}
		else
		{
			GameMode->PauseGame();
		}
	}
}

void AGTPlayerController::OnOpenMapPressed()
{
	// TODO: Open map UI
	UE_LOG(LogTemp, Log, TEXT("Open map pressed"));
}
