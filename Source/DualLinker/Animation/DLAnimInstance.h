// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "DLAnimInstance.generated.h"

class ADLCharacterBase;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class DUALLINKER_API UDLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	
	//- Begin UAnimInstance's interface	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	//- End UAnimInstance's interface

	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:
	UPROPERTY()
	ADLCharacterBase* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
	/** GameplayTag와 AnimInstance의 속성값을 매핑해준다 */
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
