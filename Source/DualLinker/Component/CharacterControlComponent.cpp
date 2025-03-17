// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterControlComponent.h"
#include "DualLinker/Character/DLSiegCharacter.h"
#include "DualLinker/Character/DLEdelCharacter.h"
#include "GameFramework/PlayerController.h"


UCharacterControlComponent::UCharacterControlComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterControlComponent::BeginPlay()
{
    Super::BeginPlay();
}



