// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLGameplayAbility_Weapon_Melee.h"
#include "DLGameplayAbility_Weapon_MeleeAttack.generated.h"

/** 
 * 동작 원리:
 * 1. [어빌리티 활성화] 공격 애니메이션 재생과 동시에 타겟 감지 시스템 활성화
 * 2. [타겟 감지] 무기의 트레이스 이벤트를 통해 적중한 대상들을 수집
 * 3. [히트 처리] 적중 대상을 분석하여 일반 공격/블록된 공격으로 분류
 * 4. [데미지 적용] 각 대상에게 적절한 데미지와 이펙트 적용
 * 5. [어빌리티 종료] 애니메이션 완료 또는 블록 시 어빌리티 종료
 * 핵심 특징:
 * - 이벤트 기반 타겟 감지 (애니메이션 노티파이와 연동)
 * - 블록 시스템 지원 (방향성 블록 판정)
 * - 중복 히트 방지 (동일 대상 다중 타격 방지)
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility_Weapon_MeleeAttack : public UDLGameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
public:
	UDLGameplayAbility_Weapon_MeleeAttack();
protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 애니메이션 몽타주 이벤트 처리
	 * 기본 구현에서는 어빌리티 종료만 수행
	 * 자식 클래스에서 콤보 연결 등의 추가 로직 구현 가능
	 */
	virtual void HandleMontageEvent(FGameplayEventData Payload);

private:
	/**
	 * 무기 트레이스에서 타겟 데이터가 준비되었을 때 호출
	 */
	UFUNCTION()
	void OnTargetDataReady(FGameplayEventData Payload);

	/**
	 * 애니메이션 몽타주에서 특정 이벤트가 트리거되었을 때 호출
	 * HandleMontageEvent로 전달하여 처리
	 */
	UFUNCTION()
	void OnMontageEventTriggered(FGameplayEventData Payload);

	/**
	 * 애니메이션 몽타주가 완료되었을 때 호출
	 * 서버에서만 어빌리티 종료 처리
	 */
	UFUNCTION()
	void OnMontageFinished();

protected:
	/** 공격 시 재생할 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 공격이 블록되었을 때 재생할 후진 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee Attack")
	TObjectPtr<UAnimMontage> BackwardMontage;

protected:
	/** 현재 공격이 블록되었는지 여부 (추가 히트 처리 방지용) */
	bool bBlocked = false;

	/** 블록 몽타주 타이머 핸들 (필요시 타이머 관리용) */
	FTimerHandle BlockMontageTimerHandle;
};
