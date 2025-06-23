// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DualLinker/AbilitySystem/Abilities/DLGameplayAbility.h"

#include "DLAbilitySystemComponent.generated.h"

class UDLGameplayAbility;

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * 어빌리티 액터 정보 초기화
	 * 어빌리티 시스템과 액터를 연결하고 스폰 시 활성화할 어빌리티들을 처리
	 */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	/**
	 * 조건부 어빌리티 취소 함수 타입 정의
	 * 특정 조건을 만족하는 어빌리티들을 선별적으로 취소하기 위한 함수 포인터
	 */
	typedef TFunctionRef<bool(const UDLGameplayAbility* DLAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	
	/**
	 * 조건부 어빌리티 취소
	 * 주어진 조건 함수를 만족하는 모든 Activation 어빌리티를 취소
	 */
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
	
	void AbilityInputTagStarted(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	/**
	 * Activation Group 관리 함수들
	 * 어빌리티 간 상호 배타적 관계를 관리하는 핵심 시스템
	 */

	/**
	 * Activation Group 블록 상태 확인
	 * 특정 ActivaionGroup 이 현재 블록되어 있는지 확인
	 */
	bool IsActivationGroupBlocked(EDLAbilityActivationGroup Group) const;

	/**
	 * 어빌리티를 Activation Group에 추가
	 * 새로운 어빌리티가 활성화될 때 해당 그룹에 등록하고 필요시 기존 어빌리티들 취소
	 */
	void AddAbilityToActivationGroup(EDLAbilityActivationGroup Group, UDLGameplayAbility* DLAbility);

	/**
	 * 어빌리티를 Activation Group에서 제거
	 * 어빌리티가 종료될 때 해당 그룹에서 제거
	 */
	void RemoveAbilityFromActivationGroup(EDLAbilityActivationGroup Group, UDLGameplayAbility* DLAbility);

	/**
	 * Activation Group의 어빌리티들 취소
	 * 특정 그룹의 모든 어빌리티를 취소 (특정 어빌리티 제외 가능)
	 */
	void CancelActivationGroupAbilities(EDLAbilityActivationGroup Group, UDLGameplayAbility* IgnoreDLAbility, bool bReplicateCancelAbility/*Cancel Ability Replicate 유무*/);

	/**
	 * 스폰 시 어빌리티 활성화 시도
	 * InitAbilityActorInfo 호출 시 자동으로 활성화되어야 하는 어빌리티들을 처리
	 */
	void TryActivateAbilitiesOnSpawn();
	
protected:
	
	virtual void AbilitySpecInputStarted(FGameplayAbilitySpec& Spec);
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	/**
	 * 어빌리티 활성화 알림 (엔진 오버라이드)
	 * 어빌리티가 성공적으로 활성화되었을 때 호출
	 * Activation 그룹 관리 및 태그 관계 처리
	 */
	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;

	/**
	 * 어빌리티 활성화 실패 알림 (엔진 오버라이드)
	 * 어빌리티 활성화가 실패했을 때 호출
	 * 클라이언트에 실패 사유 전달
	 */
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;

	/**
	 * 어빌리티 종료 알림 (엔진 오버라이드)
	 * 어빌리티가 종료되었을 때 호출
	 * Activation 그룹에서 제거 및 정리 작업
	 */
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	/**
	 * 클라이언트에 어빌리티 활성화 실패 알림 (네트워크 RPC)
	 * 서버에서 어빌리티 활성화가 실패했을 때 클라이언트에 알림
	 */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	/**
	 * 어빌리티 실패 처리 (내부 구현)
	 * 실패 로그 출력 및 추가 처리 로직
	 */
	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:
	
	TArray<FGameplayAbilitySpecHandle> InputStartedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	/**
	 * 각 Activation 그룹별 실행 중인 어빌리티 개수
	 * Activation 그룹 관리 및 블록 상태 확인에 사용
	 * 인덱스는 EDLAbilityActivationGroup값
	 */
	int32 ActivationGroupCounts[(uint8)EDLAbilityActivationGroup::MAX];
};
