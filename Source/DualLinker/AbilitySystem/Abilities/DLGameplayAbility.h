// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DLGameplayAbility.generated.h"

class UInputMappingContext;
class ADLPlayerController;
class UDLAbilitySystemComponent;
class ADLCharacterBase;
class UDLCameraMode;

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

UENUM(BlueprintType)
enum class EDLAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
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

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	UDLAbilitySystemComponent* GetDLAbilitySystemComponentFromActorInfo() const;

	ADLPlayerController* GetDLPlayerControllerFromActorInfo() const;
	
	EDLAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EDLAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintCallable, Category = "DL|Ability")
	void SetCameraMode(TSubclassOf<UDLCameraMode> CameraMode);

	// 어빌리티 종료시 필요에 따라 자동 호출
	UFUNCTION(BlueprintCallable, Category = "DL|Ability")
	void ClearCameraMode();

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}
	
protected:
	// Called when the ability fails to activate
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
	
	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	//~End of UGameplayAbility interface
	
	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

protected:
	TSubclassOf<UDLCameraMode> ActiveCameraMode;

	// Ability 동작 정책 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DL|Ability Activation")
	EDLAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DL|Ability Activation")
	EDLAbilityActivationGroup ActivationGroup;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DL|Ability")
	TObjectPtr<UInputMappingContext> InputMappingContext;

};
