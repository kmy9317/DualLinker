// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DLInputMappingSubsystem.generated.h"

class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefaultInputMappingLoaded, UInputMappingContext*, LoadedInputMapping);

/**
 * 
 */
UCLASS(Config=Game)
class DUALLINKER_API UDLInputMappingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    /** 입력 매핑 서브시스템 초기화 */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** 기본 입력 매핑을 적용 */
    void ApplyDefaultInputMapping(APlayerController* PlayerController);

    /** 현재 기본 입력 매핑을 반환 */
    UInputMappingContext* GetDefaultInputMappingContext() const;

    /** Default Input Mapping 로드 완료 시 호출되는 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "Input")
    FOnDefaultInputMappingLoaded OnDefaultInputMappingLoaded;

private:
    /** 기본적으로 사용할 입력 매핑 */

    UPROPERTY(Config)
    FSoftObjectPath DefaultInputMappingPath;

    UPROPERTY()
    TSoftObjectPtr<UInputMappingContext> DefaultInputMapping;

    bool bDefaultMappingLoaded = false;
};
