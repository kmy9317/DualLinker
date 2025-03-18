// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DLExperienceSubsystem.generated.h"

class UDLExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExperienceLoaded, const UDLExperienceDefinition*);

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLExperienceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    /** 데이터 로드 시작 (이전 데이터의 에셋은 언로드) */
    void LoadExperience(TSoftObjectPtr<UDLExperienceDefinition> NewExperienceDefinition);

    // 아래의 OnExperienceLoaded에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출함
    void CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate);

    bool IsExperienceLoaded() const;
    const UDLExperienceDefinition* GetCurrentExperienceChecked() const;
    void ResetLoadingState();

    FOnExperienceLoaded OnExperienceLoaded;

private:
    /** 현재 로드 중인 데이터 */
    UPROPERTY()
    TObjectPtr<const UDLExperienceDefinition> CurrentExperience = nullptr;

    /** 이전 Experience에서 사용한 에셋들을 추적하여 불필요한 것들을 언로드 */
    TSet<FSoftObjectPath> PreviouslyLoadedAssets;

    void OnAssetLoaded();
    void UnloadPreviousExperienceAssets();

    bool bIsExperienceLoaded = false;
};
