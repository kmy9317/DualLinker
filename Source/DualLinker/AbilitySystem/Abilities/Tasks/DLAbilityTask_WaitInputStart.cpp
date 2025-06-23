// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAbilityTask_WaitInputStart.h"

#include "AbilitySystemComponent.h"



UDLAbilityTask_WaitInputStart::UDLAbilityTask_WaitInputStart()
{
}

UDLAbilityTask_WaitInputStart* UDLAbilityTask_WaitInputStart::WaitInputStart(UGameplayAbility* OwningAbility)
{
	// 새로운 어빌리티 태스크 인스턴스 생성
	// NewAbilityTask는 어빌리티와 태스크를 자동으로 연결하고 초기화
	UDLAbilityTask_WaitInputStart* Task = NewAbilityTask<UDLAbilityTask_WaitInputStart>(OwningAbility);
	return Task;
}

/**
* 1. AbilitySystemComponent에서 GameCustom1 이벤트에 대한 delegate 등록
* 2. 네트워크 환경에서 원격 클라이언트 처리
* 3. 이벤트 발생 시 OnStartCallback 호출되도록 설정
*/
void UDLAbilityTask_WaitInputStart::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		// GameCustom1 이벤트에 대한 delegate 등록
		// 이 이벤트는 DLAbilitySystemComponent::AbilitySpecInputStarted()에서 발생
		// 플레이어가 새로운 입력을 시작할 때 트리거됨
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UDLAbilityTask_WaitInputStart::OnStartCallback);

		// 원격 클라이언트(서버에서 실행되는 클라이언트 어빌리티)인 경우
		if (IsForRemoteClient())
		{
			// 이미 발생한 이벤트가 있는지 확인하고 즉시 처리
			// 네트워크 지연으로 인해 이벤트를 놓치는 것을 방지
			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
			{
				// 이벤트가 아직 발생하지 않았으므로 원격 플레이어 데이터 대기 상태로 설정
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UDLAbilityTask_WaitInputStart::OnStartCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (Ability == nullptr || ASC == nullptr)
		return;

	// 등록된 delegate 제거 - 일회성 이벤트이므로 더 이상 감지할 필요 없음
	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	// 네트워크 예측 윈도우 생성 - 클라이언트 예측과 서버 권한 처리
	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{
		// 클라이언트에서 예측 실행 중인 경우
		// 서버에 이벤트 발생을 알려 동기화 보장
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		// 서버에서 실행 중이거나 예측이 아닌 경우
		// 이벤트를 소비하여 중복 처리 방지
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	// 어빌리티 태스크 delegate 브로드캐스트 조건 확인
	// 어빌리티가 여전히 활성 상태이고 유효한 경우에만 브로드캐스트
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// OnStart delegate 브로드캐스트 - 연결된 어빌리티에 입력 시작 알림
		OnStart.Broadcast();
	}

	// 태스크 종료 - 일회성 태스크이므로 이벤트 처리 후 즉시 정리
	EndTask();
}


