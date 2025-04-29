// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DLPlayerState.generated.h"

class UDLExperienceDefinition;
class UDLPawnData;
class UDLAbilitySystemComponent;

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
	UDLAbilitySystemComponent* GetDLAbilitySystemComponent() const { return AbilitySystemComponent; }

	UPROPERTY()
	TObjectPtr<const UDLPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "DL|PlayerState")
	TObjectPtr<UDLAbilitySystemComponent> AbilitySystemComponent;
};
