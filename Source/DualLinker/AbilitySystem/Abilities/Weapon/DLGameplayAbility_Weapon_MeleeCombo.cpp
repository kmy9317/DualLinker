// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility_Weapon_MeleeCombo.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "DualLinker/AbilitySystem/Abilities/Tasks/DLAbilityTask_WaitInputStart.h"

UDLGameplayAbility_Weapon_MeleeCombo::UDLGameplayAbility_Weapon_MeleeCombo()
{
	// 이 어빌리티는 다른 어빌리티와 배타적이며, 새로운 어빌리티로 교체 가능
	// 콤보 시스템에서 이전 콤보를 새로운 콤보로 교체할 때 사용
	ActivationGroup = EDLAbilityActivationGroup::Exclusive_Replaceable;
}

void UDLGameplayAbility_Weapon_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 부모 클래스의 ActivateAbility 호출 (기본 근접 공격 로직 실행)
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 입력 상태 초기화 - 콤보 시작 시 깨끗한 상태로 시작
    bInputPressed = false;
    bInputReleased = false;

    // 입력 릴리즈 감지 태스크 설정
    // 플레이어가 공격 버튼을 언제 놓는지 추적
    if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
    {
    	InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
    	InputReleaseTask->ReadyForActivation();
    }

    // 콤보 연결을 위한 입력 모니터링 시작
    WaitInputContinue();  // 새로운 입력 시작 감지
    WaitInputStop();      // 입력 취소 감지
}

void UDLGameplayAbility_Weapon_MeleeCombo::HandleMontageEvent(FGameplayEventData Payload)
{
	// 콤보 연결 조건 검사:
	// 1. NextAbilityClass가 설정되어 있어야 함
	// 2. 입력이 눌려있거나 아직 릴리즈되지 않았어야 함
	// 3. 공격이 블록되지 않았어야 함
	bool bCanContinue = NextAbilityClass && (bInputPressed || bInputReleased == false) && (bBlocked == false);

	if (bCanContinue)
	{
		// 조건 만족 시 다음 콤보 어빌리티 활성화
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
		{
			AbilitySystemComponent->TryActivateAbilityByClass(NextAbilityClass);
		}
		// 현재 어빌리티 종료 (다음 콤보로 전환)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else if (bBlocked == false)
	{
		// 콤보 연결 불가능하고 블록되지 않은 경우 어빌리티 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	// bBlocked == true인 경우는 부모 클래스에서 처리됨
}

void UDLGameplayAbility_Weapon_MeleeCombo::WaitInputContinue()
{
	// 새로운 입력 시작을 감지하는 태스크 생성
	// 플레이어가 콤보 연결을 위해 추가 입력을 했는지 확인
	if (UDLAbilityTask_WaitInputStart* InputStartTask = UDLAbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UDLGameplayAbility_Weapon_MeleeCombo::WaitInputStop()
{
	// 입력 취소를 감지하는 태스크 생성
	// 플레이어가 콤보를 중단하려는 의도를 파악
	if (UAbilityTask_WaitConfirmCancel* InputConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this))
	{
		InputConfirmCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		InputConfirmCancelTask->ReadyForActivation();
	}
}

void UDLGameplayAbility_Weapon_MeleeCombo::OnInputReleased(float TimeHeld)
{
	// 입력 버튼이 릴리즈되었음을 기록
	// 콤보 연결 조건 판단에 사용됨
	// 물리적 버튼 상태 추적하여 해당 키가 릴리즈 되면 트리거
	bInputReleased = true;
}

void UDLGameplayAbility_Weapon_MeleeCombo::OnInputStart()
{
	// 새로운 입력이 감지되었음을 기록
	// 콤보 연결 의도를 나타냄
	bInputPressed = true;

	// 추가 입력을 계속 감지하기 위해 태스크 재설정
	WaitInputContinue();
}

void UDLGameplayAbility_Weapon_MeleeCombo::OnInputCancel()
{
	// 입력이 취소되었음을 기록
	// 콤보 중단 의도를 나타냄
	// 논리적 명령 추적 - 다른 시스템에서 명시적으로 Confirm/Cancel 호출 필요
	bInputPressed = false;

	// 입력 취소를 계속 감지하기 위해 태스크 재설정
	WaitInputStop();
}


