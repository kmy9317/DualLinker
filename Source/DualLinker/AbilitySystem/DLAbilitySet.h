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
 * Ability trigger를 위한 InputTag와 현재 Ability Level과 연관된 구조체
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
 * AbilitySet에 저장된 Granted된 Ability들의 Handle을 
 */
USTRUCT(BlueprintType)
struct FDLAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	// AbilitySpecHandles 캐싱 함수
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	// 부여된 Ability 제거 함수
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

	// ASC에 Ability 부여하는 함수, Handles에 부여된 Ability 저장하여 추후 제거에 사용
	void GiveToAbilitySystem(UDLAbilitySystemComponent* DLASC, FDLAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

	const TArray<FDLAbilitySet_GameplayAbility>& GetGrantedGameplayAbilities() const { return GrantedGameplayAbilities; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FDLAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
