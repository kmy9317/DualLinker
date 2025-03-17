// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUALLINKER_API UCharacterControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UCharacterControlComponent();


protected:
    virtual void BeginPlay() override;

public:

private:


};
