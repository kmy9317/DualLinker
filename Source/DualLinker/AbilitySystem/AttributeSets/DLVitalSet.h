// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAttributeSet.h"
#include "AbilitySystemComponent.h"

#include "DLVitalSet.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLVitalSet : public UDLAttributeSet
{
	GENERATED_BODY()
public:
	UDLVitalSet();

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

public:
	mutable FDLAttributeEvent OnHealthChanged; 
};
