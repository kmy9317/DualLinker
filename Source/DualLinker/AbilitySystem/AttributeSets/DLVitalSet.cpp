// Fill out your copyright notice in the Description page of Project Settings.


#include "DLVitalSet.h"

UDLVitalSet::UDLVitalSet()
{

}

bool UDLVitalSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
		return false;

	return true;
}

void UDLVitalSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UDLVitalSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UDLVitalSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDLVitalSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
