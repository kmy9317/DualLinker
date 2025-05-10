// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DLEquipmentDefinition.generated.h"

class UDLEquipmentInstance;

USTRUCT()
struct FDLEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	/** Socket에서 어느정도 Transformation을 더할것인지 결정: (Rotation, Position, Scale) */
	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UDLEquipmentDefinition();

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<UDLEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FDLEquipmentActorToSpawn> ActorsToSpawn;
	
};
