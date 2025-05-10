// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DLEquipmentInstance.generated.h"

struct FDLEquipmentActorToSpawn;

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLEquipmentInstance : public UObject
{
	GENERATED_BODY()
public:
	UDLEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	UFUNCTION(BlueprintPure, Category = Equipment)
	UObject* GetInstigator() const { return Instigator; }

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	void SpawnEquipmentActors(const TArray<FDLEquipmentActorToSpawn>& ActorsToSpawn);
	void DestroyEquipmentActors();

	/**
	 * DeterminesOutputType은 C++ 정의에는 APawn* 반환하지만, BP에서는 PawnType에 따라 OutputType이 결정되도록 리다이렉트(Redirect)한다
	 */
	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	/**
	* interfaces
	*/
	virtual void OnEquipped();
	virtual void OnUnequipped();

	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	/** DLEquipementDefinition에 맞게 Spawn된 Actor Instance들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
