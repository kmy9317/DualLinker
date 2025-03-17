// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCharacterBase.h"
#include "DLPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerCharacter : public ADLCharacterBase
{
	GENERATED_BODY()
public:
    ADLPlayerCharacter();

    virtual void Move(const FVector2D& InputVector);
    virtual void Look(const FVector2D& InputVector);
protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeed = 800.0f;


};
