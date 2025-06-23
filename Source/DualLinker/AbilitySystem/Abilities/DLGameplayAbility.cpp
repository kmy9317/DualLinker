// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility.h"

#include "EnhancedInputSubsystems.h"
#include "DualLinker/Character/DLCharacterBase.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/Player/DLPlayerController.h"

UDLGameplayAbility::UDLGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationPolicy = EDLAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EDLAbilityActivationGroup::Independent;
}

ADLCharacterBase* UDLGameplayAbility::GetDLCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADLCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UDLAbilitySystemComponent* UDLGameplayAbility::GetDLAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UDLAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

ADLPlayerController* UDLGameplayAbility::GetDLPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADLPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

void UDLGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EDLAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UDLGameplayAbility::SetCameraMode(TSubclassOf<UDLCameraMode> CameraMode)
{
	// TODO : Camera 모드 설정
}

void UDLGameplayAbility::ClearCameraMode()
{
	// TODO : Camera 모드 정리
}

void UDLGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	bool bSimpleFailureFound = false;
	// TODO : Fail관련 로그 호출
}

bool UDLGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UDLAbilitySystemComponent* DLASC = CastChecked<UDLAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (DLASC->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(FDLGameplayTags::Get().Ability_ActivateFail_ActivationGroup);
		}
		return false;
	}

	return true;
}

void UDLGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (InputMappingContext)
	{
		if (const APlayerController* PC = GetDLPlayerControllerFromActorInfo())
		{
			if (const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer()))
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					FModifyContextOptions Options;
					Options.bForceImmediately = true;
					Options.bIgnoreAllPressedKeysUntilRelease = true;
					Subsystem->AddMappingContext(InputMappingContext, 1, Options);
				}
			}
		}
	}
}

void UDLGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();

	if (InputMappingContext)
	{
		if (const APlayerController* PC = GetDLPlayerControllerFromActorInfo())
		{
			if (const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer()))
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					FModifyContextOptions Options;
					Options.bForceImmediately = true;
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					Subsystem->RemoveMappingContext(InputMappingContext, Options);
				}
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDLGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UDLGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}
