// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TravelManagerSubsystem.generated.h"

UCLASS(BlueprintType)
class DUALLINKER_API UTravelManager_TravelRequest : public UObject
{
	GENERATED_BODY()
public:
	/** MapID -> TEXT 변환 */
	FString GetMapName() const;

	/** ServerTravel에 전달할 최종 URL을 생성한다 */
	FString ConstructTravelURL() const;

	/** 준비할 MapID (맵 경로) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	/** 전달할 CmdArgs (Experience 이름을 전달함) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

UCLASS()
class DUALLINKER_API UTravelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UTravelManagerSubsystem() {};

	UFUNCTION(BlueprintCallable, Category = Session)
	void RequestTravel(APlayerController* InPlayer, UTravelManager_TravelRequest* Request);

	FString PendingTravelURL;
};
