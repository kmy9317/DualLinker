// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DualLinker/DLDefine.h"
#include "DualLinker/Item/DLItemTemplate.h"

#include "DLEquipmentManagerComponent.generated.h"

class UDLAbilitySystemComponent;
class UDLEquipmentManagerComponent;
class UDLEquipManagerComponent;
class ADLPlayerCharacter;
class ADLPlayerController;

/**
 * - 현재 장비 슬롯에 등록중인 아이템(장비) 정보
 */
USTRUCT(BlueprintType)
struct FDLEquipmentEntry
{
	GENERATED_BODY()

	friend class UDLEquipmentManagerComponent;

private:
	void Init(UDLItemInstance* InItemInstance, int32 InItemCount);
	UDLItemInstance* Reset();

public:
	UDLItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }

private:
	/** ItemTemplate을 통해 생성된 인스턴스 */
	UPROPERTY()
	TObjectPtr<UDLItemInstance> ItemInstance;

	/** 현재 장착물의 갯수 */
	UPROPERTY()
	int32 ItemCount = 0;

	/** 현재 장착물의 슬롯 타입*/
	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	/** 현재 Entry를 관리중인 매니저 캐싱*/
	UPROPERTY()
	TObjectPtr<UDLEquipmentManagerComponent> EquipmentManager;
};

/**
 * - EquipmentManager 초기화 시점에 각각의 장비 슬롯을 담당하는 Entry들을 관리하는 리스트를 생성
 * - DLEquipmentList는 아이템의 정보를 가진 Entry들을 관리.
 */
USTRUCT(BlueprintType)
struct FDLEquipmentList
{
	GENERATED_BODY()

	friend class UDLEquipmentManagerComponent;

public:
	FDLEquipmentList() : EquipmentManager(nullptr) { }
	FDLEquipmentList(UDLEquipmentManagerComponent* InOwnerComponent) : EquipmentManager(InOwnerComponent) { }

	UDLAbilitySystemComponent* GetAbilitySystemComponent() const;

	const TArray<FDLEquipmentEntry>& GetAllEntries() const { return Entries; }

private:
	/** 현재 장비들에 대한 관리 리스트 */
	UPROPERTY()
	TArray<FDLEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UDLEquipmentManagerComponent> EquipmentManager;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUALLINKER_API UDLEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDLEquipmentManagerComponent();

protected:
	virtual void InitializeComponent() override;

public:
	// 새로운 장착계열 아이템을 해당하는 슬롯에 장착해주는 함수
	void SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UDLItemTemplate> ItemTemplateClass, int32 ItemCount);

	// 초기 Unarmed 상태에 대한 Left, Right 장비 장착
	void AddUnarmedEquipments(TSubclassOf<UDLItemTemplate> LeftHandClass, TSubclassOf<UDLItemTemplate> RightHandClass);

	// 현재 장비 슬롯에 해당하는 Entry의 아이템 정보를 제거하는 로직(현재 장비중인 경우 EquipManager에서 제거)
	UDLItemInstance* RemoveEquipment(EEquipmentSlotType EquipmentSlotType, int32 ItemCount);

	ADLPlayerCharacter* GetCharacter() const;
	ADLPlayerController* GetPlayerController() const;
	UDLEquipManagerComponent* GetEquipManager() const;

	bool IsAllEmpty(EEquipState EquipState) const;
	UDLItemInstance* GetItemInstance(EEquipmentSlotType EquipmentSlotType) const;

	static bool IsWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsArmorSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsUtilitySlot(EEquipmentSlotType EquipmentSlotType);

	static bool IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState);
	static bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType);
	static bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType);

	static bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);

	UPROPERTY()
	FDLEquipmentList EquipmentList;
};
