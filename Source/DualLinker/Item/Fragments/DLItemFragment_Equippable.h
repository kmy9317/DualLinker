#pragma once

#include "GameplayTags.h"
#include "DualLinker/DLDefine.h"
#include "DualLinker/Item/DLItemTemplate.h"
#include "DLItemFragment_Equippable.generated.h"

class UDLItemInstance;

UCLASS(Abstract, Const)
class UDLItemFragment_Equippable : public UDLItemFragment
{
	GENERATED_BODY()
	
public:
	UDLItemFragment_Equippable();

	bool IsEquippableClassType(ECharacterType ClassType) const;

public:
	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	// 해당 장비를 장착 가능한 캐릭터 유형들을 비트마스크를 활용하여 에디터에서 지정
	UPROPERTY(EditDefaultsOnly, Category="Equippable", meta=(Bitmask, BitmaskEnum="/Script/DualLinker.ECharacterType"))
	uint32 EquippableClassFlags = (1 << (uint32)ECharacterType::Count) - 1;
};
