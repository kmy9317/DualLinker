// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAttributeSet.h"

#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"

UDLAttributeSet::UDLAttributeSet()
{

}

UWorld* UDLAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UDLAbilitySystemComponent* UDLAttributeSet::GetDLAbilitySystemComponent() const
{
	return Cast<UDLAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}