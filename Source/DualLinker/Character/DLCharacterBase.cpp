// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCharacterBase.h"
#include "DualLinker/Player/DLPlayerState.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "Managers/DLCharacterPartsManager.h"
#include "Managers/DLCharacterPartsManager_Hero.h"

ADLCharacterBase::ADLCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

    CharacterPartsManager = CreateDefaultSubobject<UDLCharacterPartsManager>(TEXT("CharacterPartsManager"));
   
}

void ADLCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADLCharacterBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    CharacterPartsManager->OwnerCharacterType = CharacterType;
}

UAbilitySystemComponent* ADLCharacterBase::GetAbilitySystemComponent() const
{
    if (AbilitySystemComponent) return AbilitySystemComponent;

    if (ADLPlayerState* PS = GetPlayerState<ADLPlayerState>())
    {
        if (UAbilitySystemComponent* ASC = PS->GetDLAbilitySystemComponent())
        {
            return AbilitySystemComponent = Cast<UDLAbilitySystemComponent>(ASC);
        }      
    }

    return nullptr;
}

void ADLCharacterBase::TakeDamage(float Damage)
{
    Health -= Damage;
    if (Health <= 0.0f)
    {
        // 사망 처리 (추후 구현)
    }
}

void ADLCharacterBase::Heal(float HealAmount)
{
    Health += HealAmount;
    if (Health > MaxHealth)
    {
        Health = MaxHealth;
    }
}

void ADLCharacterBase::Attack()
{
    // 기본 공격 (추후 상속 클래스에서 구현)
}



