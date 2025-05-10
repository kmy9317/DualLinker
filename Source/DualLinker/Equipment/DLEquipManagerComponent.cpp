#include "DLEquipManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "DLEquipmentManagerComponent.h"
#include "DualLinker/DLGameplayTags.h"
#include "Engine/ActorChannel.h"
#include "DualLinker/Item/DLItemInstance.h"
#include "DualLinker/Item/Fragments/DLItemFragment_Equippable_Weapon.h"
#include "DualLinker/Actors/DLEquipmentBase.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DualLinker/Player/DLPlayerController.h"
#include "DualLinker/System/DLAssetManager.h"

#include "DualLinker/DLGameplayTags.h"

void FDLEquipEntry::Init(UDLItemInstance* InItemInstance)
{
	// 이미 같은 아이템 타입인 경우 로직 실행 필요X
	if (ItemInstance == InItemInstance)
		return;

	ItemInstance = InItemInstance;
	ItemInstance ? Equip() : Unequip();
}

void FDLEquipEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;
	
	ADLPlayerCharacter* DLCharacter = EquipManager->GetDLPlayerCharacter();
	if (DLCharacter == nullptr)
		return;

	const UDLItemFragment_Equippable* EquippableFragment = ItemInstance->FindFragmentByClass<UDLItemFragment_Equippable>();
	if (EquippableFragment == nullptr)
		return;
		
	UDLAbilitySystemComponent* ASC = Cast<UDLAbilitySystemComponent>(DLCharacter->GetAbilitySystemComponent());
	check(ASC);

	ASC->AddLooseGameplayTag(FDLGameplayTags::Get().Player_Status_Equipped);

	// TODO: 이전 Ability 제거 및 현재 장비의 어빌리티 부여

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		// 이전 장비의 Actor Destroy
		if (IsValid(SpawnedEquipmentActor))
		{
			SpawnedEquipmentActor->Destroy();
		}

		// 장착할 장비의 Actor Spawn
		const UDLItemFragment_Equippable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UDLItemFragment_Equippable_Attachment>();
		const FDLWeaponAttachInfo& AttachInfo = AttachmentFragment->WeaponAttachInfo;
		if (AttachInfo.SpawnWeaponClass)
		{
			UWorld* World = EquipManager->GetWorld();
			ADLEquipmentBase* NewWeaponActor = World->SpawnActorDeferred<ADLEquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, DLCharacter);
			NewWeaponActor->Init(ItemInstance->GetItemTemplateID(), EquipmentSlotType);
			NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
			NewWeaponActor->AttachToComponent(DLCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
			NewWeaponActor->SetActorHiddenInGame(EquipManager->ShouldHiddenEquipments());
			NewWeaponActor->FinishSpawning(FTransform::Identity, true);
		}
	}
}

void FDLEquipEntry::Unequip()
{
	// 실제 장비중인 장착물에 대한 Ability 제거, Stat 제거, Actor Destroy등을 실행하는 로직
	ADLPlayerCharacter* DLPlayerCharacter = EquipManager->GetDLPlayerCharacter();
	if (DLPlayerCharacter == nullptr) return;
	
	// 캐릭터의 ASC를 가져와 어빌리티, Stat 제거
	UDLAbilitySystemComponent* ASC = Cast< UDLAbilitySystemComponent>(DLPlayerCharacter->GetAbilitySystemComponent());
	check(ASC);

	ASC->RemoveLooseGameplayTag(FDLGameplayTags::Get().Player_Status_Equipped);
	// TODO: 부여된 Ability및 기타 스텟요소들 제거 로직 예정

	// 실제 소환된 장비의 Actor Destroy
	if (IsValid(SpawnedEquipmentActor))
	{
		SpawnedEquipmentActor->Destroy();
	}
}

void FDLEquipList::Equip(EEquipmentSlotType EquipmentSlotType, UDLItemInstance* ItemInstance)
{
	// EquipmentManager에서 건네준 SlotType에 해당하는 Entry에 Actor Spawn 및 Attachment 요청
	FDLEquipEntry& Entry = Entries[(int32)(EquipmentSlotType)];
	Entry.Init(ItemInstance);
}

void FDLEquipList::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	// EquipmentManager에서 건네준 SlotType에 해당하는 spawned된 장비에 대한 Unequip을 요청
	FDLEquipEntry& Entry = Entries[(int32)(EquipmentSlotType)];
	Entry.Init(nullptr);
}

UDLEquipManagerComponent::UDLEquipManagerComponent()
 : EquipList(this)
{
	bWantsInitializeComponent = true;
}

void UDLEquipManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		TArray<FDLEquipEntry>& Entries = EquipList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FDLEquipEntry& Entry = Entries[i];
			Entry.EquipManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
		}
	}
}

void UDLEquipManagerComponent::UninitializeComponent()
{
	for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
	{
		Unequip((EEquipmentSlotType)i);
	}
		
	Super::UninitializeComponent();
}

void UDLEquipManagerComponent::Equip(EEquipmentSlotType EquipmentSlotType, UDLItemInstance* ItemInstance)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
		return;
	
	// Unarmed인 경우 내부적으로 로직X
	EquipList.Equip(EquipmentSlotType, ItemInstance);
}

void UDLEquipManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	EquipList.Unequip(EquipmentSlotType);
}

void UDLEquipManagerComponent::EquipCurrentSlots()
{
	if (CurrentEquipState == EEquipState::Count)
		return;
	
	if (UDLEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		for (EEquipmentSlotType EquipmentSlotType : UDLEquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
		{
			Equip(EquipmentSlotType, EquipmentManager->GetItemInstance(EquipmentSlotType));
		}
	}
}

void UDLEquipManagerComponent::UnequipCurrentSlots()
{
	if (CurrentEquipState == EEquipState::Count)
		return;
	
	for (EEquipmentSlotType EquipmentSlotType : UDLEquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
	{
		Unequip(EquipmentSlotType);
	}
}

void UDLEquipManagerComponent::ChangeEquipState(EEquipState NewEquipState)
{
	if (CanChangeEquipState(NewEquipState))
	{
		// 장착 해제 상태
		if (CurrentEquipState == NewEquipState)
		{
			NewEquipState = EEquipState::Unarmed;
		}
		
		// CurrentEquipState 계열 장비들을 해제
		UnequipCurrentSlots();
		CurrentEquipState = NewEquipState;
		// NewEquipState 계열 장비들을 장착
		EquipCurrentSlots();
	}
}

bool UDLEquipManagerComponent::CanChangeEquipState(EEquipState NewEquipState) const
{
	// 현재 착용상태(Primary, Secondary, Utility)를 변경하는 로직
	if (NewEquipState == EEquipState::Count)
		return false;

	if (CurrentEquipState == EEquipState::Unarmed && NewEquipState == EEquipState::Unarmed)
		return false;

	if (CurrentEquipState == NewEquipState)
		return true;
	
	UDLEquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (EquipmentManager == nullptr)
		return false;
	
	return (EquipmentManager->IsAllEmpty(NewEquipState) == false);
}

ADLEquipmentBase* UDLEquipManagerComponent::GetFirstEquippedActor() const
{
	ADLEquipmentBase* EquipmentActor = nullptr;
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;

	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
			if (EquipmentActor)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
		}
	}
	
	return EquipmentActor;
}

ADLEquipmentBase* UDLEquipManagerComponent::GetEquippedActor(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedActor();
	
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetEquipmentActor() : nullptr;
}

void UDLEquipManagerComponent::GetAllEquippedActors(TArray<ADLEquipmentBase*>& OutActors) const
{
	OutActors.Reset();
	
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;
	
	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
			{
				OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
			}
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
		{
			OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
		}
	}
}

UDLItemInstance* UDLEquipManagerComponent::GetFirstEquippedItemInstance(bool bIgnoreArmor) const
{
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;

	if (bIgnoreArmor == false)
	{
		for (int i = 0; i < (int32)EArmorType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EArmorType)i);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			if (UDLItemInstance* ItemInstance = Entries[EntryIndex].GetItemInstance())
				return ItemInstance;
		}
	}

	UDLItemInstance* ItemInstance = nullptr;
	
	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			ItemInstance = Entries[EntryIndex].GetItemInstance();
			if (ItemInstance)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			ItemInstance = Entries[EntryIndex].GetItemInstance();
		}
	}
	
	return ItemInstance;
}

UDLItemInstance* UDLEquipManagerComponent::GetEquippedItemInstance(EArmorType ArmorType) const
{
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(ArmorType);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UDLItemInstance* UDLEquipManagerComponent::GetEquippedItemInstance(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedItemInstance();
	
	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UDLItemInstance* UDLEquipManagerComponent::GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const TArray<FDLEquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)EquipmentSlotType;
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

ADLPlayerCharacter* UDLEquipManagerComponent::GetDLPlayerCharacter() const
{
	return Cast<ADLPlayerCharacter>(GetOwner());
}

ADLPlayerController* UDLEquipManagerComponent::GetDLPlayerController() const
{
	if (ADLPlayerCharacter* DLPlayerCharacter = GetDLPlayerCharacter())
	{
		return DLPlayerCharacter->GetDLPlayerController();
	}
	return nullptr;
}

TArray<FDLEquipEntry>& UDLEquipManagerComponent::GetAllEntries()
{
	return EquipList.GetAllEntries();
}

UAbilitySystemComponent* UDLEquipManagerComponent::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

UDLEquipmentManagerComponent* UDLEquipManagerComponent::GetEquipmentManager() const
{
	if (ADLPlayerCharacter* DLPlayerCharacter = GetDLPlayerCharacter())
	{
		return DLPlayerCharacter->FindComponentByClass<UDLEquipmentManagerComponent>();
	}
	return nullptr;
}

EEquipmentSlotType UDLEquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (EquipState == EEquipState::Unarmed)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Unarmed_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Unarmed_RightHand; break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Utility_Primary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;
	}
	else if (EquipState == EEquipState::Utility_Secondary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;
	}
	else if (EquipState == EEquipState::Utility_Tertiary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;
	}
	else if (EquipState == EEquipState::Utility_Quaternary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;
	}
	
	return EquipmentSlotType;
}

EEquipmentSlotType UDLEquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (WeaponSlotType == EWeaponSlotType::Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	
	return EquipmentSlotType;
}

EEquipmentSlotType UDLEquipManagerComponent::ConvertToEquipmentSlotType(EArmorType ArmorType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (ArmorType)
	{
	case EArmorType::Helmet: EquipmentSlotType = EEquipmentSlotType::Helmet; break;
	case EArmorType::Chest:  EquipmentSlotType = EEquipmentSlotType::Chest;  break;
	case EArmorType::Legs:   EquipmentSlotType = EEquipmentSlotType::Legs;   break;
	case EArmorType::Hands:  EquipmentSlotType = EEquipmentSlotType::Hands;  break;
	case EArmorType::Foot:   EquipmentSlotType = EEquipmentSlotType::Foot;   break;
	}

	return EquipmentSlotType;
}

EEquipmentSlotType UDLEquipManagerComponent::ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;	break;
	case EUtilitySlotType::Secondary:	EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;	break;
	case EUtilitySlotType::Quaternary:	EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;	break;
	}
	
	return EquipmentSlotType;
}

EWeaponHandType UDLEquipManagerComponent::ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;
	
	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Unarmed_LeftHand:
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Secondary_LeftHand:
		WeaponHandType = EWeaponHandType::LeftHand;
		break;
	case EEquipmentSlotType::Unarmed_RightHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Secondary_RightHand:
		WeaponHandType = EWeaponHandType::RightHand;
		break;
	case EEquipmentSlotType::Primary_TwoHand:
	case EEquipmentSlotType::Secondary_TwoHand:
		WeaponHandType = EWeaponHandType::TwoHand;
		break;
	}

	return WeaponHandType;
}

EArmorType UDLEquipManagerComponent::ConvertToArmorType(EEquipmentSlotType EquipmentSlotType)
{
	EArmorType ArmorType = EArmorType::Count;
	
	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Helmet:	ArmorType = EArmorType::Helmet;	break;
	case EEquipmentSlotType::Chest:		ArmorType = EArmorType::Chest;	break;
	case EEquipmentSlotType::Legs:		ArmorType = EArmorType::Legs;	break;
	case EEquipmentSlotType::Hands:		ArmorType = EArmorType::Hands;	break;
	case EEquipmentSlotType::Foot:		ArmorType = EArmorType::Foot;	break;
	}

	return ArmorType;
}

EUtilitySlotType UDLEquipManagerComponent::ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType)
{
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Utility_Primary:		UtilitySlotType = EUtilitySlotType::Primary;	break;
	case EEquipmentSlotType::Utility_Secondary:		UtilitySlotType = EUtilitySlotType::Secondary;	break;
	case EEquipmentSlotType::Utility_Tertiary:		UtilitySlotType = EUtilitySlotType::Tertiary;	break;
	case EEquipmentSlotType::Utility_Quaternary:	UtilitySlotType = EUtilitySlotType::Quaternary;	break;
	}

	return UtilitySlotType;
}

EEquipState UDLEquipManagerComponent::ConvertToEquipState(EWeaponSlotType WeaponSlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (WeaponSlotType)
	{
	case EWeaponSlotType::Primary:		EquipState = EEquipState::Weapon_Primary;		break;
	case EWeaponSlotType::Secondary:	EquipState = EEquipState::Weapon_Secondary;		break;
	}

	return EquipState;
}

EEquipState UDLEquipManagerComponent::ConvertToEquipState(EUtilitySlotType UtilitySlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipState = EEquipState::Utility_Primary;		break;
	case EUtilitySlotType::Secondary:	EquipState = EEquipState::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipState = EEquipState::Utility_Tertiary;		break;
	case EUtilitySlotType::Quaternary:	EquipState = EEquipState::Utility_Quaternary;	break;
	}

	return EquipState;
}

bool UDLEquipManagerComponent::IsWeaponEquipState(EEquipState EquipState)
{
	return (EEquipState::Unarmed <= EquipState && EquipState <= EEquipState::Weapon_Secondary);
}

bool UDLEquipManagerComponent::IsUtilityEquipState(EEquipState EquipState)
{
	return (EEquipState::Utility_Primary <= EquipState && EquipState <= EEquipState::Utility_Quaternary);
}

const TArray<EEquipmentSlotType>& UDLEquipManagerComponent::GetEquipmentSlotsByEquipState(EEquipState EquipState)
{
	static const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByEquipState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Utility_Primary }, { EEquipmentSlotType::Utility_Secondary }, { EEquipmentSlotType::Utility_Tertiary }, { EEquipmentSlotType::Utility_Quaternary },
	};

	if (EquipmentSlotsByEquipState.IsValidIndex((int32)EquipState))
	{
		return EquipmentSlotsByEquipState[(int32)EquipState];
	}
	else
	{
		static const TArray<EEquipmentSlotType> EmptyEquipmentSlots;
		return EmptyEquipmentSlots;
	}
}

EWeaponSlotType UDLEquipManagerComponent::ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Primary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Primary;
		break;
	case EEquipmentSlotType::Secondary_LeftHand:
	case EEquipmentSlotType::Secondary_RightHand:
	case EEquipmentSlotType::Secondary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Secondary;
		break;
	}

	return WeaponSlotType;
}

void UDLEquipManagerComponent::ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments)
{
	bShouldHiddenEquipments = bNewShouldHiddenEquipments;

	TArray<ADLEquipmentBase*> OutEquippedActors;
	GetAllEquippedActors(OutEquippedActors);

	for (ADLEquipmentBase* WeaponActor : OutEquippedActors)
	{
		if (IsValid(WeaponActor))
		{
			WeaponActor->SetActorHiddenInGame(bShouldHiddenEquipments);
		}
	}
}
