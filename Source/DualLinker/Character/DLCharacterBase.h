// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DLCharacterBase.generated.h"

UCLASS()
class DUALLINKER_API ADLCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
    ADLCharacterBase();

protected:
    virtual void BeginPlay() override;

public:

    // 체력 관련 변수
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

public:
    // 체력 관리 함수
    void TakeDamage(float Damage);
    void Heal(float HealAmount);

    // 기본 공격 (추후 상속받은 클래스에서 구현)
    virtual void Attack();
};
