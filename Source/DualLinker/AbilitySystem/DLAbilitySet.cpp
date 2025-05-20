// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAbilitySet.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DualLinker/AbilitySystem/Abilities/DLGameplayAbility.h"

UDLAbilitySet::UDLAbilitySet()
{

}

void FDLAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FDLAbilitySet_GrantedHandles::TakeFromAbilitySystem(UDLAbilitySystemComponent* DLASC)
{
	check(DLASC);

	// 부여된 Ability Clear
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			DLASC->ClearAbility(Handle);
		}
	}

	AbilitySpecHandles.Reset();
}


void UDLAbilitySet::GiveToAbilitySystem(UDLAbilitySystemComponent* DLASC, FDLAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(DLASC);

	// AbilitySet의 Granted GA를 부여(InputTag와 Level 매핑)
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FDLAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability)) continue;

		UDLGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UDLGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = DLASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}

