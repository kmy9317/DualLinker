// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DLPawnData.generated.h"

class UDLInputConfig;
class UDLCameraMode;

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    UDLPawnData();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
    TSubclassOf<APawn> PawnClass;  

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UDLInputConfig> InputConfig; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    TSubclassOf<UDLCameraMode> DefaultCameraMode;
};
