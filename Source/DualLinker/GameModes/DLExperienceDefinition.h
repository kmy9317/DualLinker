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

	/** 맵에서 사용할 PawnData 리스트 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TArray<TSoftObjectPtr<UDLPawnData>> PawnDataList;
};
