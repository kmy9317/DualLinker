// Fill out your copyright notice in the Description page of Project Settings.


#include "DLInputMappingSubsystem.h"
#include "Engine/AssetManager.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"

void UDLInputMappingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (!DefaultInputMappingPath.IsNull())
    {
        DefaultInputMapping = TSoftObjectPtr<UInputMappingContext>(DefaultInputMappingPath);
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(DefaultInputMappingPath, [this]()
            {
                UInputMappingContext* LoadedMapping = DefaultInputMapping.Get();
                if (LoadedMapping)
                {
                    UE_LOG(LogTemp, Log, TEXT("Default Input Mapping Loaded: %s"), *LoadedMapping->GetName());

                    // **로드 완료 후 델리게이트 브로드캐스트**
                    bDefaultMappingLoaded = true;
                    OnDefaultInputMappingLoaded.Broadcast(LoadedMapping);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Default Input Mapping Failed to Load!"));
                }
            });
    }
}

void UDLInputMappingSubsystem::ApplyDefaultInputMapping(APlayerController* PlayerController)
{
    if (!DefaultInputMapping.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT(" Default Input Mapping is not valid!"));
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        InputSubsystem->ClearAllMappings();
        InputSubsystem->AddMappingContext(DefaultInputMapping.LoadSynchronous(), 0);
        UE_LOG(LogTemp, Log, TEXT("Applied Default Input Mapping Context."));
    }
}

UInputMappingContext* UDLInputMappingSubsystem::GetDefaultInputMappingContext() const
{
    return bDefaultMappingLoaded ? DefaultInputMapping.Get() : nullptr;
}
