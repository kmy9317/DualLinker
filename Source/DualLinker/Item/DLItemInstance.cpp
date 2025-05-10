#include "DLItemInstance.h"
#include "DLItemTemplate.h"
#include "DualLinker/Data/DLItemData.h"
#include "Fragments/DLItemFragment_Equippable_Attachment.h"

UDLItemInstance::UDLItemInstance()
{
}

void UDLItemInstance::Init(int32 InItemTemplateID)
{
	if (InItemTemplateID <= INDEX_NONE)
		return;

	ItemTemplateID = InItemTemplateID;

	const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
	for (const UDLItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			// 아이템의 Fragment속성별 추가 초기화 로직 진행
			Fragment->OnInstanceCreated(this);
		}
	}
}

const UDLItemFragment* UDLItemInstance::FindFragmentByClass(TSubclassOf<UDLItemFragment> FragmentClass) const
{
	if (ItemTemplateID > INDEX_NONE && FragmentClass)
	{
		const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
		return ItemTemplate.FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
