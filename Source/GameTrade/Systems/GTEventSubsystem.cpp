// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTEventSubsystem.h"
#include "Systems/GTTimeSubsystem.h"
#include "Systems/GTEconomySubsystem.h"

void UGTEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Event Subsystem Initialized"));

	InitializeDefaultEvents();
}

void UGTEventSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Event Subsystem Deinitialized"));
}

void UGTEventSubsystem::InitializeDefaultEvents()
{
	// Good Harvest
	FEventTemplate GoodHarvest;
	GoodHarvest.EventType = EEventType::GoodHarvest;
	GoodHarvest.Severity = EEventSeverity::Moderate;
	GoodHarvest.TitleTemplate = TEXT("Bountiful Harvest in {Settlement}");
	GoodHarvest.DescriptionTemplate = TEXT("Favorable weather has resulted in an exceptional harvest. Grain supplies are plentiful!");
	GoodHarvest.BaseChance = 0.10f; // 10% per turn
	GoodHarvest.bCanRepeat = true;
	GoodHarvest.CooldownTurns = 20;
	EventTemplates.Add(EEventType::GoodHarvest, GoodHarvest);

	// Bad Harvest
	FEventTemplate BadHarvest;
	BadHarvest.EventType = EEventType::BadHarvest;
	BadHarvest.Severity = EEventSeverity::Major;
	BadHarvest.TitleTemplate = TEXT("Poor Harvest in {Settlement}");
	BadHarvest.DescriptionTemplate = TEXT("Drought and pests have devastated the crops. Food prices are rising sharply!");
	BadHarvest.BaseChance = 0.08f;
	BadHarvest.bCanRepeat = true;
	BadHarvest.CooldownTurns = 20;
	EventTemplates.Add(EEventType::BadHarvest, BadHarvest);

	// Black Death (historical event)
	FEventTemplate BlackDeath;
	BlackDeath.EventType = EEventType::BlackDeath;
	BlackDeath.Severity = EEventSeverity::Historical;
	BlackDeath.TitleTemplate = TEXT("The Black Death Arrives");
	BlackDeath.DescriptionTemplate = TEXT("A terrible plague sweeps across Europe. Cities are ravaged, and trade routes disrupted. Prepare for dark times ahead...");
	BlackDeath.BaseChance = 0.0f; // Triggered manually at specific year
	BlackDeath.MinimumYear = 1347;
	BlackDeath.MaximumYear = 1351;
	BlackDeath.bCanRepeat = false;
	EventTemplates.Add(EEventType::BlackDeath, BlackDeath);

	// Building For Sale
	FEventTemplate BuildingForSale;
	BuildingForSale.EventType = EEventType::BuildingForSale;
	BuildingForSale.Severity = EEventSeverity::Moderate;
	BuildingForSale.TitleTemplate = TEXT("Business Opportunity in {Settlement}");
	BuildingForSale.DescriptionTemplate = TEXT("A local merchant is selling their {Building} at a discount. This is a rare opportunity!");
	BuildingForSale.BaseChance = 0.05f;
	BuildingForSale.bCanRepeat = true;
	BuildingForSale.CooldownTurns = 10;
	EventTemplates.Add(EEventType::BuildingForSale, BuildingForSale);

	UE_LOG(LogTemp, Log, TEXT("Initialized %d event templates"), EventTemplates.Num());
}

void UGTEventSubsystem::LoadEventTemplatesFromDataTable(UDataTable* EventTable)
{
	if (!EventTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Event DataTable is null"));
		return;
	}

	TArray<FEventTemplate*> Rows;
	EventTable->GetAllRows<FEventTemplate>(TEXT("LoadEvents"), Rows);

	for (FEventTemplate* Row : Rows)
	{
		if (Row)
		{
			EventTemplates.Add(Row->EventType, *Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d event templates from DataTable"), Rows.Num());
}

void UGTEventSubsystem::ProcessEventsTurn()
{
	// Roll for random events
	RollRandomEvents();

	// Check for historical events
	CheckHistoricalEvents();

	// Update cooldowns
	for (auto& CooldownPair : EventCooldowns)
	{
		if (CooldownPair.Value > 0)
		{
			CooldownPair.Value--;
		}
	}
}

void UGTEventSubsystem::RollRandomEvents()
{
	for (const auto& TemplatePair : EventTemplates)
	{
		const FEventTemplate& Template = TemplatePair.Value;

		// Skip historical events (triggered manually)
		if (Template.Severity == EEventSeverity::Historical)
		{
			continue;
		}

		// Check if should trigger
		if (ShouldTriggerEvent(Template))
		{
			TriggerEvent(Template.EventType);
		}
	}
}

void UGTEventSubsystem::CheckHistoricalEvents()
{
	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (!TimeSubsystem)
	{
		return;
	}

	int32 CurrentYear = TimeSubsystem->GetCurrentYear();

	// Black Death (1347-1351)
	if (CurrentYear == 1347 && !EventCooldowns.Contains(EEventType::BlackDeath))
	{
		TriggerHistoricalEvent(EEventType::BlackDeath);
	}

	// TODO: Add more historical events
	// - Hundred Years War events
	// - Printing Press (1450)
	// - Ottoman expansion
}

bool UGTEventSubsystem::ShouldTriggerEvent(const FEventTemplate& Template) const
{
	// Check cooldown
	const int32* Cooldown = EventCooldowns.Find(Template.EventType);
	if (Cooldown && *Cooldown > 0)
	{
		return false;
	}

	// Check year range
	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (TimeSubsystem)
	{
		int32 CurrentYear = TimeSubsystem->GetCurrentYear();
		if (CurrentYear < Template.MinimumYear || CurrentYear > Template.MaximumYear)
		{
			return false;
		}
	}

	// Roll chance
	return FMath::FRand() < Template.BaseChance;
}

FString UGTEventSubsystem::TriggerEvent(EEventType EventType, const FString& SettlementName, const FString& FamilyID)
{
	const FEventTemplate* Template = EventTemplates.Find(EventType);
	if (!Template)
	{
		UE_LOG(LogTemp, Warning, TEXT("No template found for event type"));
		return TEXT("");
	}

	FGameEvent Event;

	// Use specific builders for certain events
	switch (EventType)
	{
	case EEventType::GoodHarvest:
		Event = BuildGoodHarvestEvent(SettlementName);
		break;

	case EEventType::BadHarvest:
		Event = BuildBadHarvestEvent(SettlementName);
		break;

	case EEventType::BuildingForSale:
		Event = BuildBuildingForSaleEvent(SettlementName);
		break;

	default:
		Event = CreateEventFromTemplate(*Template);
		break;
	}

	// Set common fields
	Event.EventID = GenerateEventID();
	Event.AffectedSettlement = SettlementName;
	Event.AffectedFamily = FamilyID;

	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (TimeSubsystem)
	{
		Event.EventDate = TimeSubsystem->GetCurrentDate();
	}

	// Add to active events
	ActiveEvents.Add(Event.EventID, Event);

	// Set cooldown
	EventCooldowns.Add(EventType, Template->CooldownTurns);

	// Broadcast
	OnEventTriggered.Broadcast(Event);

	UE_LOG(LogTemp, Log, TEXT("Event triggered: %s (ID: %s)"), *Event.EventTitle, *Event.EventID);

	return Event.EventID;
}

void UGTEventSubsystem::TriggerHistoricalEvent(EEventType EventType)
{
	FString EventID;

	switch (EventType)
	{
	case EEventType::BlackDeath:
		EventID = TriggerEvent(EventType);
		// Mark as triggered so it doesn't repeat
		EventCooldowns.Add(EventType, 999999);
		break;

	default:
		EventID = TriggerEvent(EventType);
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("Historical event triggered: %s"), *UEnum::GetValueAsString(EventType));
}

bool UGTEventSubsystem::ResolveEvent(const FString& EventID, const FString& ChoiceID)
{
	FGameEvent* Event = ActiveEvents.Find(EventID);
	if (!Event)
	{
		UE_LOG(LogTemp, Warning, TEXT("Event not found: %s"), *EventID);
		return false;
	}

	// Find the choice
	FEventChoice* SelectedChoice = nullptr;
	for (FEventChoice& Choice : Event->Choices)
	{
		if (Choice.ChoiceID == ChoiceID)
		{
			SelectedChoice = &Choice;
			break;
		}
	}

	if (!SelectedChoice)
	{
		UE_LOG(LogTemp, Warning, TEXT("Choice not found: %s"), *ChoiceID);
		return false;
	}

	// Roll for success
	bool bSuccess = FMath::FRand() < SelectedChoice->SuccessChance;

	if (bSuccess)
	{
		// Apply effects
		ApplyEventEffects(SelectedChoice->Effects);
		UE_LOG(LogTemp, Log, TEXT("Event resolved successfully: %s, Choice: %s"), *EventID, *ChoiceID);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Event resolution failed: %s, Choice: %s"), *EventID, *ChoiceID);
		// TODO: Apply failure effects
	}

	// Mark as resolved
	Event->bHasBeenResolved = true;

	// Broadcast
	OnEventResolved.Broadcast(EventID, ChoiceID);

	// Remove from active events
	ActiveEvents.Remove(EventID);

	return true;
}

TArray<FGameEvent> UGTEventSubsystem::GetActiveEvents() const
{
	TArray<FGameEvent> Events;
	ActiveEvents.GenerateValueArray(Events);
	return Events;
}

TArray<FGameEvent> UGTEventSubsystem::GetPendingEvents() const
{
	TArray<FGameEvent> PendingEvents;

	for (const auto& EventPair : ActiveEvents)
	{
		if (EventPair.Value.bRequiresPlayerResponse && !EventPair.Value.bHasBeenResolved)
		{
			PendingEvents.Add(EventPair.Value);
		}
	}

	return PendingEvents;
}

FGameEvent UGTEventSubsystem::GetEvent(const FString& EventID) const
{
	const FGameEvent* Event = ActiveEvents.Find(EventID);
	return Event ? *Event : FGameEvent();
}

bool UGTEventSubsystem::HasPendingEvents() const
{
	for (const auto& EventPair : ActiveEvents)
	{
		if (EventPair.Value.bRequiresPlayerResponse && !EventPair.Value.bHasBeenResolved)
		{
			return true;
		}
	}

	return false;
}

FGameEvent UGTEventSubsystem::CreateEventFromTemplate(const FEventTemplate& Template)
{
	FGameEvent Event;
	Event.EventType = Template.EventType;
	Event.Severity = Template.Severity;
	Event.EventTitle = Template.TitleTemplate;
	Event.EventDescription = Template.DescriptionTemplate;
	Event.bRequiresPlayerResponse = false;

	return Event;
}

void UGTEventSubsystem::ApplyEventEffects(const TArray<FEventEffect>& Effects)
{
	// TODO: Apply effects to game state
	// - Modify gold
	// - Add/remove resources
	// - Change prices
	// - Modify character stats
	// - Change relationships

	for (const FEventEffect& Effect : Effects)
	{
		UE_LOG(LogTemp, Log, TEXT("Applying event effect: %s"), *Effect.EffectDescription);

		// Example: Gold change
		if (Effect.GoldChange != 0.0f)
		{
			// TODO: Modify player gold in save game
			UE_LOG(LogTemp, Log, TEXT("Gold change: %.2f"), Effect.GoldChange);
		}

		// Example: Price change
		if (Effect.PriceMultiplier != 1.0f)
		{
			// TODO: Modify prices in economy subsystem
			UE_LOG(LogTemp, Log, TEXT("Price multiplier: %.2f"), Effect.PriceMultiplier);
		}
	}
}

// Event builders
FGameEvent UGTEventSubsystem::BuildGoodHarvestEvent(const FString& SettlementName)
{
	FGameEvent Event;
	Event.EventType = EEventType::GoodHarvest;
	Event.Severity = EEventSeverity::Moderate;
	Event.EventTitle = FString::Printf(TEXT("Bountiful Harvest in %s"), *SettlementName);
	Event.EventDescription = TEXT("Favorable weather has blessed the region with an exceptional harvest. Grain supplies are plentiful, and prices are falling.");
	Event.bRequiresPlayerResponse = false;

	// Effects: Grain price drops 30%
	FEventEffect Effect;
	Effect.EffectDescription = TEXT("Grain prices drop by 30%");
	Effect.PriceMultiplier = 0.7f;

	FEventChoice AutoChoice;
	AutoChoice.ChoiceID = GenerateEventID();
	AutoChoice.ChoiceText = TEXT("Continue");
	AutoChoice.Effects.Add(Effect);
	AutoChoice.SuccessChance = 1.0f;

	Event.Choices.Add(AutoChoice);

	return Event;
}

FGameEvent UGTEventSubsystem::BuildBadHarvestEvent(const FString& SettlementName)
{
	FGameEvent Event;
	Event.EventType = EEventType::BadHarvest;
	Event.Severity = EEventSeverity::Major;
	Event.EventTitle = FString::Printf(TEXT("Failed Harvest in %s"), *SettlementName);
	Event.EventDescription = TEXT("Drought and pests have devastated the crops. Grain is scarce, and famine threatens the region.");
	Event.bRequiresPlayerResponse = true;

	// Choice 1: Buy grain reserves
	FEventChoice BuyGrain;
	BuyGrain.ChoiceID = GenerateEventID();
	BuyGrain.ChoiceText = TEXT("Buy grain reserves");
	BuyGrain.ChoiceDescription = TEXT("Spend 500 gold to stockpile grain before prices rise further");
	BuyGrain.GoldCost = 500.0f;
	BuyGrain.SuccessChance = 1.0f;

	FEventEffect BuyEffect;
	BuyEffect.EffectDescription = TEXT("Gain 200 units of grain");
	BuyEffect.ResourceChanges.Add(EResourceType::Grain, 200);
	BuyGrain.Effects.Add(BuyEffect);

	Event.Choices.Add(BuyGrain);

	// Choice 2: Do nothing
	FEventChoice DoNothing;
	DoNothing.ChoiceID = GenerateEventID();
	DoNothing.ChoiceText = TEXT("Do nothing");
	DoNothing.ChoiceDescription = TEXT("Wait and see how the situation develops");
	DoNothing.SuccessChance = 1.0f;

	FEventEffect PriceEffect;
	PriceEffect.EffectDescription = TEXT("Grain prices rise by 50%");
	PriceEffect.PriceMultiplier = 1.5f;
	DoNothing.Effects.Add(PriceEffect);

	Event.Choices.Add(DoNothing);

	return Event;
}

FGameEvent UGTEventSubsystem::BuildBlackDeathEvent()
{
	FGameEvent Event;
	Event.EventType = EEventType::BlackDeath;
	Event.Severity = EEventSeverity::Historical;
	Event.EventTitle = TEXT("The Black Death Arrives in Europe");
	Event.EventDescription = TEXT("A terrible plague has begun spreading across the continent. Entire villages are being wiped out. Trade routes are disrupted, and fear grips the land. The death toll is catastrophic.");
	Event.bRequiresPlayerResponse = true;

	// Choice 1: Isolate your family
	FEventChoice Isolate;
	Isolate.ChoiceID = GenerateEventID();
	Isolate.ChoiceText = TEXT("Isolate your family in the countryside");
	Isolate.ChoiceDescription = TEXT("Flee to a remote estate. Safer, but trade will suffer.");
	Isolate.GoldCost = 200.0f;
	Isolate.SuccessChance = 0.9f; // 90% chance family survives

	Event.Choices.Add(Isolate);

	// Choice 2: Continue trading
	FEventChoice ContinueTrading;
	ContinueTrading.ChoiceID = GenerateEventID();
	ContinueTrading.ChoiceText = TEXT("Continue trading");
	ContinueTrading.ChoiceDescription = TEXT("Risk infection but maintain business. High risk, high reward.");
	ContinueTrading.SuccessChance = 0.6f; // 60% survival chance

	FEventEffect ProfitEffect;
	ProfitEffect.EffectDescription = TEXT("Gain 1000 gold from opportunistic trading");
	ProfitEffect.GoldChange = 1000.0f;
	ContinueTrading.Effects.Add(ProfitEffect);

	Event.Choices.Add(ContinueTrading);

	return Event;
}

FGameEvent UGTEventSubsystem::BuildMarriageProposalEvent(const FString& FamilyID)
{
	FGameEvent Event;
	Event.EventType = EEventType::MarriageProposal;
	Event.Severity = EEventSeverity::Moderate;
	Event.EventTitle = TEXT("Marriage Proposal from Rival Family");
	Event.EventDescription = TEXT("The Schmidt family has proposed a marriage alliance. This could end your rivalry and merge your businesses.");
	Event.bRequiresPlayerResponse = true;

	// Choice 1: Accept
	FEventChoice Accept;
	Accept.ChoiceID = GenerateEventID();
	Accept.ChoiceText = TEXT("Accept the proposal");
	Accept.ChoiceDescription = TEXT("Pay 300 gold dowry. Gain alliance and access to their trade routes.");
	Accept.GoldCost = 300.0f;
	Accept.SuccessChance = 1.0f;

	FEventEffect AllianceEffect;
	AllianceEffect.EffectDescription = TEXT("Form alliance with Schmidt family");
	AllianceEffect.ReputationChange = 20;
	Accept.Effects.Add(AllianceEffect);

	Event.Choices.Add(Accept);

	// Choice 2: Reject
	FEventChoice Reject;
	Reject.ChoiceID = GenerateEventID();
	Reject.ChoiceText = TEXT("Reject the proposal");
	Reject.ChoiceDescription = TEXT("Insult them. Rivalry intensifies.");
	Reject.SuccessChance = 1.0f;

	FEventEffect RivalryEffect;
	RivalryEffect.EffectDescription = TEXT("Schmidt family becomes hostile");
	RivalryEffect.ReputationChange = -30;
	Reject.Effects.Add(RivalryEffect);

	Event.Choices.Add(Reject);

	return Event;
}

FGameEvent UGTEventSubsystem::BuildBuildingForSaleEvent(const FString& SettlementName)
{
	FGameEvent Event;
	Event.EventType = EEventType::BuildingForSale;
	Event.Severity = EEventSeverity::Moderate;
	Event.EventTitle = FString::Printf(TEXT("Brewery for Sale in %s"), *SettlementName);
	Event.EventDescription = TEXT("A local brewer is retiring and selling his brewery at 25% below market value. Act quickly before someone else buys it!");
	Event.bRequiresPlayerResponse = true;

	// Choice 1: Buy it
	FEventChoice Buy;
	Buy.ChoiceID = GenerateEventID();
	Buy.ChoiceText = TEXT("Buy the brewery");
	Buy.ChoiceDescription = TEXT("Spend 262 gold (25% discount)");
	Buy.GoldCost = 262.0f; // 350 * 0.75
	Buy.SuccessChance = 1.0f;

	FEventEffect BuildingEffect;
	BuildingEffect.EffectDescription = TEXT("Acquire brewery in ") + SettlementName;
	Buy.Effects.Add(BuildingEffect);

	Event.Choices.Add(Buy);

	// Choice 2: Pass
	FEventChoice Pass;
	Pass.ChoiceID = GenerateEventID();
	Pass.ChoiceText = TEXT("Pass on this opportunity");
	Pass.SuccessChance = 1.0f;

	Event.Choices.Add(Pass);

	return Event;
}

FGameEvent UGTEventSubsystem::BuildRivalBankruptcyEvent(const FString& RivalFamilyID)
{
	FGameEvent Event;
	Event.EventType = EEventType::RivalBankruptcy;
	Event.Severity = EEventSeverity::Major;
	Event.EventTitle = TEXT("Rival Family Declares Bankruptcy!");
	Event.EventDescription = TEXT("The Schmidt family has gone bankrupt! Their assets are being auctioned off at fire-sale prices.");
	Event.bRequiresPlayerResponse = true;

	// Choice 1: Buy their assets
	FEventChoice BuyAssets;
	BuyAssets.ChoiceID = GenerateEventID();
	BuyAssets.ChoiceText = TEXT("Buy their workshops");
	BuyAssets.ChoiceDescription = TEXT("Spend 800 gold to acquire 3 breweries");
	BuyAssets.GoldCost = 800.0f;
	BuyAssets.SuccessChance = 1.0f;

	Event.Choices.Add(BuyAssets);

	// Choice 2: Help them
	FEventChoice Help;
	Help.ChoiceID = GenerateEventID();
	Help.ChoiceText = TEXT("Offer them a loan");
	Help.ChoiceDescription = TEXT("Lend them 500 gold. Gain massive reputation, potential ally");
	Help.GoldCost = 500.0f;
	Help.SuccessChance = 1.0f;

	FEventEffect ReputationEffect;
	ReputationEffect.EffectDescription = TEXT("Gain 50 reputation for generosity");
	ReputationEffect.ReputationChange = 50;
	Help.Effects.Add(ReputationEffect);

	Event.Choices.Add(Help);

	return Event;
}

FString UGTEventSubsystem::GenerateEventID() const
{
	return FGuid::NewGuid().ToString();
}
