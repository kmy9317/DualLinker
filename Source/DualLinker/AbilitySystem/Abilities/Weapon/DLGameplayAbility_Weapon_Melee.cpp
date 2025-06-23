// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility_Weapon_Melee.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DualLinker/Character/DLCharacterBase.h"


UDLGameplayAbility_Weapon_Melee::UDLGameplayAbility_Weapon_Melee()
{
	
}

void UDLGameplayAbility_Weapon_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 부모 클래스 호출 - 장비 검증 및 공격 속도 스냅샷 생성
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 새로운 공격 시작 시 이전 공격의 히트 기록 초기화
	// 동일 공격에서 같은 대상을 여러 번 타격하는 것을 방지
	ResetHitActors();
}

void UDLGameplayAbility_Weapon_Melee::ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle,
	TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes)
{
	// 트레이스 결과의 모든 히트 데이터를 순회하며 분류
	for (int32 i = 0; i < InTargetDataHandle.Data.Num(); i++)
	{
		const TSharedPtr<FGameplayAbilityTargetData>& TargetData = InTargetDataHandle.Data[i];

		if (FHitResult* HitResult = const_cast<FHitResult*>(TargetData->GetHitResult()))
		{
			if (AActor* HitActor = HitResult->GetActor())
			{
				// 히트된 액터에서 캐릭터 찾기
				// 직접 캐릭터이거나, 캐릭터가 소유한 액터(무기 등)일 수 있음
				ADLCharacterBase* TargetCharacter = Cast<ADLCharacterBase>(HitActor);
				if (!TargetCharacter)
				{
					TargetCharacter = Cast<ADLCharacterBase>(HitActor->GetOwner());
				}

				// 중복 히트 방지 - 이미 히트된 액터는 제외
				AActor* SelectedActor = TargetCharacter ? TargetCharacter : HitActor;
				if (CachedHitActors.Contains(SelectedActor))
					continue;

				// 히트 액터를 캐시에 추가하여 중복 방지
				CachedHitActors.Add(SelectedActor);

				// 캐릭터와 무기/오브젝트를 구분하여 분류
				if (TargetCharacter)
				{
					// 캐릭터: 캐릭터 히트 인덱스에 추가
					OutCharacterHitIndexes.Add(i);
				}
				else
				{
					// 무기나 기타 오브젝트: 블록으로 분류
					OutBlockHitIndexes.Add(i);
				}
			}
		}
	}
}

void UDLGameplayAbility_Weapon_Melee::ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit,
	UAnimMontage* BackwardMontage, ADLEquipmentBase* WeaponActor)
{
	// 어빌리티 시스템 컴포넌트 유효성 검사
	UDLAbilitySystemComponent* SourceASC = GetDLAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// TODO : 타겟에 GameplayEffect 및 GameplayCue 적용
}

void UDLGameplayAbility_Weapon_Melee::ResetHitActors()
{
	CachedHitActors.Reset();
}
