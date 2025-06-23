// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAbilitySystemComponent.h"

#include "Abilities/DLGameplayAbility.h"
#include "DualLinker/DLLogChannels.h"
#include "DualLinker/Animation/DLAnimInstance.h"

void UDLAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	// 새로운 Pawn Avatar가 설정되었는지 확인
	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	// 부모 클래스의 초기화 먼저 실행
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// DL 애니메이션 인스턴스가 있다면 어빌리티 시스템과 연결
		if (UDLAnimInstance* DLAnimInst = Cast<UDLAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			DLAnimInst->InitializeWithAbilitySystem(this);
		}

		// 스폰 시 자동으로 활성화되어야 하는 어빌리티들 처리
		TryActivateAbilitiesOnSpawn();
	}
}


void UDLAbilitySystemComponent::AbilitySpecInputStarted(FGameplayAbilitySpec& Spec)
{
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDLAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDLAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDLAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	if (UDLGameplayAbility* DLAbility = Cast<UDLGameplayAbility>(Ability))
	{
		AddAbilityToActivationGroup(DLAbility->GetActivationGroup(), DLAbility);
	}
}

void UDLAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

void UDLAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	if (UDLGameplayAbility* DLAbility = Cast<UDLGameplayAbility>(Ability))
	{
		RemoveAbilityFromActivationGroup(DLAbility->GetActivationGroup(), DLAbility);
	}
}

void UDLAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void UDLAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	if (const UDLGameplayAbility* DLAbility = Cast<const UDLGameplayAbility>(Ability))
	{
		DLAbility->OnAbilityFailedToActivate(FailureReason);
	}
}

void UDLAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	// 어빌리티 목록에 대한 스레드 안전 잠금
	// 다른 스레드에서 어빌리티 목록을 수정하는 것을 방지
	ABILITYLIST_SCOPE_LOCK();

	// 모든 활성화 가능한 어빌리티 스펙을 순회
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// 1단계: 활성화 상태 확인
		// 비활성화된 어빌리티는 취소할 필요가 없으므로 건너뜀
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		// 2단계: DL 어빌리티 타입 확인
		// DL 프로젝트에서는 모든 어빌리티가 UDLGameplayAbility를 상속해야 함
		UDLGameplayAbility* DLAbilityCDO = Cast<UDLGameplayAbility>(AbilitySpec.Ability);
		if (!DLAbilityCDO)
		{
			UE_LOG(LogDLAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Non-DLGameplayAbility %s was Granted to ASC. Skipping."), *AbilitySpec.Ability.GetName());
			continue;
		}

		// 3단계: 인스턴싱 정책에 따른 분기 처리
		if (DLAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// 인스턴스화된 어빌리티 처리 (InstancedPerActor, InstancedPerExecution)
			// CDO가 아닌 실제 생성된 인스턴스들을 취소해야 함
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UDLGameplayAbility* DLAbilityInstance = CastChecked<UDLGameplayAbility>(AbilityInstance);

				// 4단계: 조건 함수로 취소 여부 판단
				// 사용자 정의 조건 함수에 어빌리티 인스턴스와 핸들을 전달하여 취소 여부 결정
				if (ShouldCancelFunc(DLAbilityInstance, AbilitySpec.Handle))
				{
					// 5단계: 취소 가능 여부 확인
					// 어빌리티가 현재 취소 가능한 상태인지 확인 (예: 중요한 애니메이션 중이 아닌지)
					if (DLAbilityInstance->CanBeCanceled())
					{
						// 6단계: 실제 어빌리티 취소 실행
						// 어빌리티 인스턴스의 CancelAbility 메서드 호출
						DLAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), DLAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						// 취소할 수 없는 어빌리티인 경우 에러 로그 출력
						// 예: SetCanBeCanceled(false)로 설정된 중요한 어빌리티
						UE_LOG(LogDLAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *DLAbilityInstance->GetName());
					}
				}
			}
		}
		// CDO 자체가 실행되므로 CDO를 직접 취소
		else
		{
			// 4단계: 조건 함수로 취소 여부 판단.
			if (ShouldCancelFunc(DLAbilityCDO, AbilitySpec.Handle))
			{
				// 5단계: 취소 가능 여부 확인
				// 비인스턴스화된 어빌리티는 항상 취소 가능해야 함 (설계 규칙)
				check(DLAbilityCDO->CanBeCanceled());

				// 6단계: 실제 어빌리티 취소 실행
				// CDO의 CancelAbility 메서드 호출 (빈 ActivationInfo 사용)
				DLAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void UDLAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UDLGameplayAbility* DLAbilityCDO = Cast<UDLGameplayAbility>(AbilitySpec.Ability))
		{
			DLAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void UDLAbilitySystemComponent::AbilityInputTagStarted(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputStartedSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UDLAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UDLAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UDLAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// 이번 프레임에 활성화할 어빌리티들을 저장할 정적 배열
	// 정적 변수로 선언하여 매 프레임마다 메모리 할당을 피함
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UDLGameplayAbility* DLAbilityCDO = Cast<UDLGameplayAbility>(AbilitySpec->Ability);
				// 지속 입력으로 활성화되는 어빌리티들 처리 (WhileInputActive 정책)
				if (DLAbilityCDO && DLAbilityCDO->GetActivationPolicy() == EDLAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}
	
	// 입력 시작 이벤트 처리 (Started 이벤트 → GameCustom1 이벤트 발생)
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputStartedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				// 활성화된 어빌리티만 Started 이벤트를 전달
				if (AbilitySpec->IsActive())
				{
					// GameCustom1 Replicated 이벤트 발생 
					AbilitySpecInputStarted(*AbilitySpec);
				}
			}
		}
	}
	
	// 입력 pressed 이벤트 처리 (Pressed 이벤트 → 활성화 vs 입력 이벤트 분기)
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				// Ability Spec의 InputPressed 플래그 설정
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive()) // 현재 활성화된 어빌리티인 경우
				{
					// 입력 이벤트만 전달
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else // 어빌리티가 비활성화된 경우
				{
					// 어빌리티가 비활성화 된 경우에만 새로 활성화 시도
					const UDLGameplayAbility* DLAbilityCDO = Cast<UDLGameplayAbility>(AbilitySpec->Ability);

					// OnInputTriggered Policy 어빌리티만 입력으로 활성화 가능
					if (DLAbilityCDO && DLAbilityCDO->GetActivationPolicy() == EDLAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// 큐잉된 모든 어빌리티들 일괄 활성화
	// Held와 Pressed 입력을 한 번에 처리하여 중복 활성화 방지
	// Held 입력이 어빌리티를 활성화한 후 Pressed 입력이 같은 어빌리티에 입력 이벤트를 보내는 것을 방지
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		// 실제 어빌리티 활성화 시도
		// 여기서 CanActivateAbility 체크, Activation Group 관리 등이 수행됨
		TryActivateAbility(AbilitySpecHandle);
	}

	// Release 입력 이벤트 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				// Ability Spec의 InputPressed 플래그 해제
				AbilitySpec->InputPressed = false;

				// 활성화된 어빌리티만 Release 이벤트를 전달
				if (AbilitySpec->IsActive())
				{
					// InputReleased Replicated 이벤트 발생 
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	
	// 캐시된 입력들 초기화
	// InputHeldSpecHandles는 입력이 Released될 때까지 유지
	InputStartedSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

bool UDLAbilitySystemComponent::IsActivationGroupBlocked(EDLAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EDLAbilityActivationGroup::Independent:
		// Independent 어빌리티는 절대 블록되지 않음
		// 다른 어빌리티와 독립적으로 실행 가능
		bBlocked = false;
		break;

	case EDLAbilityActivationGroup::Exclusive_Replaceable:
	case EDLAbilityActivationGroup::Exclusive_Blocking:
		// Exclusive 어빌리티들은 Exclusive_Blocking 그룹이 활성화되어 있으면 블록됨
		// Exclusive_Blocking은 다른 모든 Exclusive 어빌리티를 블록
		bBlocked = (ActivationGroupCounts[(uint8)EDLAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

void UDLAbilitySystemComponent::AddAbilityToActivationGroup(EDLAbilityActivationGroup Group, UDLGameplayAbility* DLAbility)
{
	check(DLAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);
	
	// 해당 그룹의 활성화된 어빌리티 개수 증가
	ActivationGroupCounts[(uint8)Group]++;

	// 취소 Replicate는 하지 않음 (로컬에서만 처리)
	const bool bReplicateCancelAbility = false;

	// 그룹별 동작:
	// - Independent: 다른 어빌리티에 영향 없이 추가
	// - Exclusive_Replaceable/Blocking: 기존 Exclusive_Replaceable 어빌리티들 자동 취소
	
	switch (Group)
	{
	case EDLAbilityActivationGroup::Independent:
		// Independent 어빌리티는 다른 어빌리티를 취소하지 않음
		// 독립적으로 실행되므로 추가 처리 없음
		break;

	case EDLAbilityActivationGroup::Exclusive_Replaceable:
	case EDLAbilityActivationGroup::Exclusive_Blocking:
		// 새로운 Exclusive 어빌리티 활성화 시 기존 Exclusive_Replaceable 어빌리티들 자동 취소
		// 이것이 콤보 시스템에서 이전 콤보가 자동으로 취소되는 메커니즘!
		CancelActivationGroupAbilities(EDLAbilityActivationGroup::Exclusive_Replaceable, DLAbility, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	// 안전성 검사: Exclusive 어빌리티는 동시에 하나만 실행되어야 함
	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EDLAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)EDLAbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogDLAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

void UDLAbilitySystemComponent::RemoveAbilityFromActivationGroup(EDLAbilityActivationGroup Group, UDLGameplayAbility* DLAbility)
{
	check(DLAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	// 해당 그룹의 Activation 어빌리티 개수 감소
	ActivationGroupCounts[(uint8)Group]--;
}

void UDLAbilitySystemComponent::CancelActivationGroupAbilities(EDLAbilityActivationGroup Group, UDLGameplayAbility* IgnoreDLAbility, bool bReplicateCancelAbility)
{
	// 취소 조건을 정의하는 람다 함수(IgnoreDLAbility는 보통 새로 활성화 되는 어빌리티)
	auto ShouldCancelFunc = [this, Group, IgnoreDLAbility](const UDLGameplayAbility* DLAbility, FGameplayAbilitySpecHandle Handle)
	{
		// 같은 Activation 그룹이면서 새로 활성화되는 어빌리티가 아닌 경우에만 취소
		// 이를 통해 새로운 어빌리티는 보호하면서 기존 어빌리티들만 취소
		return ((DLAbility->GetActivationGroup() == Group) && (DLAbility != IgnoreDLAbility));
	};

	// 조건부 어빌리티 취소 실행
	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}




