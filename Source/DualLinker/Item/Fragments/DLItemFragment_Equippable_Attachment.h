#pragma once

#include "DLItemFragment_Equippable.h"
#include "DualLinker/DLDefine.h"
#include "DLItemFragment_Equippable_Attachment.generated.h"

class ADLEquipmentBase;

USTRUCT(BlueprintType)
struct FDLWeaponAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADLEquipmentBase> SpawnWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;
};

UCLASS(Abstract, Const)
class UDLItemFragment_Equippable_Attachment : public UDLItemFragment_Equippable
{
	GENERATED_BODY()
	
public:
	UDLItemFragment_Equippable_Attachment();

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	// Spawn할 장비 클래스, 소켓 위치등을 담당
	UPROPERTY(EditDefaultsOnly)
	FDLWeaponAttachInfo WeaponAttachInfo;

	/*
	* Animations
	*/
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> FrontHitMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BackHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> LeftHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> RightHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BlockHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> AnimLayerClass;
};
