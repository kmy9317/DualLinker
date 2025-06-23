// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DLExperienceDefinition.generated.h"

class UDLPawnData;

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDLExperienceDefinition();

	/** ????? ????? PawnData ????? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TArray<TSoftObjectPtr<UDLPawnData>> PawnDataList;
};
