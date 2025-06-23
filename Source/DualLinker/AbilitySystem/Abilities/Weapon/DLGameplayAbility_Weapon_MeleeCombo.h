// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLGameplayAbility_Weapon_MeleeAttack.h"
#include "DLGameplayAbility_Weapon_MeleeCombo.generated.h"

/**
 * 근접 무기 콤보 공격을 처리하는 GameplayAbility 클래스
 * - 연속적인 근접 공격 콤보 시스템 구현
 * - 플레이어 입력 타이밍에 따른 콤보 연결 관리
 * - 다음 콤보 어빌리티로의 자동 전환
 * 동작 방식:
 * 1. 어빌리티 활성화 시 입력 모니터링 시작
 * 2. 몽타주 이벤트 발생 시 콤보 연결 조건 검사
 * 3. 조건 만족 시 다음 콤보 어빌리티 활성화
 * 4. 조건 불만족 시 현재 어빌리티 종료
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility_Weapon_MeleeCombo : public UDLGameplayAbility_Weapon_MeleeAttack
{
	GENERATED_BODY()
public:
	UDLGameplayAbility_Weapon_MeleeCombo();

protected:
	/**
	 * - 입력 상태 초기화
	 * - 입력 모니터링 태스크들 설정
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 애니메이션 몽타주 이벤트 처리
	 * - 콤보 연결 조건 검사 (입력 상태, 블록 여부, 다음 어빌리티 존재)
	 * - 조건 만족 시 다음 콤보 어빌리티 활성화
	 */
	virtual void HandleMontageEvent(FGameplayEventData Payload) override;

private:
	/**
	 * 새로운 입력 시작을 대기하는 태스크 설정
	 * 플레이어가 추가 입력을 했는지 감지
	 */
	void WaitInputContinue();

	/**
	 * 입력 취소를 대기하는 태스크 설정
	 * 플레이어가 입력을 중단했는지 감지
	 */
	void WaitInputStop();

private:
	/**
	 * 입력 버튼이 릴리즈되었을 때 호출
	 * TimeHeld 버튼이 눌려있던 시간
	 */
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	/**
	 * 새로운 입력이 시작되었을 때 호출
	 * 콤보 연결을 위한 입력 감지
	 */
	UFUNCTION()
	void OnInputStart();

	/**
	 * 입력이 취소되었을 때 호출
	 * 콤보 중단 처리
	 */
	UFUNCTION()
	void OnInputCancel();

protected:
	/**
	 * 다음 콤보 단계에서 실행될 어빌리티 클래스
	 * 에디터에서 설정 가능한 콤보 체인 정의
	 */
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee Combo")
	TSubclassOf<UDLGameplayAbility_Weapon_MeleeCombo> NextAbilityClass;

private:
	/** 현재 입력이 눌려있는 상태인지 추적 */
	bool bInputPressed = false;

	/** 입력이 릴리즈된 상태인지 추적 */
	bool bInputReleased = false;
};
