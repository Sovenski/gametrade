// Copyright Medieval Trading Dynasty. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/GTDataTypes.h"
#include "GTDynastySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterBorn, const FString&, CharacterID, const FString&, ParentID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterDied, const FString&, CharacterID, int32, Age, const FString&, CauseOfDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMarriageProposed, const FString&, ProposerID, const FString&, TargetID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMarriageAccepted, const FString&, Spouse1ID, const FString&, Spouse2ID, float, DowryCost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterComingOfAge, const FString&, CharacterID);

/**
 * Manages dynasty and character system
 * - Character aging and life events
 * - Marriage and children
 * - Traits and skill development
 * - Family tree management
 * - Succession and inheritance
 */
UCLASS()
class GAMETRADE_API UGTDynastySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Character creation
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	FString CreateCharacter(const FString& FirstName, const FString& FamilyName, bool bIsMale, int32 Age);

	// Marriage
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool ProposeMarriage(const FString& ProposerID, const FString& TargetID, float DowryOffered);

	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool AcceptMarriage(const FString& ProposerID, const FString& TargetID);

	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool DivorceMarriage(const FString& Character1ID, const FString& Character2ID);

	// Children
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	FString GiveBirth(const FString& MotherID, const FString& FatherID);

	// Aging and death
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	void ProcessAgingTurn();

	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool KillCharacter(const FString& CharacterID, const FString& CauseOfDeath);

	// Character development
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	void ImproveSkill(const FString& CharacterID, const FString& SkillName, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool AddTrait(const FString& CharacterID, ECharacterTrait Trait);

	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	bool RemoveTrait(const FString& CharacterID, ECharacterTrait Trait);

	// Education
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	void EducateChild(const FString& CharacterID, const FString& Focus); // Commerce, Stewardship, etc.

	// Queries
	UFUNCTION(BlueprintPure, Category = "Dynasty")
	FCharacterData GetCharacter(const FString& CharacterID) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	TArray<FCharacterData> GetFamilyMembers(const FString& FamilyName) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	TArray<FCharacterData> GetChildren(const FString& ParentID) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	bool IsMarried(const FString& CharacterID) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	bool CanMarry(const FString& Character1ID, const FString& Character2ID) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	int32 GetSkillValue(const FString& CharacterID, const FString& SkillName) const;

	UFUNCTION(BlueprintPure, Category = "Dynasty")
	bool HasTrait(const FString& CharacterID, ECharacterTrait Trait) const;

	// Succession
	UFUNCTION(BlueprintCallable, Category = "Dynasty")
	FString DetermineHeir(const FString& DeceasedID);

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Dynasty")
	FOnCharacterBorn OnCharacterBorn;

	UPROPERTY(BlueprintAssignable, Category = "Dynasty")
	FOnCharacterDied OnCharacterDied;

	UPROPERTY(BlueprintAssignable, Category = "Dynasty")
	FOnMarriageProposed OnMarriageProposed;

	UPROPERTY(BlueprintAssignable, Category = "Dynasty")
	FOnMarriageAccepted OnMarriageAccepted;

	UPROPERTY(BlueprintAssignable, Category = "Dynasty")
	FOnCharacterComingOfAge OnCharacterComingOfAge;

protected:
	// Data
	UPROPERTY()
	TMap<FString, FCharacterData> Characters;

	// Helper functions
	FString GenerateCharacterID() const;
	void AgeCharacter(FCharacterData& Character);
	bool RollDeathChance(const FCharacterData& Character) const;
	void InheritTraits(FCharacterData& Child, const FCharacterData& Parent1, const FCharacterData& Parent2);
	float CalculateMarriageSuccess(const FString& ProposerID, const FString& TargetID) const;
	void GenerateRandomTraits(FCharacterData& Character, int32 NumTraits);

	// Name generation
	TArray<FString> MaleNames = {TEXT("Hans"), TEXT("Friedrich"), TEXT("Klaus"), TEXT("Heinrich"), TEXT("Wilhelm"), TEXT("Otto"), TEXT("Karl"), TEXT("Ludwig")};
	TArray<FString> FemaleNames = {TEXT("Greta"), TEXT("Anna"), TEXT("Marie"), TEXT("Elisabeth"), TEXT("Sophia"), TEXT("Margarethe"), TEXT("Katharina")};
};
