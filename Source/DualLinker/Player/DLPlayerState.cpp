// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerState.h"
#include "DualLinker/System/DLExperienceSubsystem.h"
#include "DualLinker/GameModes/DLGameModeBase.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"

ADLPlayerState::ADLPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ADLPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UDLExperienceSubsystem* ExperienceSubsystem = GetGameInstance()->GetSubsystem<UDLExperienceSubsystem>())
	{
		ExperienceSubsystem->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void ADLPlayerState::OnExperienceLoaded(const UDLExperienceDefinition* CurrentExperience)
{
	if (ADLGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ADLGameModeBase>())
	{
		const UDLPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		if (NewPawnData) SetPawnData(NewPawnData);
	}
}

void ADLPlayerState::SetPawnData(const UDLPawnData* InPawnData)
{
	if (InPawnData)
	{
		PawnData = InPawnData;
	}
}