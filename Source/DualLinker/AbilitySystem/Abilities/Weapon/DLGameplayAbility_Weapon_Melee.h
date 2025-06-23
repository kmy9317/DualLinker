// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DualLinker/AbilitySystem/Abilities/Hero/DLGameplayAbility_Equipment.h"
#include "DLGameplayAbility_Weapon_Melee.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility_Weapon_Melee : public UDLGameplayAbility_Equipment
{
	GENERATED_BODY()
public:
	UDLGameplayAbility_Weapon_Melee();

protected:
	/**
	 * 어빌리티 활성화 시 호출
	 * 부모 클래스의 장비 검증 후 히트 액터 캐시 초기화
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	/* 타겟 데이터 분석*/
	UFUNCTION()
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION()
	void ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage, ADLEquipmentBase* WeaponActor);

	/*개별 히트 결과를 처리하여 데미지와 이펙트 적용 */
	UFUNCTION()
	void ResetHitActors();
	
	/**
	 * 현재 공격에서 이미 히트된 액터들의 캐시(동일 공격에서 같은 대상을 여러 번 타격하는 것을 방지)
	 * WeakObjectPtr 사용으로 메모리 누수 방지
	 */
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedHitActors;
};
