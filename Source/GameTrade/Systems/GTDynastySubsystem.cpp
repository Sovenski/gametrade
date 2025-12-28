// Copyright Medieval Trading Dynasty. All Rights Reserved.

#include "Systems/GTDynastySubsystem.h"
#include "Systems/GTTimeSubsystem.h"

void UGTDynastySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Dynasty Subsystem Initialized"));
}

void UGTDynastySubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("Dynasty Subsystem Deinitialized"));
}

FString UGTDynastySubsystem::CreateCharacter(const FString& FirstName, const FString& FamilyName, bool bIsMale, int32 Age)
{
	FCharacterData NewCharacter;
	NewCharacter.CharacterID = GenerateCharacterID();
	NewCharacter.FirstName = FirstName;
	NewCharacter.FamilyName = FamilyName;
	NewCharacter.bIsMale = bIsMale;
	NewCharacter.Age = Age;
	NewCharacter.bIsAlive = true;

	// Generate random starting stats (1-3)
	NewCharacter.Stats.Commerce = FMath::RandRange(1, 3);
	NewCharacter.Stats.Stewardship = FMath::RandRange(1, 3);
	NewCharacter.Stats.Intrigue = FMath::RandRange(1, 3);
	NewCharacter.Stats.Leadership = FMath::RandRange(1, 3);
	NewCharacter.Stats.Combat = FMath::RandRange(1, 3);

	// Generate 1-2 random traits
	GenerateRandomTraits(NewCharacter, FMath::RandRange(1, 2));

	Characters.Add(NewCharacter.CharacterID, NewCharacter);

	UE_LOG(LogTemp, Log, TEXT("Created character: %s (ID: %s, Age: %d)"),
		*NewCharacter.GetFullName(), *NewCharacter.CharacterID, Age);

	return NewCharacter.CharacterID;
}

bool UGTDynastySubsystem::ProposeMarriage(const FString& ProposerID, const FString& TargetID, float DowryOffered)
{
	if (!CanMarry(ProposerID, TargetID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot propose marriage between %s and %s"), *ProposerID, *TargetID);
		return false;
	}

	// Calculate success chance
	float SuccessChance = CalculateMarriageSuccess(ProposerID, TargetID);

	// Dowry increases success chance
	SuccessChance += (DowryOffered / 1000.0f) * 0.1f; // +10% per 1000 gold

	float Roll = FMath::FRand();

	if (Roll < SuccessChance)
	{
		OnMarriageProposed.Broadcast(ProposerID, TargetID);
		return AcceptMarriage(ProposerID, TargetID);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Marriage proposal rejected (chance: %.1f%%, roll: %.1f%%)"),
			SuccessChance * 100.0f, Roll * 100.0f);
		return false;
	}
}

bool UGTDynastySubsystem::AcceptMarriage(const FString& ProposerID, const FString& TargetID)
{
	FCharacterData* Proposer = Characters.Find(ProposerID);
	FCharacterData* Target = Characters.Find(TargetID);

	if (!Proposer || !Target)
	{
		return false;
	}

	// Set as married
	Proposer->SpouseID = TargetID;
	Target->SpouseID = ProposerID;

	OnMarriageAccepted.Broadcast(ProposerID, TargetID, 0.0f); // TODO: Pass actual dowry

	UE_LOG(LogTemp, Log, TEXT("%s married %s"),
		*Proposer->GetFullName(), *Target->GetFullName());

	return true;
}

bool UGTDynastySubsystem::DivorceMarriage(const FString& Character1ID, const FString& Character2ID)
{
	FCharacterData* Char1 = Characters.Find(Character1ID);
	FCharacterData* Char2 = Characters.Find(Character2ID);

	if (!Char1 || !Char2)
	{
		return false;
	}

	if (Char1->SpouseID != Character2ID || Char2->SpouseID != Character1ID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Characters are not married to each other"));
		return false;
	}

	// Divorce
	Char1->SpouseID = TEXT("");
	Char2->SpouseID = TEXT("");

	UE_LOG(LogTemp, Log, TEXT("%s divorced %s"),
		*Char1->GetFullName(), *Char2->GetFullName());

	return true;
}

FString UGTDynastySubsystem::GiveBirth(const FString& MotherID, const FString& FatherID)
{
	FCharacterData* Mother = Characters.Find(MotherID);
	FCharacterData* Father = Characters.Find(FatherID);

	if (!Mother || !Father)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid parent IDs for birth"));
		return TEXT("");
	}

	// Random gender
	bool bIsMale = FMath::RandBool();

	// Random name
	FString FirstName;
	if (bIsMale)
	{
		FirstName = MaleNames[FMath::RandRange(0, MaleNames.Num() - 1)];
	}
	else
	{
		FirstName = FemaleNames[FMath::RandRange(0, FemaleNames.Num() - 1)];
	}

	// Create child
	FCharacterData Child;
	Child.CharacterID = GenerateCharacterID();
	Child.FirstName = FirstName;
	Child.FamilyName = Father->FamilyName; // Patrilineal naming
	Child.bIsMale = bIsMale;
	Child.Age = 0;
	Child.bIsAlive = true;
	Child.CurrentSettlement = Mother->CurrentSettlement;

	// Inherit traits from parents
	InheritTraits(Child, *Mother, *Father);

	// Add to family
	Mother->ChildrenIDs.Add(Child.CharacterID);
	Father->ChildrenIDs.Add(Child.CharacterID);

	Characters.Add(Child.CharacterID, Child);

	OnCharacterBorn.Broadcast(Child.CharacterID, MotherID);

	UE_LOG(LogTemp, Log, TEXT("%s born to %s and %s"),
		*Child.GetFullName(), *Mother->GetFullName(), *Father->GetFullName());

	return Child.CharacterID;
}

void UGTDynastySubsystem::ProcessAgingTurn()
{
	UGTTimeSubsystem* TimeSubsystem = GetGameInstance()->GetSubsystem<UGTTimeSubsystem>();
	if (!TimeSubsystem)
	{
		return;
	}

	// Age characters every 52 weeks (1 year)
	if (TimeSubsystem->GetCurrentWeek() != 1)
	{
		return; // Only age on week 1 of each year
	}

	TArray<FString> DeadCharacters;

	for (auto& CharacterPair : Characters)
	{
		FCharacterData& Character = CharacterPair.Value;

		if (!Character.bIsAlive)
		{
			continue;
		}

		// Age character
		AgeCharacter(Character);

		// Check for coming of age
		if (Character.Age == 16)
		{
			OnCharacterComingOfAge.Broadcast(Character.CharacterID);
			UE_LOG(LogTemp, Log, TEXT("%s has come of age!"), *Character.GetFullName());
		}

		// Roll for death
		if (RollDeathChance(Character))
		{
			DeadCharacters.Add(Character.CharacterID);
		}

		// Chance of birth if married and of childbearing age
		if (!Character.SpouseID.IsEmpty() && !Character.bIsMale && Character.Age >= 18 && Character.Age <= 45)
		{
			// 20% chance of pregnancy per year
			if (FMath::FRand() < 0.2f)
			{
				GiveBirth(Character.CharacterID, Character.SpouseID);
			}
		}
	}

	// Handle deaths
	for (const FString& CharacterID : DeadCharacters)
	{
		KillCharacter(CharacterID, TEXT("Natural causes"));
	}
}

void UGTDynastySubsystem::AgeCharacter(FCharacterData& Character)
{
	Character.Age++;

	// Skill development based on age
	if (Character.Age >= 16 && Character.Age <= 35)
	{
		// Prime learning years - small chance to improve skills
		if (FMath::FRand() < 0.1f) // 10% chance
		{
			// Improve random skill
			int32 SkillRoll = FMath::RandRange(0, 4);
			switch (SkillRoll)
			{
			case 0: if (Character.Stats.Commerce < 10) Character.Stats.Commerce++; break;
			case 1: if (Character.Stats.Stewardship < 10) Character.Stats.Stewardship++; break;
			case 2: if (Character.Stats.Intrigue < 10) Character.Stats.Intrigue++; break;
			case 3: if (Character.Stats.Leadership < 10) Character.Stats.Leadership++; break;
			case 4: if (Character.Stats.Combat < 10) Character.Stats.Combat++; break;
			}
		}
	}
	else if (Character.Age > 60)
	{
		// Old age - skills may decline
		if (FMath::FRand() < 0.05f) // 5% chance
		{
			// Random skill decreases
			int32 SkillRoll = FMath::RandRange(0, 4);
			switch (SkillRoll)
			{
			case 0: if (Character.Stats.Commerce > 0) Character.Stats.Commerce--; break;
			case 1: if (Character.Stats.Stewardship > 0) Character.Stats.Stewardship--; break;
			case 2: if (Character.Stats.Intrigue > 0) Character.Stats.Intrigue--; break;
			case 3: if (Character.Stats.Leadership > 0) Character.Stats.Leadership--; break;
			case 4: if (Character.Stats.Combat > 0) Character.Stats.Combat--; break;
			}
		}
	}
}

bool UGTDynastySubsystem::RollDeathChance(const FCharacterData& Character) const
{
	float DeathChance = 0.0f;

	// Age-based death chance
	if (Character.Age < 5)
	{
		DeathChance = 0.05f; // 5% infant mortality
	}
	else if (Character.Age < 50)
	{
		DeathChance = 0.01f; // 1% base chance
	}
	else if (Character.Age < 60)
	{
		DeathChance = 0.02f; // 2%
	}
	else if (Character.Age < 70)
	{
		DeathChance = 0.05f; // 5%
	}
	else if (Character.Age < 80)
	{
		DeathChance = 0.10f; // 10%
	}
	else
	{
		DeathChance = 0.20f; // 20%
	}

	// TODO: Modifiers
	// - Plague events (+40%)
	// - In combat (+varies)
	// - Poor health trait (+5%)

	return FMath::FRand() < DeathChance;
}

bool UGTDynastySubsystem::KillCharacter(const FString& CharacterID, const FString& CauseOfDeath)
{
	FCharacterData* Character = Characters.Find(CharacterID);
	if (!Character)
	{
		return false;
	}

	Character->bIsAlive = false;

	OnCharacterDied.Broadcast(CharacterID, Character->Age, CauseOfDeath);

	UE_LOG(LogTemp, Log, TEXT("%s died at age %d (%s)"),
		*Character->GetFullName(), Character->Age, *CauseOfDeath);

	// TODO: Handle succession if this was player character
	// TODO: Handle inheritance

	return true;
}

void UGTDynastySubsystem::ImproveSkill(const FString& CharacterID, const FString& SkillName, int32 Amount)
{
	FCharacterData* Character = Characters.Find(CharacterID);
	if (!Character)
	{
		return;
	}

	if (SkillName == TEXT("Commerce"))
	{
		Character->Stats.Commerce = FMath::Min(10, Character->Stats.Commerce + Amount);
	}
	else if (SkillName == TEXT("Stewardship"))
	{
		Character->Stats.Stewardship = FMath::Min(10, Character->Stats.Stewardship + Amount);
	}
	else if (SkillName == TEXT("Intrigue"))
	{
		Character->Stats.Intrigue = FMath::Min(10, Character->Stats.Intrigue + Amount);
	}
	else if (SkillName == TEXT("Leadership"))
	{
		Character->Stats.Leadership = FMath::Min(10, Character->Stats.Leadership + Amount);
	}
	else if (SkillName == TEXT("Combat"))
	{
		Character->Stats.Combat = FMath::Min(10, Character->Stats.Combat + Amount);
	}

	UE_LOG(LogTemp, Log, TEXT("%s improved %s by %d"),
		*Character->GetFullName(), *SkillName, Amount);
}

bool UGTDynastySubsystem::AddTrait(const FString& CharacterID, ECharacterTrait Trait)
{
	FCharacterData* Character = Characters.Find(CharacterID);
	if (!Character)
	{
		return false;
	}

	if (Character->Traits.Contains(Trait))
	{
		return false; // Already has trait
	}

	Character->Traits.Add(Trait);
	UE_LOG(LogTemp, Log, TEXT("%s gained trait: %s"),
		*Character->GetFullName(), *UEnum::GetValueAsString(Trait));

	return true;
}

bool UGTDynastySubsystem::RemoveTrait(const FString& CharacterID, ECharacterTrait Trait)
{
	FCharacterData* Character = Characters.Find(CharacterID);
	if (!Character)
	{
		return false;
	}

	Character->Traits.Remove(Trait);
	return true;
}

void UGTDynastySubsystem::EducateChild(const FString& CharacterID, const FString& Focus)
{
	// TODO: Implement education system
	// - Improve skills based on focus over time
	// - Influenced by educator's skills
	UE_LOG(LogTemp, Log, TEXT("Educating character %s with focus: %s"), *CharacterID, *Focus);
}

FCharacterData UGTDynastySubsystem::GetCharacter(const FString& CharacterID) const
{
	const FCharacterData* Character = Characters.Find(CharacterID);
	return Character ? *Character : FCharacterData();
}

TArray<FCharacterData> UGTDynastySubsystem::GetFamilyMembers(const FString& FamilyName) const
{
	TArray<FCharacterData> Members;

	for (const auto& CharacterPair : Characters)
	{
		if (CharacterPair.Value.FamilyName == FamilyName && CharacterPair.Value.bIsAlive)
		{
			Members.Add(CharacterPair.Value);
		}
	}

	return Members;
}

TArray<FCharacterData> UGTDynastySubsystem::GetChildren(const FString& ParentID) const
{
	TArray<FCharacterData> Children;

	const FCharacterData* Parent = Characters.Find(ParentID);
	if (!Parent)
	{
		return Children;
	}

	for (const FString& ChildID : Parent->ChildrenIDs)
	{
		const FCharacterData* Child = Characters.Find(ChildID);
		if (Child && Child->bIsAlive)
		{
			Children.Add(*Child);
		}
	}

	return Children;
}

bool UGTDynastySubsystem::IsMarried(const FString& CharacterID) const
{
	const FCharacterData* Character = Characters.Find(CharacterID);
	return Character && !Character->SpouseID.IsEmpty();
}

bool UGTDynastySubsystem::CanMarry(const FString& Character1ID, const FString& Character2ID) const
{
	const FCharacterData* Char1 = Characters.Find(Character1ID);
	const FCharacterData* Char2 = Characters.Find(Character2ID);

	if (!Char1 || !Char2)
	{
		return false;
	}

	// Both must be alive
	if (!Char1->bIsAlive || !Char2->bIsAlive)
	{
		return false;
	}

	// Neither can be married
	if (!Char1->SpouseID.IsEmpty() || !Char2->SpouseID.IsEmpty())
	{
		return false;
	}

	// Must be of age (16+)
	if (Char1->Age < 16 || Char2->Age < 16)
	{
		return false;
	}

	// Must be different genders (medieval period, historically accurate)
	if (Char1->bIsMale == Char2->bIsMale)
	{
		return false;
	}

	// TODO: Check for family relations (no incest)

	return true;
}

int32 UGTDynastySubsystem::GetSkillValue(const FString& CharacterID, const FString& SkillName) const
{
	const FCharacterData* Character = Characters.Find(CharacterID);
	if (!Character)
	{
		return 0;
	}

	if (SkillName == TEXT("Commerce"))
	{
		return Character->Stats.Commerce;
	}
	else if (SkillName == TEXT("Stewardship"))
	{
		return Character->Stats.Stewardship;
	}
	else if (SkillName == TEXT("Intrigue"))
	{
		return Character->Stats.Intrigue;
	}
	else if (SkillName == TEXT("Leadership"))
	{
		return Character->Stats.Leadership;
	}
	else if (SkillName == TEXT("Combat"))
	{
		return Character->Stats.Combat;
	}

	return 0;
}

bool UGTDynastySubsystem::HasTrait(const FString& CharacterID, ECharacterTrait Trait) const
{
	const FCharacterData* Character = Characters.Find(CharacterID);
	return Character && Character->Traits.Contains(Trait);
}

FString UGTDynastySubsystem::DetermineHeir(const FString& DeceasedID)
{
	const FCharacterData* Deceased = Characters.Find(DeceasedID);
	if (!Deceased || Deceased->ChildrenIDs.Num() == 0)
	{
		return TEXT("");
	}

	// Simple primogeniture - eldest child
	FString EldestID;
	int32 OldestAge = -1;

	for (const FString& ChildID : Deceased->ChildrenIDs)
	{
		const FCharacterData* Child = Characters.Find(ChildID);
		if (Child && Child->bIsAlive && Child->Age > OldestAge)
		{
			OldestAge = Child->Age;
			EldestID = ChildID;
		}
	}

	return EldestID;
}

void UGTDynastySubsystem::InheritTraits(FCharacterData& Child, const FCharacterData& Parent1, const FCharacterData& Parent2)
{
	// 25% chance to inherit each parent's traits
	for (ECharacterTrait Trait : Parent1.Traits)
	{
		if (FMath::FRand() < 0.25f)
		{
			Child.Traits.AddUnique(Trait);
		}
	}

	for (ECharacterTrait Trait : Parent2.Traits)
	{
		if (FMath::FRand() < 0.25f)
		{
			Child.Traits.AddUnique(Trait);
		}
	}

	// Base stats influenced by parents (average + variance)
	Child.Stats.Commerce = FMath::Clamp((Parent1.Stats.Commerce + Parent2.Stats.Commerce) / 2 + FMath::RandRange(-1, 1), 1, 5);
	Child.Stats.Stewardship = FMath::Clamp((Parent1.Stats.Stewardship + Parent2.Stats.Stewardship) / 2 + FMath::RandRange(-1, 1), 1, 5);
	Child.Stats.Intrigue = FMath::Clamp((Parent1.Stats.Intrigue + Parent2.Stats.Intrigue) / 2 + FMath::RandRange(-1, 1), 1, 5);
	Child.Stats.Leadership = FMath::Clamp((Parent1.Stats.Leadership + Parent2.Stats.Leadership) / 2 + FMath::RandRange(-1, 1), 1, 5);
	Child.Stats.Combat = FMath::Clamp((Parent1.Stats.Combat + Parent2.Stats.Combat) / 2 + FMath::RandRange(-1, 1), 1, 5);
}

float UGTDynastySubsystem::CalculateMarriageSuccess(const FString& ProposerID, const FString& TargetID) const
{
	// Base 50% chance
	float Chance = 0.5f;

	const FCharacterData* Proposer = Characters.Find(ProposerID);
	if (!Proposer)
	{
		return 0.0f;
	}

	// Charisma increases chance
	if (Proposer->Traits.Contains(ECharacterTrait::Charismatic))
	{
		Chance += 0.2f;
	}

	// Leadership skill increases chance
	Chance += Proposer->Stats.Leadership * 0.03f; // +3% per leadership point

	return FMath::Clamp(Chance, 0.1f, 0.95f); // Between 10% and 95%
}

void UGTDynastySubsystem::GenerateRandomTraits(FCharacterData& Character, int32 NumTraits)
{
	TArray<ECharacterTrait> PossibleTraits = {
		ECharacterTrait::Genius,
		ECharacterTrait::Charismatic,
		ECharacterTrait::Diligent,
		ECharacterTrait::Brave,
		ECharacterTrait::Patient,
		ECharacterTrait::Shrewd,
		ECharacterTrait::Slow,
		ECharacterTrait::Greedy,
		ECharacterTrait::Wrathful,
		ECharacterTrait::Craven,
		ECharacterTrait::Slothful,
		ECharacterTrait::Trusting,
		ECharacterTrait::Ambitious,
		ECharacterTrait::Cynical,
		ECharacterTrait::Zealous
	};

	for (int32 i = 0; i < NumTraits; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, PossibleTraits.Num() - 1);
		Character.Traits.AddUnique(PossibleTraits[RandomIndex]);
	}
}

FString UGTDynastySubsystem::GenerateCharacterID() const
{
	return FGuid::NewGuid().ToString();
}
