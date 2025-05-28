// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DualLinker/DLDefine.h"

#include "DLPlayerState.generated.h"

class UDLExperienceDefinition;
class UDLPawnData;
class UDLAbilitySystemComponent;
class UDLAbilitySet;

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADLPlayerState();

	virtual void PostInitializeComponents() final;
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void OnExperienceLoaded(const UDLExperienceDefinition* CurrentExperience);
	void SetPawnData(const UDLPawnData* InPawnData);
	void ApplyAbilitySets(const TArray<UDLAbilitySet*>& AbilitySets);
	UDLAbilitySystemComponent* GetDLAbilitySystemComponent() const { return AbilitySystemComponent; }

	// TEMP -> 디폴트를 count로 변경 후 다른 위치에서 값 초기화 하도록 함
	ECharacterType CurrentCharacterType = ECharacterType::Edel;

	UPROPERTY()
	TObjectPtr<const UDLPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "DL|PlayerState")
	TObjectPtr<UDLAbilitySystemComponent> AbilitySystemComponent;
};
