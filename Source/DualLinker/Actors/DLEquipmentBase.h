#pragma once

#include "DualLinker/DLDefine.h"
#include "AbilitySystemInterface.h"

#include "DLEquipmentBase.generated.h"

class UDLItemInstance;
class UAbilitySystemComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Abstract)
class ADLEquipmentBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ADLEquipmentBase();

protected:
	//- Begin ActorInterface
	virtual void Destroyed() override;
	//- End ActorInterface

public:
	void Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType);

	//- Begin AbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//- End AbilitySystemInterface

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeBlockState(bool bShouldBlock);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ProcessEquip(UDLItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	void PlayEquipMontage();
	
private:
	void PropertyInitialization();

	void SetBlockingCollision();

public:
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	EEquipmentSlotType GetEquipmentSlotType() const { return EquipmentSlotType; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSoftObjectPtr<UAnimMontage> GetEquipMontage() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;
	
protected:
	UPROPERTY()
	int32 ItemTemplateID = INDEX_NONE;
	
	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	bool bCanBlock = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bInitialized = false;
	
};
