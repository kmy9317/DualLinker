// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility_Weapon_MeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/Actors/DLEquipmentBase.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"

UDLGameplayAbility_Weapon_MeleeAttack::UDLGameplayAbility_Weapon_MeleeAttack()
{
	ActivationGroup = EDLAbilityActivationGroup::Exclusive_Replaceable;
}

void UDLGameplayAbility_Weapon_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 부모 클래스에서 기본 근접 공격 초기화 진행(HitActor 캐시 리셋 등)
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bBlocked = false;

	// [1단계] 타겟 데이터 수신 대기 태스크 설정
	// 무기의 트레이스 이벤트(GameplayEvent_Trace)를 대기
	// 애니메이션 노티파이에서 발생하는 트레이스 결과를 수신
	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FDLGameplayTags::Get().GameplayEvent_Trace, nullptr, false, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetDataReady);
		GameplayEventTask->ReadyForActivation();
	}

	// [2단계] 공격 애니메이션 몽타주 재생
	// 몽타주 완료 시 OnMontageFinished 호출
	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("MeleeAttack"), AttackMontage, 1.f, NAME_None, false, 1.f, 0.f, false))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}

	// [3단계] 몽타주 종료 이벤트 대기 태스크 설정
	// 애니메이션에서 특정 시점에 발생하는 이벤트를 감지
	// 콤보 연결 등의 추가 로직 처리를 위한 확장 포인트
	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FDLGameplayTags::Get().GameplayEvent_Montage_End, nullptr, true, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEventTriggered);
		GameplayEventTask->ReadyForActivation();
	}
}

void UDLGameplayAbility_Weapon_MeleeAttack::OnTargetDataReady(FGameplayEventData Payload)
{
	// 이미 블록된 상태라면 추가 히트 처리 무시
	// 한 번의 공격에서 여러 번 블록되는 것을 방지
	if (bBlocked) return;
	
	// Payload.Instigator는 trace를 수행한 무기 객체
	ADLEquipmentBase* WeaponActor = const_cast<ADLEquipmentBase*>(Cast<ADLEquipmentBase>(Payload.Instigator));
	if (!WeaponActor) return;

	// 어빌리티 시스템 컴포넌트 유효성 검사
	UDLAbilitySystemComponent* SourceASC = GetDLAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// 현재 어빌리티가 여전히 유효한지 확인
	// 어빌리티가 취소되었거나 종료된 경우 처리하지 않음
	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		// 타겟 데이터 핸들 이동 (성능 최적화)
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(Payload.TargetData)));

		// 히트 결과를 일반 공격과 블록된 공격으로 분류
		TArray<int32> CharacterHitIndexes;  // 일반적으로 적중한 캐릭터들
		TArray<int32> BlockHitIndexes;      // 블록된 히트들
		ParseTargetData(LocalTargetDataHandle, CharacterHitIndexes, BlockHitIndexes);

		// 무기의 기본 데미지 값 가져오기
		float Damage = GetEquipmentStatValue(FDLGameplayTags::Get().SetByCaller_BaseDamage, WeaponActor);

		// 블록된 히트가 있는 경우 우선 처리
		if (BlockHitIndexes.Num() > 0)
		{
			// 첫 번째 블록된 히트만 처리 (다중 블록 방지)
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndexes[0]]->GetHitResult());
			ProcessHitResult(HitResult, Damage, true, BackwardMontage, WeaponActor);
			bBlocked = true;  // 블록 상태로 설정하여 추가 히트 방지
		}
		else
		{
			// 일반 히트들을 모두 처리
			for (int32 CharqacterHitIndex : CharacterHitIndexes)
			{
				FHitResult HitResult = *LocalTargetDataHandle.Data[CharqacterHitIndex]->GetHitResult();
				ProcessHitResult(HitResult, Damage, false, nullptr, WeaponActor);
			}
		}
	}
}

void UDLGameplayAbility_Weapon_MeleeAttack::OnMontageEventTriggered(FGameplayEventData Payload)
{
	// 애니메이션 몽타주에서 발생한 이벤트를 HandleMontageEvent로 전달
	// 이벤트 기반 어빌리티 제어를 위한 중간 단계
	HandleMontageEvent(Payload);
}

// 기본 구현: 몽타주 이벤트 발생 시 어빌리티 종료
// 자식 클래스에서 오버라이드하여 콤보 연결 등의 추가 로직 구현 가능
// 예: MeleeCombo 클래스에서는 콤보 연결 조건을 검사
void UDLGameplayAbility_Weapon_MeleeAttack::HandleMontageEvent(FGameplayEventData Payload)
{
	OnMontageFinished();
}

void UDLGameplayAbility_Weapon_MeleeAttack::OnMontageFinished()
{
	// 어빌리티 정상 종료 (bReplicateEndAbility=true, bWasCancelled=false)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

