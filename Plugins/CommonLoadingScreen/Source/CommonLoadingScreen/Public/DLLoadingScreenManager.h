// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DLLoadingScreenManager.generated.h"

class FLoadingScreenInputPreProcessor;

/**
 * 
 */
UCLASS(config=Game)
class COMMONLOADINGSCREEN_API UDLLoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
    // 서브시스템 초기화
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    //~FTickableObjectBase interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDLLoadingScreenManager, STATGROUP_Tickables); }
    virtual UWorld* GetTickableGameObjectWorld() const override;
    //~End of FTickableObjectBase interface

    /** 로딩 스크린 표시 요청 */
    UFUNCTION(BlueprintCallable)
    void ShowLoadingScreen();

    /** 로딩 스크린 숨김 요청 */
    UFUNCTION(BlueprintCallable)
    void HideLoadingScreen();

private:

    void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
    void HandlePostLoadMap(UWorld* LoadedWorld);
    void UpdateLoadingScreenVisibility();

    bool ShouldShowLoadingScreen();
    bool CheckForAnyNeedToShowLoadingScreen();

    void StartBlockingInput();
    void StopBlockingInput();

    void ChangePerformanceSettings(bool bEnabingLoadingScreen);

    

    /** 로딩 스크린 위젯 클래스 */
    UPROPERTY(Config, EditDefaultsOnly, Category = "Loading")
    FSoftClassPath LoadingScreenWidgetClass;

    /** 현재 표시 중인 로딩 위젯 */
    TSharedPtr<SWidget> ActiveLoadingWidget;

    TSharedPtr<FLoadingScreenInputPreProcessor> InputPreProcessor;

    double TimeLoadingScreenLastDismissed = -1.0;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Loading")
    float HoldLoadingScreenDuration = 2.0f;

    FString DebugReasonForShowingOrHidingLoadingScreen;

    bool bCurrentlyInLoadMap = false;
    bool bCurrentlyShowingLoadingScreen = false;
};
