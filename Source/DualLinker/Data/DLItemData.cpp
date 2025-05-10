#include "DLItemData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "DualLinker/Item/DLItemTemplate.h"
#include "DualLinker/Item/Fragments/DLItemFragment_Equippable_Weapon.h"
#include "DualLinker/System/DLAssetManager.h"
#include "UObject/ObjectSaveContext.h"

const UDLItemData& UDLItemData::Get()
{
	return UDLAssetManager::Get().GetItemData();
}

#if WITH_EDITORONLY_DATA
void UDLItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B)
	{
		return A < B;
	});

	ItemTemplateClassToID.Empty();
	WeaponItemTemplateClasses.Empty();

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);

		const UDLItemTemplate* ItemTemplate = Pair.Value.GetDefaultObject();
		if (const UDLItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UDLItemFragment_Equippable_Weapon>())
		{
			if (WeaponFragment->WeaponType != EWeaponType::Unarmed)
			{
				WeaponItemTemplateClasses.Add(Pair.Value);
			}
		}
	}
}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
EDataValidationResult UDLItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<int32> ItemTemplateIDSet;
	TSet<TSubclassOf<UDLItemTemplate>> ItemTemplateClassSet;

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check
		const int32 ItemTemplateID = Pair.Key;
		
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateIDSet.Add(ItemTemplateID);

		// Class Check
		const TSubclassOf<UDLItemTemplate> ItemTemplateClass = Pair.Value;
		
		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateClassSet.Add(ItemTemplateClass);
	}
	return Result;
}
#endif // WITH_EDITOR

const UDLItemTemplate& UDLItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<UDLItemTemplate>* ItemTemplateClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemTemplateClass, TEXT("Can't find ItemTemplateClass from ID [%d]"), ItemTemplateID);
	return *(ItemTemplateClass->GetDefaultObject());
}

int32 UDLItemData::FindItemTemplateIDByClass(TSubclassOf<UDLItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}

void UDLItemData::GetAllItemTemplateClasses(TArray<TSubclassOf<UDLItemTemplate>>& OutItemTemplateClasses) const
{
	OutItemTemplateClasses.Reset();
	OutItemTemplateClasses.Reserve(ItemTemplateIDToClass.Num());
	
	for (auto& Pair : ItemTemplateIDToClass)
	{
		OutItemTemplateClasses.Add(Pair.Value);
	}
}
