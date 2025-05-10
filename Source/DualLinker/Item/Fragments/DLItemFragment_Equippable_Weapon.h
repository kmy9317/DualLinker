#pragma once

#include "DLItemFragment_Equippable_Attachment.h"
#include "DualLinker/DLDefine.h"
#include "DLItemFragment_Equippable_Weapon.generated.h"

class UDLItemInstance;

UCLASS()
class UDLItemFragment_Equippable_Weapon : public UDLItemFragment_Equippable_Attachment
{
	GENERATED_BODY()
	
public:
	UDLItemFragment_Equippable_Weapon();

	virtual void OnInstanceCreated(UDLItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	// 원거리 무기 아이템의 탄알과 같은 아이템 템플릿을 담당
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="WeaponType == EWeaponType::Gun", EditConditionHides))
	TSubclassOf<UDLItemTemplate> AmmoItemTemplateClass;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> AttackSound;
	
	// TODO 무기의 고유 스킬 AbilitySet 추가 고려
};
