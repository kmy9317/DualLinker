// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DLUserFacingExperience.generated.h"

class UTravelManager_TravelRequest;

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/**
	* Map 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하여, TravelRequest 생성
	* const 함수의 경우 BlueprintPure의 디폴트가 true
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UTravelManager_TravelRequest* CreateTravelRequest() const;

	/** the specific map to load */
	// 쉽게 말해 불러올 게임의 맵에 대한 id를 가지고 있다
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay expierence to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "DLExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
