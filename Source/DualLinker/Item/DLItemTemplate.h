// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DLItemTemplate.generated.h"

class UDLItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UDLItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UDLItemInstance* Instance) const {}
};

UCLASS(Blueprintable, Const, Abstract)
class DUALLINKER_API UDLItemTemplate : public UObject
{
	GENERATED_BODY()

public:
	UDLItemTemplate();

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = "false", meta = (DeterminesOutputType = "FragmentClass"))
	const UDLItemFragment* FindFragmentByClass(TSubclassOf<UDLItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackCount = 1;

	// Fragment를 통해 다양한 타입의 아이템 속성을 조합 가능
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UDLItemFragment>> Fragments;
};
