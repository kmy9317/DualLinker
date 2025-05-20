// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility.h"
#include "DualLinker/Character/DLCharacterBase.h"

UDLGameplayAbility::UDLGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationPolicy = EDLAbilityActivationPolicy::OnInputTriggered;
}

ADLCharacterBase* UDLGameplayAbility::GetDLCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADLCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
