#pragma once

#include "DualLinker/DLDefine.h"
#include "DLEquipManagerComponent.generated.h"

class ADLPlayerCharacter;
class ADLPlayerController;
class ADLEquipmentBase;
class UDLItemInstance;
class UDLEquipManagerComponent;
class UDLEquipmentManagerComponent;
class UAbilitySystemComponent;

/**
 * - 현재 장비 슬롯을 장착중인 경우 아이템(장비) 정보를 가지고 있는 Entry
 */
USTRUCT(BlueprintType)
struct FDLEquipEntry 
{
	GENERATED_BODY()

	friend struct FDLEquipList;
	friend class UDLEquipManagerComponent;

private:
	void Init(UDLItemInstance* InItemInstance);
	
	void Equip();
	void Unequip();

public:
	UDLItemInstance* GetItemInstance() const { return ItemInstance; }

	void SetEquipmentActor(ADLEquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
	ADLEquipmentBase* GetEquipmentActor() const { return SpawnedEquipmentActor; }
	
private:

	/** ItemTemplate을 통해 생성된 인스턴스 */
	UPROPERTY()
	TObjectPtr<UDLItemInstance> ItemInstance;

	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	/** 실제로 Spawn된 장비의 Actor */
	UPROPERTY()
	TObjectPtr<ADLEquipmentBase> SpawnedEquipmentActor;

	// TODO 부여된 AbilitySetHandle등과 같은 현재 어빌리티 관리

public:
	UPROPERTY()
	TObjectPtr<UDLEquipManagerComponent> EquipManager;
};

USTRUCT(BlueprintType)
struct FDLEquipList 
{
	GENERATED_BODY()

	friend class UDLEquipManagerComponent;

public:
	FDLEquipList() : EquipManager(nullptr) { }
	FDLEquipList(UDLEquipManagerComponent* InOwnerComponent) : EquipManager(InOwnerComponent) { }

private:
	void Equip(EEquipmentSlotType EquipmentSlotType, UDLItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);
	
public:
	TArray<FDLEquipEntry>& GetAllEntries() { return Entries; }
	
private:

	UPROPERTY()
	TArray<FDLEquipEntry> Entries;
	
	UPROPERTY()
	TObjectPtr<UDLEquipManagerComponent> EquipManager;
};

UCLASS(BlueprintType)
class UDLEquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UDLEquipManagerComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
		
public:
	void Equip(EEquipmentSlotType EquipmentSlotType, UDLItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);

	void EquipCurrentSlots();
	void UnequipCurrentSlots();
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeEquipState(EEquipState NewEquipState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeEquipState(EEquipState NewEquipState) const;

public:
	ADLPlayerCharacter* GetDLPlayerCharacter() const;
	ADLPlayerController* GetDLPlayerController() const;
	
	TArray<FDLEquipEntry>& GetAllEntries();
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UDLEquipmentManagerComponent* GetEquipmentManager() const;

	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EArmorType ArmorType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType);
	
	static EWeaponSlotType ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType);
	static EWeaponHandType ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType);
	static EArmorType ConvertToArmorType(EEquipmentSlotType EquipmentSlotType);
	static EUtilitySlotType ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType);
	static EEquipState ConvertToEquipState(EWeaponSlotType WeaponSlotType);
	static EEquipState ConvertToEquipState(EUtilitySlotType UtilitySlotType);

	static bool IsWeaponEquipState(EEquipState EquipState);
	static bool IsUtilityEquipState(EEquipState EquipState);

	static const TArray<EEquipmentSlotType>& GetEquipmentSlotsByEquipState(EEquipState EquipState);
	
	void ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments);
	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }
	EEquipState GetCurrentEquipState() const { return CurrentEquipState; }
	
	ADLEquipmentBase* GetFirstEquippedActor() const;
	ADLEquipmentBase* GetEquippedActor(EWeaponHandType WeaponHandType) const;
	void GetAllEquippedActors(TArray<ADLEquipmentBase*>& OutActors) const;
	
	UDLItemInstance* GetFirstEquippedItemInstance(bool bIgnoreArmor = true) const;
	UDLItemInstance* GetEquippedItemInstance(EArmorType ArmorType) const;
	UDLItemInstance* GetEquippedItemInstance(EWeaponHandType WeaponHandType) const;
	UDLItemInstance* GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const;

private:
	UPROPERTY()
	FDLEquipList EquipList;

	UPROPERTY()
	EEquipState CurrentEquipState = EEquipState::Count;

	UPROPERTY()
	bool bShouldHiddenEquipments = false;
};
