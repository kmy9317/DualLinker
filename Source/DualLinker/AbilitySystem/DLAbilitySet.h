// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "DLAbilitySet.generated.h"

class UDLAbilitySystemComponent;
class UDLGameplayAbility;

/**
 * Ability trigger?? ???? InputTag?? ???? Ability Level?? ?????? ?????
 */
USTRUCT(BlueprintType)
struct FDLAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDLGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * FDLAbilitySet_GrantedHandles
 * AbilitySet?? ????? Granted?? Ability???? Handle?? 
 */
USTRUCT(BlueprintType)
struct FDLAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	// AbilitySpecHandles ĳ?? ???
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	// ?ο??? Ability ???? ???
	void TakeFromAbilitySystem(UDLAbilitySystemComponent* DLASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class DUALLINKER_API UDLAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDLAbilitySet();

	// ASC?? Ability ?ο???? ???, Handles?? ?ο??? Ability ??????? ???? ????? ???
	void GiveToAbilitySystem(UDLAbilitySystemComponent* DLASC, FDLAbilitySet_GrantedHandles* OutGrantedHandles,
	                         UObject* SourceObject = nullptr) const;

	const TArray<FDLAbilitySet_GameplayAbility>& GetGrantedGameplayAbilities() const
	{
		return GrantedGameplayAbilities;
	}

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FDLAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
