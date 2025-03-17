// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"


#include "DLGameModeBase.generated.h"

class UDLExperienceDefinition;
class UDLPawnData;

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADLGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() final;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	const UDLPawnData* GetPawnDataForController(const AController* InController) const;

protected:
	virtual void BeginPlay() override;

	void RequestLoadingExperience();
	bool IsExperienceLoaded() const;
	
	UFUNCTION()
	void OnExperienceLoaded(const UDLExperienceDefinition* InExperienceDefinition);
};
