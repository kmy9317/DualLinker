// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameplayAbility_Equipment.h"

#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DualLinker/Equipment/DLEquipManagerComponent.h"
#include "DualLinker/Item/Fragments/DLItemFragment_Equippable_Weapon.h"
#include "DualLinker/Item/DLItemInstance.h"
#include "DualLinker/Actors/DLEquipmentBase.h"


UDLGameplayAbility_Equipment::UDLGameplayAbility_Equipment()
{
}

void UDLGameplayAbility_Equipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 부모 클래스의 기본 어빌리티 활성화 로직 실행
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// CanActivateAbility에서 이미 검증했으므로 여기서는 안전하게 캐스팅 가능
	ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(ActorInfo->AvatarActor.Get());
	UDLEquipManagerComponent* EquipManager = DLPlayerCharacter->FindComponentByClass<UDLEquipManagerComponent>();

	// 장비 액터 연결 (CanActivateAbility에서는 검증만, 여기서는 실제 연결)
	for (FDLEquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		// 장비 타입별로 실제 장비 액터 연결
		switch (EquipmentInfo.EquipmentType)
		{
		case EEquipmentType::Armor:
			// 방어구: 액터 연결 불필요 (캐릭터에 직접 적용)
			break;
		case EEquipmentType::Weapon:
			// 무기: 실제 무기 액터 연결 (공격 처리에 필요)
			EquipmentInfo.EquipmentActor = EquipManager->GetEquippedActor(EquipmentInfo.WeaponHandType);
			break;
		case EEquipmentType::Utility:
			// 유틸리티: 실제 유틸리티 액터 연결
			EquipmentInfo.EquipmentActor = EquipManager->GetFirstEquippedActor();
			break;
		}
	}
}

bool UDLGameplayAbility_Equipment::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	// [1단계] 캐릭터 유효성 검사
	ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!DLPlayerCharacter) return false;

	// [2단계] 장비 매니저 컴포넌트 존재 확인
	UDLEquipManagerComponent* EquipManager = DLPlayerCharacter->FindComponentByClass<UDLEquipManagerComponent>();
	if (!EquipManager) return false;

	// [3단계] 요구되는 모든 장비의 장착 상태 및 타입 검증
	for (const FDLEquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		// 유효하지 않은 장비 타입인 경우 실패
		if (EquipmentInfo.EquipmentType == EEquipmentType::Count)
			return false;

		if (EquipmentInfo.EquipmentType == EEquipmentType::Armor)
		{
			// 방어구: 요구되는 방어구 타입이 장착되어 있는지 확인
			UDLItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.RequiredArmorType);
			if (!ItemInstance) return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Weapon)
		{
			// 무기: 손 타입에 맞는 무기가 장착되어 있는지 확인
			UDLItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.WeaponHandType);
			if (!ItemInstance) return false;

			const UDLItemFragment_Equippable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UDLItemFragment_Equippable_Weapon>();
			if (!WeaponFragment) return false;

			// 요구되는 무기 타입과 실제 장착된 무기 타입 일치 확인
			if (WeaponFragment->WeaponType != EquipmentInfo.RequiredWeaponType) return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Utility)
		{
			// 유틸리티: 첫 번째 장착된 유틸리티 확인
			UDLItemInstance* ItemInstance = EquipManager->GetFirstEquippedItemInstance();
			if (!ItemInstance) return false;

			// TODO: Utility Fragment 구현 및 검증
		}
	}

	// 모든 조건을 만족하면 활성화 가능
	return true;
}

ADLEquipmentBase* UDLGameplayAbility_Equipment::GetFirstEquipmentActor() const
{
	// 등록된 장비 정보 목록에서 첫 번째 유효한 장비 액터 반환
	// 단일 장비만 사용하는 어빌리티에서 편의 함수로 사용
	for (const FDLEquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (ADLEquipmentBase* EquipmentActor = EquipmentInfo.EquipmentActor.Get())
			return EquipmentActor;
	}
	return nullptr;
}

UDLItemInstance* UDLGameplayAbility_Equipment::GetEquipmentItemInstance(const ADLEquipmentBase* InEquipmentActor) const
{
	if (!InEquipmentActor) return nullptr;

	UDLEquipManagerComponent* EquipManager = GetDLCharacterFromActorInfo()->FindComponentByClass<UDLEquipManagerComponent>();
	if (!EquipManager) return nullptr;

	// 요청된 장비 액터와 일치하는 장비 정보 찾기
	for (const FDLEquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.EquipmentActor != InEquipmentActor)
			continue;

		// 일치하는 장비 액터의 슬롯 타입으로 아이템 인스턴스 반환
		return EquipManager->GetEquippedItemInstance(InEquipmentActor->GetEquipmentSlotType());
	}

	return nullptr;
}

int32 UDLGameplayAbility_Equipment::GetEquipmentStatValue(FGameplayTag InStatTag, const ADLEquipmentBase* InEquipmentActor) const
{
	// 입력 매개변수 유효성 검사
	if (InStatTag.IsValid() == false || InEquipmentActor == nullptr)
		return 0;

	// 장비 액터에 연결된 아이템 인스턴스에서 스탯 값 조회
	if (UDLItemInstance* ItemInstance = GetEquipmentItemInstance(InEquipmentActor))
	{
		return 0;
		// TODO : 아이템 인스턴스의 스탯 값 조회 로직 구현
		// return ItemInstance->GetStackCountByTag(InStatTag);
	}
	return 0;
}
