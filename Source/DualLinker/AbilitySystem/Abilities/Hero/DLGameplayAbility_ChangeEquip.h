// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DualLinker/AbilitySystem/Abilities/DLGameplayAbility.h"
#include "DLGameplayAbility_ChangeEquip.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility_ChangeEquip : public UDLGameplayAbility
{
	GENERATED_BODY()
public:
	UDLGameplayAbility_ChangeEquip();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "D1|Change Equip")
	float EndDelay = 0.6f;
};
