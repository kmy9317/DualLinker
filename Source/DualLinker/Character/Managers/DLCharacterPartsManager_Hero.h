// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCharacterPartsManager.h"
#include "DLCharacterPartsManager_Hero.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLCharacterPartsManager_Hero : public UDLCharacterPartsManager
{
	GENERATED_BODY()
public:
	UDLCharacterPartsManager_Hero();

protected:

	//- Begin DLCharacterPartsManager interface
	virtual void InitializeParts() override;
	//- End DLCharacterPartsManager interface
};
