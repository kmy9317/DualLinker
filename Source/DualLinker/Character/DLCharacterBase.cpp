// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCharacterBase.h"

ADLCharacterBase::ADLCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ADLCharacterBase::BeginPlay()
{
	Super::BeginPlay();
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



