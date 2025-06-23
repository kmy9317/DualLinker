// Fill out your copyright notice in the Description page of Project Settings.


#include "DLItemTemplate.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR
#include "DualLinker/DLDefine.h"
#include "Fragments/DLItemFragment_Equippable_Weapon.h"

UDLItemTemplate::UDLItemTemplate()
{
}

#if WITH_EDITOR
EDataValidationResult UDLItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	// 아이템은 최소 1이상(무기(1로 제한), 물약 등등)
	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("MaxStackCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}

	// Equippable속성은 중복될 수 없기에 검사 진행.
	const UDLItemFragment_Equippable* FoundEquippable = nullptr;
	for (UDLItemFragment* Fragment : Fragments)
	{
		if (UDLItemFragment_Equippable* CurrentEquippable = Cast<UDLItemFragment_Equippable>(Fragment))
		{
			if (FoundEquippable)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Equippable Fragment"))));
				return EDataValidationResult::Invalid;
			}

			FoundEquippable = CurrentEquippable;
		}
	}

	if (FoundEquippable)
	{
		if (FoundEquippable->EquipmentType == EEquipmentType::Count)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Equipment Type is Invalid : [EEquipmentType::Count]"))));
			return EDataValidationResult::Invalid;
		}

		if (FoundEquippable->EquipmentType == EEquipmentType::Weapon)
		{
			const UDLItemFragment_Equippable_Weapon* WeaponFragment = Cast<UDLItemFragment_Equippable_Weapon>(FoundEquippable);

			if (WeaponFragment->WeaponType == EWeaponType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [EWeaponType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}

			if (WeaponFragment->WeaponHandType == EWeaponHandType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [EWeaponHandType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}

		if (FoundEquippable->EquipmentType == EEquipmentType::Armor || FoundEquippable->EquipmentType == EEquipmentType::Weapon)
		{
			if (MaxStackCount != 1)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor or Weapon Type must have MaxStackCount of 1: [MaxStackCount != 1]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // WITH_EDITOR

const UDLItemFragment* UDLItemTemplate::FindFragmentByClass(TSubclassOf<UDLItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UDLItemFragment* Fragment : Fragments)
		{
			// Fragments를 순회하여 IsA()를 통해 해당 클래스를 가지고 있는지 확인한다:
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}