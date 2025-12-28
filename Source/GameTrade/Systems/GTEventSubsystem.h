// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTEventTypes.h"
#include "Engine/DataTable.h"
#include "GTEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventTriggered, const FGameEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventResolved, const FString&, EventID, const FString&, ChoiceID);

/**
 * Manages random events and historical events
 * - Event generation and triggering
 * - Player choices and consequences
 * - Historical events timeline
 * - Event cooldowns and conditions
 */
UCLASS()
class GAMETRADE_API UGTEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Event templates
	UFUNCTION(BlueprintCallable, Category = "Events")
	void LoadEventTemplatesFromDataTable(UDataTable* EventTable);

	// Turn processing
	UFUNCTION(BlueprintCallable, Category = "Events")
	void ProcessEventsTurn();

	// Event triggering
	UFUNCTION(BlueprintCallable, Category = "Events")
	FString TriggerEvent(EEventType EventType, const FString& SettlementName = TEXT(""), const FString& FamilyID = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Events")
	void TriggerHistoricalEvent(EEventType EventType);

	// Event resolution
	UFUNCTION(BlueprintCallable, Category = "Events")
	bool ResolveEvent(const FString& EventID, const FString& ChoiceID);

	// Queries
	UFUNCTION(BlueprintPure, Category = "Events")
	TArray<FGameEvent> GetActiveEvents() const;

	UFUNCTION(BlueprintPure, Category = "Events")
	TArray<FGameEvent> GetPendingEvents() const; // Requires player response

	UFUNCTION(BlueprintPure, Category = "Events")
	FGameEvent GetEvent(const FString& EventID) const;

	UFUNCTION(BlueprintPure, Category = "Events")
	bool HasPendingEvents() const;

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEventTriggered OnEventTriggered;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEventResolved OnEventResolved;

protected:
	// Data
	UPROPERTY()
	TMap<FString, FGameEvent> ActiveEvents;

	UPROPERTY()
	TMap<EEventType, FEventTemplate> EventTemplates;

	UPROPERTY()
	TMap<EEventType, int32> EventCooldowns; // Tracks when events can occur again

	// Helper functions
	void InitializeDefaultEvents();
	void RollRandomEvents();
	void CheckHistoricalEvents();
	bool ShouldTriggerEvent(const FEventTemplate& Template) const;
	FGameEvent CreateEventFromTemplate(const FEventTemplate& Template);
	void ApplyEventEffects(const TArray<FEventEffect>& Effects);
	FString GenerateEventID() const;

	// Event builders
	FGameEvent BuildGoodHarvestEvent(const FString& SettlementName);
	FGameEvent BuildBadHarvestEvent(const FString& SettlementName);
	FGameEvent BuildBlackDeathEvent();
	FGameEvent BuildMarriageProposalEvent(const FString& FamilyID);
	FGameEvent BuildBuildingForSaleEvent(const FString& SettlementName);
	FGameEvent BuildRivalBankruptcyEvent(const FString& RivalFamilyID);
};
