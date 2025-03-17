// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLPlayerCharacter.h"
#include "DLSiegCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLSiegCharacter : public ADLPlayerCharacter
{
	GENERATED_BODY()

public:
    ADLSiegCharacter();

protected:
    virtual void BeginPlay() override;

public:
    void HeavyAttack();  // 대검 강공격
};
