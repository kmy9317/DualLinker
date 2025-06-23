// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DualLinker/DLDefine.h"
#include "AbilitySystemInterface.h"

#include "DLCharacterBase.generated.h"

class UDLCharacterPartsManager;
class UDLAbilitySystemComponent;

UCLASS()
class DUALLINKER_API ADLCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
    ADLCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Parts")
    TObjectPtr<UDLCharacterPartsManager> CharacterPartsManager = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Types")
    ECharacterType CharacterType = ECharacterType::Count;

public:
    UPROPERTY()
    mutable UDLAbilitySystemComponent* AbilitySystemComponent;

    //- Begin ActorInterface
    virtual void PostInitializeComponents() override;
    //- End ActorInterface

    // 체력 관리 함수
    void TakeDamage(float Damage);
    void Heal(float HealAmount);
    ECharacterType GetCharacterType() const { return CharacterType; }

    // 기본 공격 (추후 상속받은 클래스에서 구현)
    virtual void Attack();

};
