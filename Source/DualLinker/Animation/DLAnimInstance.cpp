// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "DualLinker/Character/DLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UDLAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
		if (ADLCharacterBase* Owner = Cast<ADLCharacterBase>(OwningActor))
		{
			OwningCharacter = Owner;
			OwningMovementComponent = OwningCharacter->GetCharacterMovement();
		}
	}
}

void UDLAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	// ASC의 내부를 관리하는 GameplayTag와 AnimInstance의 멤버 Property와 Delegate를 연결하여 값 변화에 대한 반영을 진행한다
	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UDLAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	// 매 프레임 매우 짧게 끝나는 계산(속도, 가속도, 태그 바인딩)
	// Worker thread 에서 실행되어도 안전한 로직
	if (!OwningCharacter || !OwningMovementComponent) return;

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
