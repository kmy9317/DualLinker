#pragma once

#include "DualLinker/DLDefine.h"
#include "DLItemInstance.generated.h"

class UDLItemFragment;

UCLASS(BlueprintType)
class UDLItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UDLItemInstance();

	void Init(int32 InItemTemplateID);

	UFUNCTION(BlueprintCallable)
	int32 GetItemTemplateID() const { return ItemTemplateID; }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType="FragmentClass"))
	const UDLItemFragment* FindFragmentByClass(TSubclassOf<UDLItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}
	
private:
	// 현재 아이템 인스턴스의 데이터를 가지고 있는 ItemTemplate을 ID 형태로 저장
	int32 ItemTemplateID = INDEX_NONE;

	// TODO 아이템의 갯수와 같은 상태값 구현
};
