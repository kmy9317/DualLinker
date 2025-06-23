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
	* Map �ε� �� Experience ��ȯ�� ����, MapID�� ExperienceID�� Ȱ���Ͽ�, TravelRequest ����
	* const �Լ��� ��� BlueprintPure�� ����Ʈ�� true
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UTravelManager_TravelRequest* CreateTravelRequest() const;

	/** the specific map to load */
	// ���� ���� �ҷ��� ������ �ʿ� ���� id�� ������ �ִ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay expierence to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "DLExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
