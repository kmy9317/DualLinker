// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility_ChangeEquip.h"
#include "DualLinker/DLDefine.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/Character/DLCharacterBase.h"
#include "DualLinker/Equipment/DLEquipManagerComponent.h"

UDLGameplayAbility_ChangeEquip::UDLGameplayAbility_ChangeEquip()
{
	ActivationPolicy = EDLAbilityActivationPolicy::Manual;

	SetAssetTags(FGameplayTagContainer(FDLGameplayTags::Get().Ability_ChangeEquip));
	ActivationOwnedTags.AddTag(FDLGameplayTags::Get().Player_Status_ChangeEquip);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = FDLGameplayTags::Get().GameplayEvent_ChangeEquip;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UDLGameplayAbility_ChangeEquip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ADLCharacterBase* DLCharacter = GetDLCharacterFromActorInfo();
	if (DLCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UDLEquipManagerComponent* EquipManager = DLCharacter->GetComponentByClass<UDLEquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	int32 NewEquipStateIndex = FMath::RoundToInt(TriggerEventData->EventMagnitude);
	EEquipState NewEquipState = (EEquipState)NewEquipStateIndex;

	if (EquipManager->CanChangeEquipState(NewEquipState))
	{	
		EquipManager->ChangeEquipState(NewEquipState);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			}, EndDelay, false);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
