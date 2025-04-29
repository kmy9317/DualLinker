// Fill out your copyright notice in the Description page of Project Settings.


#include "DLEnemyCharacter.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"

ADLEnemyCharacter::ADLEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}
