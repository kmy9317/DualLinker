// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DualLinker/DLDefine.h"
#include "DualLinker/AbilitySystem/Abilities/DLGameplayAbility.h"
#include "DLGameplayAbility_Equipment.generated.h"

class UDLItemInstance;
class UDLCameraMode;
class ADLEquipmentBase;

/**
 * 어빌리티가 요구하는 장비 정보를 정의하는 구조체
 * - 어빌리티 실행에 필요한 장비 타입과 세부 요구사항을 명시
 * - 에디터에서 조건부 표시를 통해 관련 필드만 노출
 * - 런타임에 실제 장착된 장비 액터와 연결
 */
USTRUCT(BlueprintType)
struct FDLEquipmentInfo
{
	GENERATED_BODY()

	/** 요구되는 장비의 기본 타입 (무기/방어구/유틸리티) */
	UPROPERTY(EditAnywhere, Category="DL|Equipment")
	EEquipmentType EquipmentType = EEquipmentType::Count;

	/** 방어구 타입인 경우 요구되는 세부 방어구 종류 */
	UPROPERTY(EditAnywhere, Category="DL|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Armor", EditConditionHides))
	EArmorType RequiredArmorType = EArmorType::Count;

	/** 무기 타입인 경우 요구되는 손 사용 방식 (한손/양손) */
	UPROPERTY(EditAnywhere, Category="DL|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	/** 무기 타입인 경우 요구되는 세부 무기 종류 */
	UPROPERTY(EditAnywhere, Category="DL|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;
	
	/** 유틸리티 타입인 경우 요구되는 세부 유틸리티 종류 */
	UPROPERTY(EditAnywhere, Category="DL|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Utility", EditConditionHides))
	EUtilityType RequiredUtilityType = EUtilityType::Count;
	
	// 런타임에 연결되는 실제 장비 액터
	UPROPERTY()
	TWeakObjectPtr<ADLEquipmentBase> EquipmentActor;
};

/**
 * 장비 기반 어빌리티의 기반 클래스
 * 1. [장비 검증] 어빌리티 활성화 전/후 필요한 장비 장착 상태 확인
 * 2. [성능 스냅샷] 어빌리티 시작 시점의 공격 속도 등 성능 수치 고정
 * 3. [장비 연동] 실제 장착된 장비 액터와 아이템 인스턴스 연결
 * 4. [스탯 조회] 장비의 스탯 정보를 게임플레이 태그 기반으로 조회
 * 5. [카메라 모드] 어빌리티별 전용 카메라 모드 지원
 * 핵심 특징:
 * - 다중 장비 요구사항 지원 (무기+방어구+유틸리티 조합 가능)
 * - 성능 수치 스냅샷으로 어빌리티 실행 중 일관성 보장
 * - 장비 타입별 조건부 검증 시스템
 */
UCLASS()
class DUALLINKER_API UDLGameplayAbility_Equipment : public UDLGameplayAbility
{
	GENERATED_BODY()
public:
	UDLGameplayAbility_Equipment();

protected:
	/**
	 * 어빌리티 활성화 시 호출
	 * 1. 캐릭터 및 장비 매니저 유효성 검사
	 * 2. 요구되는 모든 장비의 장착 상태 확인
	 * 3. 장비 액터와 아이템 인스턴스 연결
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 어빌리티 활성화 가능 여부 검사
	 * 1. 부모 클래스의 기본 조건 확인
	 * 2. 캐릭터 및 장비 매니저 존재 확인
	 * 3. 요구되는 모든 장비의 장착 상태 확인
	 * 4. 장비 타입별 세부 조건 검증
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

public:
	/**
	 * 첫 번째 장비 액터 반환
	 * 단일 장비만 사용하는 어빌리티에서 편의 함수로 사용
	 */
	ADLEquipmentBase* GetFirstEquipmentActor() const;

	/**
	 * 특정 장비 액터에 연결된 아이템 인스턴스 반환
	 * 장비의 스탯이나 속성 정보 조회 시 사용
	 */
	UDLItemInstance* GetEquipmentItemInstance(const ADLEquipmentBase* InEquipmentActor) const;

	/**
	 * 장비의 특정 스탯 값 조회
	 * 게임플레이 태그 기반으로 데미지, 공격속도 등의 수치 반환
	 */
	int32 GetEquipmentStatValue(FGameplayTag InStatTag, const ADLEquipmentBase* InEquipmentActor) const;

protected:
	/**
	 * 이 어빌리티가 요구하는 장비 정보 목록
	 * 에디터에서 설정 가능하며, 다중 장비 조합 지원
	 */
	UPROPERTY(EditDefaultsOnly, Category="DL|Equipment")
	TArray<FDLEquipmentInfo> EquipmentInfos;

	/** 기본 공격 속도 (스탯 보정 전 기준값) */
	UPROPERTY(EditDefaultsOnly, Category="DL|Equipment")
	float DefaultAttackRate = 1.f;

	/** 이 어빌리티 실행 시 사용할 카메라 모드 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="DL|Equipment")
	TSubclassOf<UDLCameraMode> CameraModeClass;
};
