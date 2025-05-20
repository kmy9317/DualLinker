// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DLGameplayAbility.generated.h"

class ADLCharacterBase;

UENUM(BlueprintType)
enum class EDLAbilityActivationPolicy : uint8
{
	Manual,

	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDLGameplayAbility();

	UFUNCTION(BlueprintCallable, Category = "DL|Ability")
	ADLCharacterBase* GetDLCharacterFromActorInfo() const;

protected:
	// Ability 동작 정책 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DL|Ability Activation")
	EDLAbilityActivationPolicy ActivationPolicy;

};
