// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerState.h"
#include "DualLinker/System/DLExperienceSubsystem.h"
#include "DualLinker/GameModes/DLGameModeBase.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DualLinker/AbilitySystem/DLAbilitySet.h"
#include "DualLinker/AbilitySystem/AttributeSets/DLVitalSet.h"

ADLPlayerState::ADLPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UDLVitalSet>(TEXT("VitalSet"));
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

void ADLPlayerState::ApplyAbilitySets(const TArray<UDLAbilitySet*>& AbilitySets)
{
	if (UDLAbilitySystemComponent* ASC = GetDLAbilitySystemComponent())
	{
		for (UDLAbilitySet* Set : AbilitySets)
		{
			if (IsValid(Set))
			{
				// OutHandles가 필요하면 두 번째 인자로 전달
				Set->GiveToAbilitySystem(ASC, /*OutGrantedHandles*/ nullptr);
			}
		}
	}
}
