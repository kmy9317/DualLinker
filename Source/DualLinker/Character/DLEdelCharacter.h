// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLPlayerCharacter.h"
#include "DLEdelCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLEdelCharacter : public ADLPlayerCharacter
{
	GENERATED_BODY()
public:
    ADLEdelCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void BeginPlay() override;
};
