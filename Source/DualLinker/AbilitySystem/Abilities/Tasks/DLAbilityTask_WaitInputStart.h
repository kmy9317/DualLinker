// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DLAbilityTask_WaitInputStart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputStartDelegate);

/**
 * 새로운 입력 시작을 감지하는 어빌리티 태스크
 * 핵심 특징:
 * - 일회성 태스크: 입력 감지 시 즉시 종료
 * - 네트워크 안전: 클라이언트 예측과 서버 권한 모두 지원
 * - 이벤트 기반: GameCustom1 레플리케이트 이벤트를 통한 입력 감지
 * 동작 메커니즘:
 * 1. Activate() 호출 시 GameCustom1 이벤트에 delegate 등록
 * 2. DLAbilitySystemComponent::AbilitySecInputStarted()에서 이벤트 발생
 * 3. OnStartCallback() 호출되어 OnStart delegate 브로드캐스트
 * 4. EndTask() 호출로 태스크 정리 및 종료
 * 네트워크 처리:
 * - 클라이언트 예측: ServerSetReplicatedEvent로 서버에 알림
 * - 서버 권한: ConsumeGenericReplicatedEvent로 이벤트 소비
 * - 원격 클라이언트: 지연된 이벤트 처리를 위한 대기 상태 지원
*/
UCLASS()
class DUALLINKER_API UDLAbilityTask_WaitInputStart : public UAbilityTask
{
	GENERATED_BODY()
public:
	UDLAbilityTask_WaitInputStart();
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UDLAbilityTask_WaitInputStart* WaitInputStart(UGameplayAbility* OwningAbility);
	
public:
	virtual void Activate() override;
	
public:
	UFUNCTION()
	void OnStartCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FInputStartDelegate OnStart;

protected:
	/**
	 * GameCustom1 이벤트에 등록된 delegate 핸들
	 * OnStartCallback 호출 후 delegate 제거를 위해 사용
	 */
	FDelegateHandle DelegateHandle;
	
};
