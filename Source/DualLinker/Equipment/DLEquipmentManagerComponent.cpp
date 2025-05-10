// Fill out your copyright notice in the Description page of Project Settings.


#include "DLEquipmentManagerComponent.h"
#include "DLEquipManagerComponent.h"
#include "DualLinker/Data/DLItemData.h"
#include "DualLinker/Item/Fragments/DLItemFragment_Equippable_Weapon.h"
#include "DualLinker/Item/DLItemInstance.h"
#include "DualLinker/Player/DLPlayerController.h"
#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DLEquipmentDefinition.h"
#include "DLEquipmentInstance.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void FDLEquipmentEntry::Init(UDLItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);

	UDLEquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return;

	const UDLItemFragment_Equippable* EquippableFragment = InItemInstance->FindFragmentByClass<UDLItemFragment_Equippable>();
	if (EquippableFragment == nullptr)
		return;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	ItemInstance = InItemInstance;

	const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);

	// 현재 초기화하는 장비가 착용중인 장비 상태(EX: Primary계열 추가->현재 Primary 장비 장착 상태)라면 바로 착용
	if (EquippableFragment->EquipmentType == EEquipmentType::Armor || EquipmentManager->IsSameEquipState(EquipmentSlotType, EquipManager->GetCurrentEquipState()))
	{
		EquipManager->Equip(EquipmentSlotType, ItemInstance);
	}
}

UDLItemInstance* FDLEquipmentEntry::Reset()
{
	UDLEquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return nullptr;

	// 현재 Slot의 Item이 장비 목록에 등록중인 경우 EquipManager에 Unequip의뢰(현재 장착중인 경우 Unequip)
	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	UDLItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;
	
	// 무기 Unequip시 기존의 EquipState에 해당하는 무기 Slot들 중 어떠한 Slot들도 Instance가 존재하지 않다면 Unarmed상태로 변경
	if (EquipmentManager->IsAllEmpty(EquipManager->GetCurrentEquipState()))
	{
		EquipManager->ChangeEquipState(EEquipState::Unarmed);
	}

	return RemovedItemInstance;
}

UDLAbilitySystemComponent* FDLEquipmentList::GetAbilitySystemComponent() const
{
	check(EquipmentManager);
	AActor* OwningActor = EquipmentManager->GetOwner();

	return Cast<UDLAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UDLEquipmentManagerComponent::UDLEquipmentManagerComponent()
	: EquipmentList(this)
{
	bWantsInitializeComponent = true;
}

void UDLEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		TArray<FDLEquipmentEntry>& Entries = EquipmentList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);


		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FDLEquipmentEntry& Entry = Entries[i];
			Entry.EquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
		}
	}
}

void UDLEquipmentManagerComponent::SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UDLItemTemplate> ItemTemplateClass, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemTemplateClass == nullptr || ItemCount <= 0)
		return;

	const int32 ItemTemplateID = UDLItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	// 아이템 템플릿은 instance화 할 일종의 메타 데이터만 가지고 있기에 CDO형태로 불러온다
	const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);

	ItemCount = FMath::Clamp(ItemCount, 1, ItemTemplate.MaxStackCount);

	const UDLItemFragment_Equippable* EquippableFragment = ItemTemplate.FindFragmentByClass<UDLItemFragment_Equippable>();
	if (EquippableFragment == nullptr)
		return;

	// 현재 해당 슬롯을 Clear
	FDLEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	Entry.Reset();

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UDLItemFragment_Equippable_Weapon* WeaponFragment = Cast<UDLItemFragment_Equippable_Weapon>(EquippableFragment);
		EWeaponHandType WeaponHandType = WeaponFragment->WeaponHandType;

		if (IsPrimaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				// 현재 무기가 왼/오른손 타입이라면 기존의 양손 무기를 EquipmentList에서 제거, Equip중인 경우 EquipManager에서 Unequip
				RemoveEquipment(EEquipmentSlotType::Primary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				// 현재 무기가 양손 타입이라면 기존의 왼/오른손 무기들을 EquipmentList에서 제거, Equip중인 경우 EquipManager에서 Unequip
				RemoveEquipment(EEquipmentSlotType::Primary_LeftHand, 1);
				RemoveEquipment(EEquipmentSlotType::Primary_RightHand, 1);
			}
		}
		else if (IsSecondaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				RemoveEquipment(EEquipmentSlotType::Secondary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				RemoveEquipment(EEquipmentSlotType::Secondary_LeftHand, 1);
				RemoveEquipment(EEquipmentSlotType::Secondary_RightHand, 1);
			}
		}
	}

	UDLItemInstance* AddedItemInstance = NewObject<UDLItemInstance>();
	AddedItemInstance->Init(ItemTemplateID);
	Entry.Init(AddedItemInstance, ItemCount);

	if (IsWeaponSlot(EquipmentSlotType))
	{
		if (UDLEquipManagerComponent* EquipManager = GetEquipManager())
		{
			EWeaponSlotType WeaponSlotType = UDLEquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
			EEquipState EquipState = UDLEquipManagerComponent::ConvertToEquipState(WeaponSlotType);
			if (EquipManager->GetCurrentEquipState() != EquipState)
			{
				EquipManager->ChangeEquipState(EquipState);
			}
		}
	}
}

UDLItemInstance* UDLEquipmentManagerComponent::RemoveEquipment(EEquipmentSlotType EquipmentSlotType, int32 ItemCount)
{
	FDLEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	UDLItemInstance* ItemInstance = Entry.GetItemInstance();

	// 2개 이상의 착용 가능한 장착물이 아닌 1개의 count를 지닌 보통의 Weapon타입들은 해당 장비 destroy 로직 실행함.
	Entry.ItemCount -= ItemCount;

	if (Entry.ItemCount <= 0) Entry.Reset();
	return ItemInstance;
}

ADLPlayerCharacter* UDLEquipmentManagerComponent::GetCharacter() const
{
	return Cast<ADLPlayerCharacter>(GetOwner());
}

ADLPlayerController* UDLEquipmentManagerComponent::GetPlayerController() const
{
	if (ADLPlayerCharacter* DLPlayerCharacter = GetCharacter())
	{
		return DLPlayerCharacter->GetDLPlayerController();
	}
	return nullptr;
}

UDLEquipManagerComponent* UDLEquipmentManagerComponent::GetEquipManager() const
{
	UDLEquipManagerComponent* EquipManager = nullptr;
	if (ADLPlayerCharacter* Character = GetCharacter())
	{
		EquipManager = Character->FindComponentByClass<UDLEquipManagerComponent>();
	}
	return EquipManager;
}

bool UDLEquipmentManagerComponent::IsAllEmpty(EEquipState EquipState) const
{
	if (EquipState == EEquipState::Count)
		return true;

	if (EquipState == EEquipState::Unarmed)
		return false;

	bool bAllEmpty = true;
	for (EEquipmentSlotType SlotType : UDLEquipManagerComponent::GetEquipmentSlotsByEquipState(EquipState))
	{
		const FDLEquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		if (Entry.ItemInstance)
		{
			bAllEmpty = false;
			break;
		}
	}
	return bAllEmpty;
}

UDLItemInstance* UDLEquipmentManagerComponent::GetItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const TArray<FDLEquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FDLEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	return Entry.GetItemInstance();
}

bool UDLEquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Unarmed_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}

bool UDLEquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Helmet <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Foot);
}

bool UDLEquipmentManagerComponent::IsUtilitySlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Utility_Primary <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Utility_Quaternary);
}

bool UDLEquipmentManagerComponent::IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EEquipState::Unarmed) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Primary) ||
		((EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Primary && WeaponEquipState == EEquipState::Utility_Primary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Secondary && WeaponEquipState == EEquipState::Utility_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Tertiary && WeaponEquipState == EEquipState::Utility_Tertiary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Quaternary && WeaponEquipState == EEquipState::Utility_Quaternary));
}

bool UDLEquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand) && WeaponHandType == EWeaponHandType::LeftHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponHandType == EWeaponHandType::TwoHand));
}

bool UDLEquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet) || (EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest) ||
		(EquipmentSlotType == EEquipmentSlotType::Legs && ArmorType == EArmorType::Legs) || (EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hands) ||
		(EquipmentSlotType == EEquipmentSlotType::Foot && ArmorType == EArmorType::Foot));
}

bool UDLEquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Primary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Primary_TwoHand);
}

bool UDLEquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Secondary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}


